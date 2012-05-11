//
//  MuzzleFlareParticle.cpp
//  CoreEngine3D(OSX)
//
//  Created by Jody McAdams on 2/26/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#include "MuzzleFlareParticle.h"
#include "../OpenGLRenderer.h"
#include "../matrix.h"
#include "../MathUtil.h"
#include "../Game.h"

void MuzzleFlareParticle::InitParticle(MuzzleFlareParticle::ParticleSettings *pSettings, CoreObjectHandle hParentRenderable, mat4f localMat, u32 texIndex)
{
	m_pSettings = pSettings;
	
	mat4f_Copy(m_localMat, localMat);
	
	m_hParentRenderable = hParentRenderable;
	
	ItemArtDescription* pArtDesc = pSettings->pItemArt;

	RenderableGeometry3D* pGeom = NULL;
	m_hRenderable = GLRENDERER->CreateRenderableGeometry3D(RenderableObjectType_Normal,&pGeom);
	
	if(pGeom == NULL)
	{
		COREDEBUG_PrintDebugMessage("ERROR: Could not create renderable for particle!");
		return;
	}
	
	GLRENDERER->InitRenderableGeometry3D(pGeom, pArtDesc->pModelData, pSettings->renderMaterial, &pArtDesc->textureHandle, NULL, pSettings->renderLayer, pSettings->blendMode, pSettings->renderFlags|RenderFlag_Visible);
	pGeom->material.uniqueUniformValues[0] = (u8*)&m_texcoordOffset;
	pGeom->material.uniqueUniformValues[1] = (u8*)&m_diffuseColor;
	
	m_radius = rand_FloatRange(pSettings->radiusMin, pSettings->radiusMax);
	
	m_lifeTimer = 0.0f;
	m_timeToLive = rand_FloatRange(pSettings->lifetimeMin,pSettings->lifetimeMax);
	
	SetVec4(&m_diffuseColor,1.0f,1.0f,1.0f,1.0f);
	CopyVec4(&m_diffuseColorStart,&m_pSettings->diffuseColor);
	
	switch (texIndex)
	{
		case 0:
		{
			SetVec2(&m_texcoordOffset, 0.0f, 0.0f);
			break;
		}
		case 1:
		{
			SetVec2(&m_texcoordOffset, 0.5f, 0.0f);
			break;
		}
		case 2:
		{
			SetVec2(&m_texcoordOffset, 0.0f, 0.5f);
			break;
		}
		case 3:
		{
			SetVec2(&m_texcoordOffset, 0.5f, 0.5f);
			break;
		}
		default:
		{
			break;
		}
	}
	
	UpdateAttachment();
}


void MuzzleFlareParticle::UpdateAttachment()
{
	RenderableGeometry3D* pGeomSelf = GetGeomPointer(m_hRenderable);
	if(pGeomSelf == NULL)
	{
		return;
	}
	
	RenderableGeometry3D* pGeomParent = GetGeomPointer(m_hParentRenderable);
	if(pGeomParent == NULL)
	{
		return;
	}
	
	mat4f normalizedMat;
	mat4f_Copy(normalizedMat, pGeomParent->worldMat);
	mat4f_Normalize_Self(normalizedMat);
	
	mat4f localMat;
	mat4f_Copy(localMat, m_localMat);
	mat4f_Scale_Self(localMat, m_computedRadius);
	
	mat4f_Multiply(pGeomSelf->worldMat, normalizedMat, localMat);
}


void MuzzleFlareParticle::Update(f32 timeElapsed)
{
    RenderableGeometry3D* pGeom = GetGeomPointer(m_hRenderable);
    
    if(pGeom == NULL)
	{
		return;
	}
    
	m_lifeTimer += timeElapsed;
	
	const f32 lifeT = MinF(1.0f,m_lifeTimer/m_timeToLive);
 
    if(lifeT == 1.0f)
	{
		this->DeleteObject();
	}
    
	f32 scale;
	f32 alpha;
	
	if(lifeT <= 0.25f)
	{
		const f32 lerpT = lifeT/0.25f;
		scale = Lerp(m_pSettings->radiusScaleStart,m_pSettings->radiusScaleIdle,lerpT);
		alpha = lerpT;
	}
	else if(lifeT > 0.25f && lifeT < 0.75f)
	{
		scale = m_pSettings->radiusScaleIdle;
		alpha = 1.0f;
	}
	else
	{
		const f32 lerpT = (lifeT-0.75f)/0.25f;
		scale = Lerp(m_pSettings->radiusScaleIdle,m_pSettings->radiusScaleEnd,lerpT);
		alpha = 1.0f-lerpT;
	}
	
	m_diffuseColor.w = alpha;
	
	m_computedRadius = m_radius*scale;
	
	UpdateAttachment();
}


void MuzzleFlareParticle::Uninit()
{
	RenderableGeometry3D* pGeom = (RenderableGeometry3D*)COREOBJECTMANAGER->GetObjectByHandle(m_hRenderable);
	if(pGeom != NULL)
	{
		pGeom->DeleteObject();
	}
	
	CoreObject::Uninit();
}


void MuzzleFlareParticle::UpdateHandle()	//Call when the memory location changes
{	
	CoreObject::UpdateHandle();
    
    RenderableGeometry3D* pGeom = (RenderableGeometry3D*)COREOBJECTMANAGER->GetObjectByHandle(m_hRenderable);
    pGeom->material.uniqueUniformValues[0] = (u8*)&m_texcoordOffset;
	pGeom->material.uniqueUniformValues[1] = (u8*)&m_diffuseColor;
}
