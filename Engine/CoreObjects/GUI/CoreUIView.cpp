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
			//We found a match!
			if(pCurrView->nameSig == nameSig)
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
	
	sortValue = 0;
	
	nameSig = 0;
	opacity = 1.0f;
	visible = true;
	offset.x = 0;
	offset.y = 0;
	width = GLRENDERER->screenWidth_points;
	height = GLRENDERER->screenHeight_points;
	//angle = 0.0f;
	origin = CoreUI_Origin_TopLeft;
	
	//LOAD ATTRIBUTES
	
	pugi::xml_attribute offsetX_Attrib = pProperties->attribute("offsetX");
	pugi::xml_attribute offsetY_Attrib = pProperties->attribute("offsetY");
	pugi::xml_attribute width_Attrib = pProperties->attribute("width");
	pugi::xml_attribute height_Attrib = pProperties->attribute("height");
	//pugi::xml_attribute angle_Attrib = pProperties->attribute("angle");
	pugi::xml_attribute opacity_Attrib = pProperties->attribute("opacity");
	pugi::xml_attribute visible_Attrib = pProperties->attribute("visible");
	pugi::xml_attribute name_Attrib = pProperties->attribute("name");
	pugi::xml_attribute sortValue_Attrib = pProperties->attribute("sortValue");
	
	if(offsetX_Attrib.empty() == false)
	{
		offset.x = atof(offsetX_Attrib.value());
	}
	
	if(offsetY_Attrib.empty() == false)
	{
		offset.y = atof(offsetY_Attrib.value());
	}
	
	if(width_Attrib.empty() == false)
	{
		width = atof(width_Attrib.value());
	}
	
	if(height_Attrib.empty() == false)
	{
		height = atof(height_Attrib.value());
	}
	
	/*if(angle_Attrib.empty() == false)
	{
		angle = atof(angle_Attrib.value());
	}*/
	
	if(opacity_Attrib.empty() == false)
	{
		opacity = atof(opacity_Attrib.value());
	}
	
	if(visible_Attrib.empty() == false)
	{
		if(strcmp(visible_Attrib.value(),"false") == 0)
		{
			visible = false;
		}
	}
	
	if(name_Attrib.empty() == false)
	{
		nameSig = Hash(name_Attrib.value());
		
#if defined(_DEBUG_PC)
		nameString = name_Attrib.value();
#endif
	}
	
	if(sortValue_Attrib.empty() == false)
	{
		sortValue = atoi(sortValue_Attrib.value());
	}
	
	pugi::xml_attribute origin_Attrib = pProperties->attribute("origin");
	if(origin_Attrib.empty() == false)
	{
		const char* originStr = origin_Attrib.value();
		
		if(strcmp(originStr, "center") == 0)
		{
			origin = CoreUI_Origin_Center;
		}
		else if(strcmp(originStr, "left") == 0)
		{
			origin = CoreUI_Origin_Left;
		}
		else if(strcmp(originStr, "right") == 0)
		{
			origin = CoreUI_Origin_Right;
		}
		else if(strcmp(originStr, "top") == 0)
		{
			origin = CoreUI_Origin_Top;
		}
		else if(strcmp(originStr, "bottom") == 0)
		{
			origin = CoreUI_Origin_Bottom;
		}
		else if(strcmp(originStr, "topLeft") == 0)
		{
			origin = CoreUI_Origin_TopLeft;
		}
		else if(strcmp(originStr, "bottomLeft") == 0)
		{
			origin = CoreUI_Origin_BottomLeft;
		}
		else if(strcmp(originStr, "topRight") == 0)
		{
			origin = CoreUI_Origin_TopRight;
		}
		else if(strcmp(originStr, "bottomRight") == 0)
		{
			origin = CoreUI_Origin_BottomRight;
		}
	}

	
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
	//NOTE: the parent position is the center of the object
	
	const f32 parentWidth = pParentView?pParentView->width:GLRENDERER->screenWidth_points;
	const f32 parentHeight = pParentView?pParentView->height:GLRENDERER->screenHeight_pixels;
	const f32 parentPosX = pParentView?pParentView->position.x:GLRENDERER->screenWidth_points/2;
	const f32 parentPosY = pParentView?pParentView->position.y:GLRENDERER->screenHeight_points/2;
	
	const f32 parentHalfWidth = parentWidth*0.5f;
	const f32 parentHalfHeight = parentHeight*0.5f;
	
	parentOpacity = pParentView?(pParentView->parentOpacity*pParentView->opacity):1.0f;
	parentVisible = pParentView?(pParentView->parentVisible && pParentView->visible):true;
	
	switch(origin)
	{
		case CoreUI_Origin_Center:
		{
			position.x = (f32)parentPosX+offset.x;
			position.y = (f32)parentPosY+offset.y;
			
			break;
		}
		case CoreUI_Origin_Left:
		{
			position.x = (f32)parentPosX+offset.x-parentHalfWidth;
			position.y = (f32)parentPosY+offset.y;
			
			break;
		}
		case CoreUI_Origin_Right:
		{
			position.x = (f32)parentPosX+offset.x+parentHalfWidth;
			position.y = (f32)parentPosY+offset.y;
			
			break;
		}
		case CoreUI_Origin_Top:
		{
			position.x = (f32)parentPosX+offset.x;
			position.y = (f32)parentPosY+offset.y-parentHalfHeight;
			
			break;
		}
		case CoreUI_Origin_Bottom:
		{
			position.x = (f32)parentPosX+offset.x;
			position.y = (f32)parentPosY+offset.y+parentHalfHeight;
			
			break;
		}
		case CoreUI_Origin_TopLeft:
		{
			position.x = (f32)parentPosX+offset.x-parentHalfWidth;
			position.y = (f32)parentPosY+offset.y-parentHalfHeight;
			
			break;
		}
		case CoreUI_Origin_BottomLeft:
		{
			position.x = (f32)parentPosX+offset.x-parentHalfWidth;
			position.y = (f32)parentPosY+offset.y+parentHalfHeight;
			
			break;
		}
		case CoreUI_Origin_TopRight:
		{
			position.x = (f32)parentPosX+offset.x+parentHalfWidth;
			position.y = (f32)parentPosY+offset.y-parentHalfHeight;
			
			break;
		}
		case CoreUI_Origin_BottomRight:
		{
			position.x = (f32)parentPosX+offset.x+parentHalfWidth;
			position.y = (f32)parentPosY+offset.y+parentHalfHeight;
			
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
void CoreUIView::ProcessMessage(u32 message)
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

