//
//  MuzzleFlareParticle_2D.cpp
//  CoreEngine3D
//
//  Created by Jody McAdams on 2/26/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#include "MuzzleFlareParticle_2D.h"

DEFINEFACTORY(MuzzleFlareParticle_2D);

#include "Graphics/OpenGLRenderer.h"
#include "Math/matrix.h"
#include "Math/MathUtil.h"
#include "Game/Game.h"

void MuzzleFlareParticle_2D::InitParticle(MuzzleFlareParticle_2D::ParticleSettings *pSettings, CoreObjectHandle hParentRenderable, mat4f localMat, u32 texIndex)
{
	m_pSettings = pSettings;
	
	mat4f_Copy(m_localMat, localMat);
	
	m_hParentRenderable = hParentRenderable;
	
	TextureAsset* pArtDesc = pSettings->pItemArt;

	RenderableGeometry3D* pGeom = NULL;
	m_hRenderable = GLRENDERER->CreateRenderableGeometry3D(RenderableObjectType_Normal,&pGeom);
	
	if(pGeom == NULL)
	{
		COREDEBUG_PrintDebugMessage("ERROR: Could not create renderable for particle!");
		return;
	}
	
	GLRENDERER->InitRenderableGeometry3D(pGeom, &g_Square1x1_modelData, pSettings->renderMaterial, &pArtDesc->textureHandle, NULL, pSettings->renderLayer, pSettings->blendMode, pSettings->renderFlags|RenderFlag_Visible);
	pGeom->material.uniqueUniformValues[0] = (u8*)&m_diffuseColor;
	
	m_radius = rand_FloatRange(pSettings->radiusMin, pSettings->radiusMax);
	
	m_lifeTimer = 0.0f;
	m_timeToLive = rand_FloatRange(pSettings->lifetimeMin,pSettings->lifetimeMax);
	
	SetVec4(&m_diffuseColor,1.0f,1.0f,1.0f,1.0f);
	CopyVec4(&m_diffuseColorStart,&m_pSettings->diffuseColor);
}


void MuzzleFlareParticle_2D::UpdateAttachment()
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
	AddScaledVec3_Self(mat4f_GetPos(localMat), mat4f_GetUp(localMat), 0.5f*(1.0f-m_heightScale)*m_computedRadius);
	mat4f_Scale_Self(localMat, m_computedRadius);
	
	ScaleVec3_Self(mat4f_GetLeft(localMat), m_widthScale);
	ScaleVec3_Self(mat4f_GetUp(localMat), m_heightScale);
	
	mat4f_Multiply(pGeomSelf->worldMat, normalizedMat, localMat);
}


void MuzzleFlareParticle_2D::Update(f32 timeElapsed)
{
    RenderableGeometry3D* pGeom = GetGeomPointer(m_hRenderable);
    
    if(pGeom == NULL)
	{
		return;
	}
    
	m_lifeTimer += timeElapsed;
	m_lifeTimer = MinF(m_lifeTimer, m_timeToLive);
	
	const f32 lifeT = m_lifeTimer/m_timeToLive;
 
    if(lifeT == 1.0f)
	{
		this->DeleteObject();
	}
    
	f32 scale;
	f32 alpha;
	
	if(lifeT <= m_pSettings->lerpT_Idle)
	{
		const f32 lerpT = lifeT/m_pSettings->lerpT_Idle;
		scale = Lerp(m_pSettings->radiusScaleStart,m_pSettings->radiusScaleIdle,lerpT);
		alpha = lerpT;
		
		m_heightScale = Lerp(m_pSettings->heightScaleStart,m_pSettings->heightScaleIdle,lerpT);
		m_widthScale = Lerp(m_pSettings->widthScaleStart,m_pSettings->widthScaleIdle,lerpT);
	}
	else if(lifeT > m_pSettings->lerpT_Idle && lifeT < m_pSettings->lerpT_End)
	{
		scale = m_pSettings->radiusScaleIdle;
		alpha = 1.0f;
		
		m_heightScale = m_pSettings->heightScaleIdle;
		m_widthScale = m_pSettings->widthScaleIdle;
	}
	else
	{
		const f32 lerpT = (lifeT-m_pSettings->lerpT_End)/(1.0f-m_pSettings->lerpT_End);
		scale = Lerp(m_pSettings->radiusScaleIdle,m_pSettings->radiusScaleEnd,lerpT);
		alpha = 1.0f-lerpT;
		
		m_heightScale = Lerp(m_pSettings->heightScaleIdle,m_pSettings->heightScaleEnd,lerpT);
		m_widthScale = Lerp(m_pSettings->widthScaleIdle,m_pSettings->widthScaleEnd,lerpT);
	}
	
	m_diffuseColor.w = alpha*m_diffuseColorStart.w;
	
	m_computedRadius = m_radius*scale;
	
	UpdateAttachment();
}


void MuzzleFlareParticle_2D::Uninit()
{
	RenderableGeometry3D* pGeom = (RenderableGeometry3D*)COREOBJECTMANAGER->GetObjectByHandle(m_hRenderable);
	if(pGeom != NULL)
	{
		pGeom->DeleteObject();
	}
	
	CoreObject::Uninit();
}


void MuzzleFlareParticle_2D::UpdatePointers()	//Call when the memory location changes
{	
	CoreObject::UpdatePointers();
    
    RenderableGeometry3D* pGeom = (RenderableGeometry3D*)COREOBJECTMANAGER->GetObjectByHandle(m_hRenderable);
	
	if(pGeom != NULL)
	{
		pGeom->material.uniqueUniformValues[0] = (u8*)&m_diffuseColor;
	}
}
