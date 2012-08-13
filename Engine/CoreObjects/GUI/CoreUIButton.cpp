//
//  CoreUIButton.cpp
//  CoreEngine3D
//
//  Created by Jody McAdams on 4/21/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#include "CoreUIButton.h"
#include "CoreUIImageView.h"

//Engine includes
#include "Math/MathUtil.h"
#include "Math/matrix.h"
#include "Game/Game.h"

//Game includes
#include "EngineModels.h"

static const u32 g_ImageType_ButtonIdle = 2002506047; //ButtonIdle
static const u32 g_ImageType_ButtonHover = 1657410501; //ButtonHover
static const u32 g_ImageType_ButtonPressed = 2873333591; //ButtonPressed

//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIButton::InitClass()
{
    //TODO: initialize shared class globals here
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CoreUIButton::LoadResourcesForType(u32 type)
{
    //TODO: call GAME->AddItemArt(&g_Art_SpaceShip)
	//TODO: call GAME->AddItemSound(&g_Sound_SpaceShipBeam)
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CoreUIButton::Init(u32 type)
{
    //Base class init
    CoreUIView::Init(type);
    
    //TODO: other init
	viewType = CoreUI_ViewType_Button;
	
	m_hView_Idle = 0;
	m_hView_Hover = 0;
	m_hView_Pressed = 0;
	
	m_buttonState = CoreUI_ButtonState_Idle;
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CoreUIButton::SpawnInit(void* pSpawnStruct, CoreObjectHandle hParent)
{
    pugi::xml_node* pProperties = (pugi::xml_node*)pSpawnStruct;
	
	//First do the standard spawn init to bring
	//in all the attributes
	CoreUIView::SpawnInit(pProperties, hParent);
    
	//TODO: button specific attributes
	
	//Cache the children that are button images
	for(u32 i=0; i<numChildren; ++i)
	{
		CoreUIView* pView = (CoreUIView*)COREOBJECTMANAGER->GetObjectByHandle(children[i]);
		if(pView->viewType == CoreUI_ViewType_ImageView)
		{
			CoreUIImageView* pImageView = (CoreUIImageView*)pView;
			CoreObjectAttribute_Char32* pCharAttrib = (CoreObjectAttribute_Char32*)pImageView->attributes.GetAttributeByByteIndex(pImageView->attrib_imageType);
			const u32 nameHash = pCharAttrib->hashedValue;
			
			switch(nameHash)
			{
				case g_ImageType_ButtonIdle:
				{
					m_hView_Idle = pView->GetHandle();
					
					break;
				}
				case g_ImageType_ButtonHover:
				{
					m_hView_Hover = pView->GetHandle();
					
					//Hover starts hidden
					pImageView->visible = false;
					pImageView->LayoutView();
					
					break;
				}
				case g_ImageType_ButtonPressed:
				{
					m_hView_Pressed = pView->GetHandle();
					
					//Pressed starts hidden
					pImageView->visible = false;
					pImageView->LayoutView();
					
					break;
				}
			}
		}
	}
	
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIButton::Uninit()
{
    //Base class uninit
    CoreGameObject::Uninit();
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIButton::Update(f32 timeElapsed)
{
    //TODO: update here
    CoreUIView::Update(timeElapsed);
	
#if defined(PLATFORM_OSX) || defined(PLATFORM_WIN) || defined(PLATFORM_LINUX)
	
	const bool pressed = GAME->m_mouseState.buttonState[0] == CoreInput_ButtonState_Began
	|| GAME->m_mouseState.buttonState[0] == CoreInput_ButtonState_Held;
	
	UpdateButton(pressed, &GAME->m_mouseState.position, &GAME->m_mouseState.position);

#elif defined(PLATFORM_IOS) || defined(PLATFORM_ANDROID)
	DeviceInputState* pInputState = GAME->GetDeviceInputState();
	for(u32 i=0; i<MAX_MULTITOUCH; ++i)
	{
		TouchState touchState = pInputState->GetTouchState(i);
		const bool pressed = touchState == TouchState_Began
		|| touchState == TouchState_Moving
		|| touchState == TouchState_Stationary;
		
		vec2 touchPosBegin;
		vec2 touchPosCurr;
		pInputState->GetTouchPos_Begin(i, &touchPosBegin);
		pInputState->GetTouchPos_Curr(i, &touchPosCurr);
		
		intVec2 touchPosBeginInt = {(s32)touchPosBegin.x,(s32)touchPosBegin.y};
		intVec2 touchPosCurrInt = {(s32)touchPosCurr.x,(s32)touchPosCurr.y};
		
		const bool buttonWasPressed = UpdateButton(pressed, &touchPosBeginInt, &touchPosCurrInt);
		if(buttonWasPressed)
		{
			//This button was pressed, so don't let any more touches
			//try to press it
			
			break;
		}
	}
#endif
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIButton::UpdatePointers()
{
    //TODO: update anything that has pointers to local
    //member variables, such as material uniform locations
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIButton::ProcessMessage(u32 message, u32 parameter)
{
    //TODO: switch statement based on message
    
    /*switch(message)
    {
        case 0:
        {
            break;
        }
        default:
        {
            break;
        }
    }*/
};

//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIButton::LayoutView()
{
	CoreUIView::LayoutView();
	
	//TODO: Do more stuff
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CoreUIButton::UpdateButton(bool buttonIsDown, intVec2* pBeginTouchPos, intVec2* pCurrTouchPos)
{
	if(visible == false)
	{
		return false;
	}
	
	const f32 beginX = pBeginTouchPos->x;
	const f32 beginY = pBeginTouchPos->y;
	
	CoreObjectAttribute_S32* widthAttrib = (CoreObjectAttribute_S32*)attributes.GetAttributeByByteIndex(attrib_width);
	
	CoreObjectAttribute_S32* heightAttrib = (CoreObjectAttribute_S32*)attributes.GetAttributeByByteIndex(attrib_height);
	
	const s32 halfWidth = widthAttrib->value/2;
	const s32 halfHeight = heightAttrib->value/2;
	
	vec3 pos3D;
	pos3D.x = position.x;
	pos3D.y = position.y;
	pos3D.z = 0.0f;
	
    vec3 topLeft = pos3D;
	vec3 topRight = pos3D;
	vec3 bottomLeft = pos3D;
	vec3 bottomRight = pos3D;
	
	topLeft.x -= halfWidth;
	topLeft.y -= halfHeight;
	
	topRight.x += halfWidth;
	topRight.y -= halfHeight;
	
	bottomLeft.x -= halfWidth;
	bottomLeft.y += halfHeight;
	
	bottomRight.x += halfWidth;
	bottomRight.y += halfHeight;
	
	const bool touchBeginIsInside =  beginX > topLeft.x && beginX < topRight.x
	&& beginY > topLeft.y && beginY < bottomRight.y;
	
	const f32 currX = pCurrTouchPos->x;
	const f32 currY = pCurrTouchPos->y;
	
	const bool touchCurrIsInside =  currX > topLeft.x && currX < topRight.x
	&& currY > topLeft.y && currY < bottomRight.y;
	
	CoreUIImageView* pViewIdle = (CoreUIImageView*)COREOBJECTMANAGER->GetObjectByHandle(m_hView_Idle);
	CoreUIImageView* pViewHover = (CoreUIImageView*)COREOBJECTMANAGER->GetObjectByHandle(m_hView_Hover);
	CoreUIImageView* pViewPressed = (CoreUIImageView*)COREOBJECTMANAGER->GetObjectByHandle(m_hView_Pressed);
	
#if defined(PLATFORM_OSX) || defined(PLATFORM_WIN) || defined(PLATFORM_LINUX)
	if(buttonIsDown == false && touchCurrIsInside)
	{
		if(pViewIdle != NULL)
		{
			pViewIdle->visible = false;
		}
		
		if(pViewHover != NULL)
		{
			pViewHover->visible = true;
		}
		
		if(pViewPressed != NULL)
		{
			pViewPressed->visible = false;
		}
		
		m_buttonState = CoreUI_ButtonState_Hover;
	}
	else
#endif
	if(touchBeginIsInside && touchCurrIsInside && buttonIsDown == true)
	{
		//Currently inside
		
			
		if(pViewIdle != NULL)
		{
			pViewIdle->visible = false;
		}
		
		if(pViewHover != NULL)
		{
			pViewHover->visible = false;
		}
		
		if(pViewPressed != NULL)
		{
			pViewPressed->visible = true;
		}
		
		m_buttonState = CoreUI_ButtonState_Pressed;
	}
	else if(m_buttonState == CoreUI_ButtonState_Pressed
			|| m_buttonState == CoreUI_ButtonState_Hover)
	{
		if(pViewIdle != NULL)
		{
			pViewIdle->visible = true;
		}
		
		if(pViewHover != NULL)
		{
			pViewHover->visible = false;
		}
		
		if(pViewPressed != NULL)
		{
			pViewPressed->visible = false;
		}
		
		m_buttonState = CoreUI_ButtonState_Idle;
	}
	
	//TODO: make a changestate function that does this
	//just when the state changes
	LayoutView();
	
	return m_buttonState == CoreUI_ButtonState_Pressed;
}

