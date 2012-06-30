//
//  CoreUIImageView.cpp
//  CoreEngine3D
//
//  Created by Jody McAdams on 4/21/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#include "CoreUIImageView.h"

//Engine includes
#include "../MathUtil.h"
#include "../matrix.h"
#include "../Game.h"
#include "../Hash.h"

//Game includes
#include "EngineModels.h"


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIImageView::InitClass()
{
    //TODO: initialize shared class globals here
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CoreUIImageView::LoadResourcesForType(u32 type)
{
    //TODO: call GAME->AddItemArt(&g_Art_SpaceShip)
	//TODO: call GAME->AddItemSound(&g_Sound_SpaceShipBeam)
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CoreUIImageView::Init(u32 type)
{
    //Base class init
    CoreGameObject::Init(type);
    
    //TODO: other init
	m_hRenderable = CoreObjectHandle();
    
	CopyVec4(&m_diffuseColor,&color4f_white);
	
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CoreUIImageView::SpawnInit(void* pSpawnStruct)
{
    pugi::xml_node* pProperties = (pugi::xml_node*)pSpawnStruct;
    
	//First do the standard spawn init to bring
	//in all the attributes
	CoreUIView::SpawnInit(pProperties);
	
	//Set up the renderable
	RenderableGeometry3D* pGeom = NULL;
	m_hRenderable = GLRENDERER->CreateRenderableGeometry3D(RenderableObjectType_UI, &pGeom);
	
	if(pGeom != NULL)
	{
		GLRENDERER->InitRenderableGeometry3D(pGeom, &g_Square1x1_modelData, MT_TextureAndDiffuseColor, NULL, NULL, RenderLayer_Normal, BlendMode_Normal, RenderFlagDefaults_2DTexture_AlphaBlended|RenderFlag_Visible);
		
		pGeom->material.uniqueUniformValues[0] = (u8*)&m_diffuseColor;
		
		pugi::xml_attribute texture_Attrib = pProperties->attribute("texture");
		if(texture_Attrib.empty() == false)
		{
			const u32 nameSig = Hash(texture_Attrib.value());
			pGeom->material.customTexture0 = GAME->GetHUDTextureByNameSig(nameSig);
		}
	}
	
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CoreUIImageView::PostSpawnInit(void* pSpawnStruct)
{
    //TODO: save any links to other objects here
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIImageView::Uninit()
{
    //Base class uninit
    CoreGameObject::Uninit();
	
	RenderableGeometry3D* pGeom = GetGeomPointer(m_hRenderable);
	if(pGeom != NULL)
	{
		pGeom->DeleteObject();
	}
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIImageView::Update(f32 timeElapsed)
{
    //TODO: update here
    
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIImageView::UpdateHandle()
{
    //TODO: update anything that has pointers to local
    //member variables, such as material uniform locations
	
	RenderableGeometry3D* pGeom = GetGeomPointer(m_hRenderable);
	if(pGeom != NULL)
	{
		pGeom->material.uniqueUniformValues[0] = (u8*)&m_diffuseColor;
	}
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIImageView::ProcessMessage(u32 message)
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
void CoreUIImageView::LayoutView(const CoreUIView* pParentView)
{
	CoreUIView::LayoutView(pParentView);
	
	m_diffuseColor.w = parentOpacity*opacity;
	
	//Update renderable to reflect new layout
	RenderableGeometry3D* pGeom = GetGeomPointer(m_hRenderable);
	if(pGeom != NULL)
	{
		mat4f_LoadScaleFromFloats(pGeom->worldMat, width, height, 1.0f);
		vec3* pPos = GetGeomPos(pGeom);
		
		pPos->x = position.x;
		pPos->y = position.y;
		pPos->z = 0.0f;
	}
}

