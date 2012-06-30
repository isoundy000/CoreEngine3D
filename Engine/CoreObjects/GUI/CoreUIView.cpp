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
#include "pugixml/src/pugixml.hpp"
#include "../CoreObjectFactories.h"

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
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CoreUIView::SpawnInit(void* pSpawnStruct)
{
	m_numChildren = 0;
	
	pugi::xml_node* pProperties = (pugi::xml_node*)pSpawnStruct;
	
	//DEFAULTS
	opacity = 1.0f;
	parentOpacity = 1.0f;
	
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
	
	//Create images
	for (pugi::xml_node image = pProperties->child("image"); image; image = image.next_sibling("image"))
	{
		CoreUIImageView* pImageView = g_Factory_CoreUIImageView.CreateObject(0);
		if(pImageView != NULL)
		{
			m_children[m_numChildren] = pImageView->GetHandle();
			++m_numChildren;
			
			assert(m_numChildren < CoreUIView_MAX_CHILDREN);
			
			pImageView->SpawnInit(&image);
		}
	}
	
	//Create buttons
	for (pugi::xml_node button = pProperties->child("button"); button; button = button.next_sibling("button"))
	{
		CoreUIButton* pButton = g_Factory_CoreUIButton.CreateObject(0);
		if(pButton != NULL)
		{
			m_children[m_numChildren] = pButton->GetHandle();
			++m_numChildren;
			
			assert(m_numChildren < CoreUIView_MAX_CHILDREN);
			
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
	for(u32 i=0; i<m_numChildren; ++i)
	{
		CoreUIView* pSubView = (CoreUIView*)COREOBJECTMANAGER->GetObjectByHandle(m_children[i]);
		pSubView->LayoutView(this);
	}
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

