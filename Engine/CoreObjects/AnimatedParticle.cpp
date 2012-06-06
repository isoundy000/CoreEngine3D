//
//  AnimatedParticle.cpp
//  CoreEngine3D(OSX)
//
//  Created by Jody McAdams on 2/26/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#include "AnimatedParticle.h"
#include "../OpenGLRenderer.h"
#include "../matrix.h"
#include "../MathUtil.h"
#include "../Game.h"

void AnimatedParticle::InitParticle(ParticleSettings *pSettings, const vec3* pPosition, const vec3* pDirection, f32 startAngle, s32 animID, u32 frameOffset, f32 playSpeed)
{
	m_pSettings = pSettings;
	
	m_animPlayer.Init(pSettings->pAnimSet);
	m_animPlayer.PlayAnimation(animID, frameOffset, playSpeed);
	
	ItemArtDescription* pArtDesc = pSettings->pItemArt;

	RenderableGeometry3D* pGeom = NULL;
	m_hRenderable = GLRENDERER->CreateRenderableGeometry3D(RenderableObjectType_Normal,&pGeom);
	
	if(pGeom == NULL)
	{
		COREDEBUG_PrintDebugMessage("ERROR: Could not create renderable for particle!");
		return;
	}
	
	GLRENDERER->InitRenderableGeometry3D(pGeom, &g_Square1x1_modelData, pSettings->renderMaterial, &pArtDesc->textureHandle, NULL, pSettings->renderLayer, pSettings->blendMode, pSettings->renderFlags|RenderFlag_Visible);
	pGeom->material.uniqueUniformValues[0] = (u8*)&m_texcoordOffset;
	pGeom->material.uniqueUniformValues[1] = (u8*)&m_diffuseColor;
	
	const f32 radius = rand_FloatRange(pSettings->radiusMin, pSettings->radiusMax);
	m_radiusStart = radius*pSettings->radiusScale_start;
	m_radiusEnd = radius*pSettings->radiusScale_end;
	
	mat4f_LoadScale(pGeom->worldMat, m_radiusStart);
	
	vec3* pPos = mat4f_GetPos(pGeom->worldMat);
	CopyVec3(pPos, pPosition);
	CopyVec3(&m_position,pPosition);
	
	const f32 speed = rand_FloatRange(pSettings->moveSpeedMin, pSettings->moveSpeedMax);
	ScaleVec3(&m_velocity,pDirection,speed);
	f32 spinSpeed = rand_FloatRange(pSettings->spinSpeedMin, pSettings->spinSpeedMax);
	if(pSettings->randomlyFlipSpin)
	{
		spinSpeed *= -1.0f;
	}
	m_spinSpeed = spinSpeed*(rand_Bool() ? -1.0f : 1.0f);
	m_currSpinAngle = startAngle;
	m_lifeTimer = rand_FloatRange(pSettings->lifetimeMin,pSettings->lifetimeMax);
	m_totalLifeTime = m_lifeTimer;
	
	m_fadeTime = pSettings->fadeTime*m_totalLifeTime;
	
	CopyVec4(&m_diffuseColor,&pSettings->diffuseColor);
	CopyVec4(&m_diffuseColorStart,&m_diffuseColor);
	
	m_animPlayer.Update(0.0f);
	const s32 currFrame = (s32)m_animPlayer.GetCurrentFrame();
	
	SpriteFrameInfo frameInfo;
	
	const bool gotInfo = pSettings->pSpriteAnimator->GetSpriteInfoForFrame(&frameInfo,currFrame);
	
	if(gotInfo == true)
	{
		CopyVec2(&m_texcoordOffset,&frameInfo.textureOffset);
	}
}


u32 AnimatedParticle::GetCategoryFlags()
{
	return m_pSettings->categoryFlags;
}


void AnimatedParticle::Update(f32 timeElapsed)
{
    RenderableGeometry3D* pGeom = (RenderableGeometry3D*)COREOBJECTMANAGER->GetObjectByHandle(m_hRenderable);
    
    if(pGeom == NULL)
	{
		return;
	}
    
	m_lifeTimer -= timeElapsed;
	
	const f32 breakableAlpha = ClampF(m_lifeTimer/m_fadeTime,0.0f,1.0f);
    
    if(m_pSettings->blendMode == BlendMode_Premultiplied)
	{
		ScaleVec4(&m_diffuseColor,&m_diffuseColorStart,breakableAlpha);
	}
	else
	{
		m_diffuseColor.w = m_diffuseColorStart.w*breakableAlpha;
	}

	vec3* pPos = mat4f_GetPos(pGeom->worldMat);
	
	
	const f32 lerpT = MinF(1.0f,m_lifeTimer/m_totalLifeTime);
	const f32 radius = Lerp(m_radiusEnd, m_radiusStart, lerpT);
	
	m_currSpinAngle += m_spinSpeed*timeElapsed;

	m_velocity.y -= m_pSettings->gravity*timeElapsed;
	AddScaledVec3_Self(&m_position,&m_velocity,timeElapsed);
	
	CopyVec3(pPos,&m_position);
	
	vec3 velNorm;
	TryNormalizeVec3(&velNorm,&m_velocity);
	
	mat4f_LoadScaledZRotation_IgnoreTranslation(pGeom->worldMat, m_currSpinAngle, radius);
	
	
	//Update sprite animation
	m_animPlayer.Update(timeElapsed);
	const s32 currFrame = (s32)m_animPlayer.GetCurrentFrame();
	
	SpriteFrameInfo frameInfo;
	
	const bool gotInfo = m_pSettings->pSpriteAnimator->GetSpriteInfoForFrame(&frameInfo,currFrame);
	
	if(gotInfo == true)
	{
		CopyVec2(&m_texcoordOffset,&frameInfo.textureOffset);
		pGeom->pModel = frameInfo.pModelData;
	}
	
	
	if(m_lifeTimer <= 0.0f)
	{
		this->DeleteObject();
	}
    
    GAME->TiledLevel_DeleteObjectIfOffscreen_X(this,pPos,radius,0.0f);
    GAME->TiledLevel_DeleteObjectIfOffscreen_Y(this,pPos,radius,0.0f);
}


void AnimatedParticle::Uninit()
{
	RenderableGeometry3D* pGeom = (RenderableGeometry3D*)COREOBJECTMANAGER->GetObjectByHandle(m_hRenderable);
	if(pGeom != NULL)
	{
		pGeom->DeleteObject();
	}
	
	CoreObject::Uninit();
}


const vec3* AnimatedParticle::GetPosition() const
{
	return &m_position;
}

void AnimatedParticle::AddVelocity(const vec3* pVelAdd)
{
	AddVec3_Self(&m_velocity, pVelAdd);
}


void AnimatedParticle::UpdateHandle()	//Call when the memory location changes
{	
	CoreObject::UpdateHandle();
    
    RenderableGeometry3D* pGeom = (RenderableGeometry3D*)COREOBJECTMANAGER->GetObjectByHandle(m_hRenderable);
    pGeom->material.uniqueUniformValues[0] = (u8*)&m_texcoordOffset;
	pGeom->material.uniqueUniformValues[1] = (u8*)&m_diffuseColor;
}
