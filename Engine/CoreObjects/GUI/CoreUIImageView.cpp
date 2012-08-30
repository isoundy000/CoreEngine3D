//
//  CoreUIImageView.cpp
//  CoreEngine3D
//
//  Created by Jody McAdams on 4/21/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#include "CoreUIImageView.h"

DEFINEFACTORY(CoreUIImageView);

//Engine includes
#include "Math/MathUtil.h"
#include "Math/Matrix.h"
#include "Game/Game.h"
#include "Util/Hash.h"

//Game includes
#include "Models/EngineModels.h"


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
    CoreUIView::Init(type);
	
	//Add special attributes that are just for CoreUIImageView
	
	attrib_colorR = attributes.Add(CoreObjectAttribute_U32("colorR",255,0,255));
	attrib_colorG = attributes.Add(CoreObjectAttribute_U32("colorG",255,0,255));
	attrib_colorB = attributes.Add(CoreObjectAttribute_U32("colorB",255,0,255));
	
	attrib_imageType = attributes.Add(CoreObjectAttribute_Char32("imageType","default"));
    
    //TODO: other init
	viewType = CoreUI_ViewType_ImageView;
	
	m_hRenderable = CoreObjectHandle();
    
	CopyVec4(&m_diffuseColor,&color4f_white);
	
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CoreUIImageView::SpawnInit(void* pSpawnStruct, CoreObjectHandle hParent)
{
    pugi::xml_node* pProperties = (pugi::xml_node*)pSpawnStruct;
    
	//First do the standard spawn init to bring
	//in all the attributes
	CoreUIView::SpawnInit(pProperties, hParent);
	
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
		
		CoreObjectAttribute_S32* pSortValueAttrib = (CoreObjectAttribute_S32*)attributes.GetAttributeByByteIndex(attrib_sortValue);
		
		pGeom->sortValue = pSortValueAttrib?pSortValueAttrib->value:0;
	}
	
	UpdateDiffuseColor();
	
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
	CoreUIView::Update(timeElapsed);
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIImageView::UpdatePointers()
{
	CoreObject::UpdatePointers();
	
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
void CoreUIImageView::ProcessMessage(u32 message, u32 parameter)
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
void CoreUIImageView::LayoutView()
{
	CoreUIView::LayoutView();
	
	RefreshSettings();
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIImageView::UpdateDiffuseColor()
{
	CoreObjectAttribute_U32* pRedAttrib = (CoreObjectAttribute_U32*)attributes.GetAttributeByByteIndex(attrib_colorR);
	m_diffuseColor.x = pRedAttrib->value/255.0f;
	
	CoreObjectAttribute_U32* pGreenAttrib = (CoreObjectAttribute_U32*)attributes.GetAttributeByByteIndex(attrib_colorG);
	m_diffuseColor.y = pGreenAttrib->value/255.0f;
	
	CoreObjectAttribute_U32* pBlueAttrib = (CoreObjectAttribute_U32*)attributes.GetAttributeByByteIndex(attrib_colorB);
	m_diffuseColor.z = pBlueAttrib->value/255.0f;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIImageView::RefreshSettings()
{
	CoreObjectAttribute_F32* pOpacityAttrib = (CoreObjectAttribute_F32*)attributes.GetAttributeByByteIndex(attrib_opacity);
	
	const f32 finalOpacity = parentOpacity*pOpacityAttrib->value*fadeAlpha;
	m_diffuseColor.w = finalOpacity;
	
	//Update renderable to reflect new layout
	RenderableGeometry3D* pGeom = GetGeomPointer(m_hRenderable);
	if(pGeom != NULL)
	{
		CoreObjectAttribute_S32* pWidthAttrib = (CoreObjectAttribute_S32*)attributes.GetAttributeByByteIndex(attrib_width);		
		CoreObjectAttribute_S32* pHeightAttrib = (CoreObjectAttribute_S32*)attributes.GetAttributeByByteIndex(attrib_height);
		
		mat4f_LoadScaleFromFloats(pGeom->worldMat, pWidthAttrib->value, pHeightAttrib->value, 1.0f);
		vec3* pPos = GetGeomPos(pGeom);
		
		pPos->x = position.x;
		pPos->y = position.y;
		pPos->z = 0.0f;
		
		if(finalOpacity > 0.0f && (parentVisible && visible))
		{
			pGeom->material.flags |= RenderFlag_Visible;
		}
		else
		{
			pGeom->material.flags &= ~RenderFlag_Visible;
		}
		
		CoreObjectAttribute_S32* pSortValueAttrib = (CoreObjectAttribute_S32*)attributes.GetAttributeByByteIndex(attrib_sortValue);
		
		pGeom->postRenderLayerSortValue = pSortValueAttrib?pSortValueAttrib->value:0;
	}
	
	UpdateDiffuseColor();
	
	GLRENDERER->ForceRenderablesNeedSorting(RenderableObjectType_UI);
}

