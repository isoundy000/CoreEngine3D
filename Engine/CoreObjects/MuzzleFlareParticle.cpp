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

void MuzzleFlareParticle::InitParticle(ParticleSettings *pSettings, const vec3* pPosition, const vec3* pDirection, u32 texIndex)
{
	m_pSettings = pSettings;
	
	ItemArtDescription* pArtDesc = pSettings->pItemArt;

	RenderableGeometry3D* pGeom = NULL;
	m_hRenderable = GLRENDERER->CreateRenderableGeometry3D(RenderableObjectType_Normal,&pGeom);
	
	if(pGeom == NULL)
	{
		COREDEBUG_PrintDebugMessage("ERROR: Could not create renderable for particle!");
		return;
	}
	
	CopyVec3(&m_dir,pDirection);
	
	GLRENDERER->InitRenderableGeometry3D(pGeom, pArtDesc->pModelData, pSettings->renderMaterial, &pArtDesc->textureHandle, NULL, pSettings->renderLayer, pSettings->blendMode, pSettings->renderFlags|RenderFlag_Visible);
	pGeom->material.uniqueUniformValues[0] = (u8*)&m_texcoordOffset;
	pGeom->material.uniqueUniformValues[1] = (u8*)&m_diffuseColor;
	
	m_radius = rand_FloatRange(pSettings->radiusMin, pSettings->radiusMax);
	
	mat4f_LoadScale(pGeom->worldMat, m_radius);
	
	vec3* pPos = mat4f_GetPos(pGeom->worldMat);
	CopyVec3(pPos, pPosition);
	CopyVec3(&m_position,pPosition);
	
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
}


void MuzzleFlareParticle::Update(f32 timeElapsed)
{
    RenderableGeometry3D* pGeom = (RenderableGeometry3D*)COREOBJECTMANAGER->GetObjectByHandle(m_hRenderable);
    
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
    
	vec3* pPos = mat4f_GetPos(pGeom->worldMat);
	
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
	
	ScaleVec4(&m_diffuseColor,&m_diffuseColorStart,alpha);
	
	const f32 radius = m_radius*scale;
	mat4f_LoadScaledRotationFromUp(pGeom->worldMat, &m_dir, radius);

	CopyVec3(pPos, &m_position);
	AddScaledVec3_Self(pPos, &m_dir, radius*0.5f);
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
