//
//  CoreUIView.cpp
//  CoreEngine3D
//
//  Created by Jody McAdams on 4/21/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#include "CoreUIView.h"

//Engine includes
#include "../MathUtil.h"
#include "../matrix.h"
#include "../Game.h"
#include "../pugixml/src/pugixml.hpp"
#include "../CoreObjectFactories.h"
#include "../Hash.h"

//Art
/*static TextureAsset g_Art_SpaceShip =
{
	"ArtResources/Characters/SpaceShip/SpaceShip.png",,
	ImageType_PNG,
	0,
	GL_LINEAR,
	GL_CLAMP_TO_EDGE,
	GL_CLAMP_TO_EDGE,
	true,
};*/


//Sound
/*static ItemSoundDescription g_Sound_SpaceShipBeam =
{
	"ArtResources/SFX/SpaceShipBeam.wav",
	0,//soundBufferID
};*/


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIView::InitClass()
{
    //TODO: initialize shared class globals here
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CoreUIView::LoadResourcesForType(u32 type)
{
    //TODO: call GAME->AddItemArt(&g_Art_SpaceShip)
	//TODO: call GAME->AddItemSound(&g_Sound_SpaceShipBeam)
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CoreUIView::Init(u32 type)
{
    //Base class init
    CoreGameObject::Init(type);
    
    //TODO: other init
	viewType = CoreUI_ViewType_View;
	
	//Add attributes
	attributes.Init(m_attribData,1024);
	
	//cache attributes that are used often
	m_attrib_name = attributes.Add(CoreObjectAttribute_Char32("name","Unnamed"));
	
	m_attrib_origin = attributes.Add(CoreObjectAttribute_CoreUI_Origin("origin"));
	m_attrib_offsetX = attributes.Add(CoreObjectAttribute_S32("offsetX",0));
	m_attrib_offsetY = attributes.Add(CoreObjectAttribute_S32("offsetY",0));
	
	//TODO: get a better way to say this view is full screen
	m_attrib_width = attributes.Add(CoreObjectAttribute_S32("width",GLRENDERER->screenWidth_points));
	m_attrib_height = attributes.Add(CoreObjectAttribute_S32("height",GLRENDERER->screenHeight_points));
	m_attrib_opacity = attributes.Add(CoreObjectAttribute_F32("opacity",1.0f));
	m_attrib_sortValue = attributes.Add(CoreObjectAttribute_S32("sortValue",0));
	
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
CoreUIView* CoreUIView::GetChildViewByName(u32 nameSig)
{
	//Loop through all the child views
	for(u32 i=0; i<numChildren; ++i)
	{
		CoreUIView* pCurrView = (CoreUIView*)COREOBJECTMANAGER->GetObjectByHandle(children[i]);
		if(pCurrView != NULL)
		{
			CoreObjectAttribute_Char32* pNameAttrib = (CoreObjectAttribute_Char32*)pCurrView->attributes[m_attrib_name];
			
			//We found a match!
			if(pNameAttrib->hashedValue == nameSig)
			{
				return pCurrView;
			}
			//We didn't find a match...
			else
			{
				//Recursively find the view
				CoreUIView* pFoundView = pCurrView->GetChildViewByName(nameSig);
				if(pFoundView != NULL)
				{
					return pFoundView;
				}
			}
		}
	}
	
	return NULL;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CoreUIView::SpawnInit(void* pSpawnStruct)
{
	numChildren = 0;
	
	pugi::xml_node* pProperties = (pugi::xml_node*)pSpawnStruct;
	
	//DEFAULTS
	
	parentOpacity = 1.0f;
	parentVisible = true;

	visible = true;

	//LOAD ATTRIBUTES
	pugi::xml_node::attribute_iterator attribIter = pProperties->attributes_begin();
	while(attribIter != pProperties->attributes_end())
	{
		attributes.SetValueForAttribByCString(attribIter->name(), attribIter->value());
		++attribIter;
	}
	
	CoreObjectAttribute_F32* pOpacityAttrib = (CoreObjectAttribute_F32*)attributes[m_attrib_opacity];
	opacity = pOpacityAttrib->value;
	
	//CREATE SUBVIEWS
	
	//Create views
	for (pugi::xml_node view = pProperties->child("view"); view; view = view.next_sibling("view"))
	{
		CoreUIView* pChildView = g_Factory_CoreUIView.CreateObject(0);
		if(pChildView != NULL)
		{
			children[numChildren] = pChildView->GetHandle();
			++numChildren;
			
			assert(numChildren < CoreUIView_MAX_CHILDREN);
			
			pChildView->SpawnInit(&view);
		}
	}
	
	//Create images
	for (pugi::xml_node image = pProperties->child("image"); image; image = image.next_sibling("image"))
	{
		CoreUIImageView* pImageView = g_Factory_CoreUIImageView.CreateObject(0);
		if(pImageView != NULL)
		{
			children[numChildren] = pImageView->GetHandle();
			++numChildren;
			
			assert(numChildren < CoreUIView_MAX_CHILDREN);
			
			pImageView->SpawnInit(&image);
		}
	}
	
	//Create buttons
	for (pugi::xml_node button = pProperties->child("button"); button; button = button.next_sibling("button"))
	{
		CoreUIButton* pButton = g_Factory_CoreUIButton.CreateObject(0);
		if(pButton != NULL)
		{
			children[numChildren] = pButton->GetHandle();
			++numChildren;
			
			assert(numChildren < CoreUIView_MAX_CHILDREN);
			
			pButton->SpawnInit(&button);
		}
	}
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIView::LayoutView(const CoreUIView* pParentView)
{
	const CoreObjectAttributeList* parentAttribs = pParentView?(&pParentView->attributes):NULL;
	
	//NOTE: the parent position is the center of the object
	CoreObjectAttribute_S32* pParentWidthAttrib = parentAttribs?(CoreObjectAttribute_S32*)(*parentAttribs)[m_attrib_width]:NULL;
	
	CoreObjectAttribute_S32* pParentHeightAttrib = parentAttribs?(CoreObjectAttribute_S32*)(*parentAttribs)[m_attrib_height]:NULL;
	
	const f32 parentWidth = pParentView?pParentWidthAttrib->value:GLRENDERER->screenWidth_points;
	const f32 parentHeight = pParentView?pParentHeightAttrib->value:GLRENDERER->screenHeight_pixels;
	const f32 parentPosX = pParentView?pParentView->position.x:GLRENDERER->screenWidth_points/2;
	const f32 parentPosY = pParentView?pParentView->position.y:GLRENDERER->screenHeight_points/2;
	
	const f32 parentHalfWidth = parentWidth*0.5f;
	const f32 parentHalfHeight = parentHeight*0.5f;
	
	parentOpacity = pParentView?(pParentView->parentOpacity*pParentView->opacity):1.0f;
	parentVisible = pParentView?(pParentView->parentVisible && pParentView->visible):true;
	
	CoreObjectAttribute_CoreUI_Origin* pOriginAttrib = (CoreObjectAttribute_CoreUI_Origin*)attributes[m_attrib_origin];
	
	CoreObjectAttribute_S32* pOffsetXAttrib = (CoreObjectAttribute_S32*)attributes[m_attrib_offsetX];
	CoreObjectAttribute_S32* pOffsetYAttrib = (CoreObjectAttribute_S32*)attributes[m_attrib_offsetY];
	
	const s32 offsetX = pOffsetXAttrib->value;
	const s32 offsetY = pOffsetYAttrib->value;
	
	switch(pOriginAttrib->value)
	{
		case CoreUI_Origin_Center:
		{
			position.x = (f32)parentPosX+offsetX;
			position.y = (f32)parentPosY+offsetY;
			
			break;
		}
		case CoreUI_Origin_Left:
		{
			position.x = (f32)parentPosX+offsetX-parentHalfWidth;
			position.y = (f32)parentPosY+offsetY;
			
			break;
		}
		case CoreUI_Origin_Right:
		{
			position.x = (f32)parentPosX+offsetX+parentHalfWidth;
			position.y = (f32)parentPosY+offsetY;
			
			break;
		}
		case CoreUI_Origin_Top:
		{
			position.x = (f32)parentPosX+offsetX;
			position.y = (f32)parentPosY+offsetY-parentHalfHeight;
			
			break;
		}
		case CoreUI_Origin_Bottom:
		{
			position.x = (f32)parentPosX+offsetX;
			position.y = (f32)parentPosY+offsetY+parentHalfHeight;
			
			break;
		}
		case CoreUI_Origin_TopLeft:
		{
			position.x = (f32)parentPosX+offsetX-parentHalfWidth;
			position.y = (f32)parentPosY+offsetY-parentHalfHeight;
			
			break;
		}
		case CoreUI_Origin_BottomLeft:
		{
			position.x = (f32)parentPosX+offsetX-parentHalfWidth;
			position.y = (f32)parentPosY+offsetY+parentHalfHeight;
			
			break;
		}
		case CoreUI_Origin_TopRight:
		{
			position.x = (f32)parentPosX+offsetX+parentHalfWidth;
			position.y = (f32)parentPosY+offsetY-parentHalfHeight;
			
			break;
		}
		case CoreUI_Origin_BottomRight:
		{
			position.x = (f32)parentPosX+offsetX+parentHalfWidth;
			position.y = (f32)parentPosY+offsetY+parentHalfHeight;
			
			break;
		}
		default:
		{
			break;
		}
	}
	
	//Layout children (subviews)
	LayoutSubViews();
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIView::LayoutSubViews()
{
	for(u32 i=0; i<numChildren; ++i)
	{
		CoreUIView* pSubView = (CoreUIView*)COREOBJECTMANAGER->GetObjectByHandle(children[i]);
		pSubView->LayoutView(this);
	}
}

//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIView::RefreshView()
{
	CoreUIView::LayoutView(this);
}

//----------------------------------------------------------------
//----------------------------------------------------------------
bool CoreUIView::PostSpawnInit(void* pSpawnStruct)
{
    //TODO: save any links to other objects here
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIView::Uninit()
{
	attributes.Uninit();
	
    //Base class uninit
    CoreGameObject::Uninit();
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIView::Update(f32 timeElapsed)
{
    //TODO: update here
    
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIView::UpdateHandle()
{
    //TODO: update anything that has pointers to local
    //member variables, such as material uniform locations
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIView::ProcessMessage(u32 message, u32 parameter)
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

