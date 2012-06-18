//
//  BasicParticle_2D.cpp
//  CoreEngine3D(OSX)
//
//  Created by Jody McAdams on 2/26/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#include "BasicParticle_2D.h"
#include "../OpenGLRenderer.h"
#include "../matrix.h"
#include "../MathUtil.h"
#include "../Game.h"

void BasicParticle_2D::InitParticle(ParticleSettings *pSettings, u32 numColumns, const vec3* pPosition, const vec3* pDirection, f32 startAngle, u32 texIndex)
{
	m_pSettings = pSettings;
	
	TextureAsset* pArtDesc = pSettings->pItemArt;
	
	m_hParentRenderable = CoreObjectHandle();

	RenderableGeometry3D* pGeom = NULL;
	m_hRenderable = GLRENDERER->CreateRenderableGeometry3D(RenderableObjectType_Normal,&pGeom);
	
	if(pGeom == NULL)
	{
		COREDEBUG_PrintDebugMessage("ERROR: Could not create renderable for particle!");
		return;
	}
	
	ModelData* pModelData = NULL;
	
	switch(numColumns)
	{
		case 0:
		case 1:
		{
			pModelData = &g_Square1x1_modelData;
			
			break;
		}
		case 2:
		{
			pModelData = &g_Square_Tiled_2_modelData;
			
			break;
		}
		case 4:
		{
			pModelData = &g_Square_Tiled_4_modelData;
			
			break;
		}
		case 8:
		{
			pModelData = &g_Square_Tiled_8_modelData;
			
			break;
		}
		case 16:
		{
			pModelData = &g_Square_Tiled_16_modelData;
			
			break;
		}
		case 32:
		{
			pModelData = &g_Square_Tiled_32_modelData;
			
			break;
		}
	}
	
	GLRENDERER->InitRenderableGeometry3D(pGeom, pModelData, pSettings->renderMaterial, &pArtDesc->textureHandle, NULL, pSettings->renderLayer, pSettings->blendMode, pSettings->renderFlags|RenderFlag_Visible);
	
	m_numColumns = numColumns;
	
	if(m_numColumns < 2)
	{
		pGeom->material.uniqueUniformValues[0] = (u8*)&m_diffuseColor;
	}
	else
	{
		pGeom->material.uniqueUniformValues[0] = (u8*)&m_texcoordOffset;
		pGeom->material.uniqueUniformValues[1] = (u8*)&m_diffuseColor;
	}
	
	const f32 radius = rand_FloatRange(pSettings->radiusMin, pSettings->radiusMax);
	m_radiusStart = radius*pSettings->radiusScale_start;
	m_radiusEnd = radius*pSettings->radiusScale_end;
		
	mat4f_LoadScale(pGeom->worldMat, m_radiusStart);
	
	vec3* pPos = mat4f_GetPos(pGeom->worldMat);
	CopyVec3(pPos, pPosition);
	CopyVec3(&m_position,pPosition);
	
	const f32 speed = rand_FloatRange(pSettings->moveSpeedMin, pSettings->moveSpeedMax);
	ScaleVec3(&m_velocity,pDirection,speed);
	
	m_spinSpeed = rand_FloatRange(pSettings->spinSpeedMin, pSettings->spinSpeedMax);
	if(pSettings->randomlyFlipSpin)
	{
		m_spinSpeed *= (rand_Bool() ? -1.0f : 1.0f);
	}
	
	m_currSpinAngle = startAngle;
	m_lifeTimer = 0.0f;
	m_totalLifeTime = rand_FloatRange(pSettings->lifetimeMin,pSettings->lifetimeMax);
	
	m_fadeTimeStart = pSettings->fadeTimeStart*m_totalLifeTime;
	m_fadeTimeEnd = m_totalLifeTime-pSettings->fadeTimeEnd*m_totalLifeTime;
	
	CopyVec4(&m_diffuseColor,&pSettings->diffuseColor);
	CopyVec4(&m_diffuseColorStart,&m_diffuseColor);
	
	
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
	
	m_pBody = NULL;
	if(pSettings->pBox2D_ParticleSettings != NULL)
	{
		Box2D_ParticleSettings* pCurrSettings = &pSettings->pBox2D_ParticleSettings[texIndex];
		
		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(m_position.x/GAME->GetPixelsPerMeter(), m_position.y/GAME->GetPixelsPerMeter());
		
		b2FixtureDef fixtureDef;
		fixtureDef.filter.maskBits = pCurrSettings->maskBits;
		fixtureDef.filter.categoryBits = pCurrSettings->categoryBits;
		fixtureDef.filter.groupIndex = pCurrSettings->groupIndex;
		fixtureDef.restitution = pCurrSettings->restitution;
		fixtureDef.density = pCurrSettings->density;
		fixtureDef.friction = pCurrSettings->friction;
		
		
		b2CircleShape circleShape;


		circleShape.m_radius = pCurrSettings->collisionRadiusPixels/GAME->GetPixelsPerMeter();

		fixtureDef.shape = &circleShape;
		
		m_pBody = GAME->Box2D_GetWorld()->CreateBody(&bodyDef);
		m_pBody->CreateFixture(&fixtureDef);
		
		m_pBody->SetAngularVelocity(m_spinSpeed);
		m_pBody->SetLinearVelocity(b2Vec2(pDirection->x,pDirection->y));
	}
	
	Update(0.0f);
}


u32 BasicParticle_2D::GetCategoryFlags()
{
	return m_pSettings->categoryFlags;
}


void BasicParticle_2D::SetPositionRelativeToRenderable(CoreObjectHandle hParentRenderable)
{
	m_hParentRenderable = hParentRenderable;
	
	RenderableGeometry3D* pGeom = GetGeomPointer(m_hParentRenderable);
	if(pGeom != NULL)
	{
		SubVec3_Self(&m_position, GetGeomPos(pGeom));
	}
}


void BasicParticle_2D::Update(f32 timeElapsed)
{
    RenderableGeometry3D* pGeom = (RenderableGeometry3D*)COREOBJECTMANAGER->GetObjectByHandle(m_hRenderable);
    
    if(pGeom == NULL)
	{
		return;
	}
    
	m_lifeTimer += timeElapsed;
	
	//Calculate alpha based on life time
	f32 breakableAlpha;
	if(m_lifeTimer < m_fadeTimeStart)
	{
		breakableAlpha = m_lifeTimer/m_fadeTimeStart;
	}
	else if(m_lifeTimer > m_fadeTimeEnd)
	{
		breakableAlpha = 1.0f-(m_lifeTimer-m_fadeTimeEnd)/(m_totalLifeTime-m_fadeTimeEnd);
	}
	else
	{
		breakableAlpha = 1.0f;
	}
    
	switch(m_pSettings->blendMode)
	{
		case BlendMode_Add:
		case BlendMode_DesaturatedAdd:
		case BlendMode_Premultiplied:
		{
			ScaleVec4(&m_diffuseColor,&m_diffuseColorStart,breakableAlpha);
			
			break;
		}
		default:
		{
			m_diffuseColor.w = m_diffuseColorStart.w*breakableAlpha;
			
			break;
		}
	}

	vec3* pPos = mat4f_GetPos(pGeom->worldMat);
	
	const f32 lerpT = MinF(1.0f,m_lifeTimer/m_totalLifeTime);
	const f32 radius = Lerp(m_radiusStart, m_radiusEnd, lerpT);
	
	if(m_pBody != NULL)
	{
		const b2Vec2& posVec = m_pBody->GetPosition();
		pPos->x = posVec.x*GAME->GetPixelsPerMeter();
		pPos->y = posVec.y*GAME->GetPixelsPerMeter();
		pPos->z = 0.0f;
		
		mat4f_LoadScaledZRotation_IgnoreTranslation(pGeom->worldMat, m_pBody->GetAngle(), radius);
	}
	else
	{
		m_currSpinAngle += m_spinSpeed*timeElapsed;

		m_velocity.y -= m_pSettings->gravity*timeElapsed;
		AddScaledVec3_Self(&m_position,&m_velocity,timeElapsed);
		
		CopyVec3(pPos,&m_position);
		
		mat4f_LoadScaledZRotation_IgnoreTranslation(pGeom->worldMat, m_currSpinAngle, radius);
		
		//If this particle is relative to a renderable, adjust it's position to be
		//based off of the position of the parent
		if(m_hParentRenderable)
		{
			RenderableGeometry3D* pParentGeom = GetGeomPointer(m_hParentRenderable);
			if(pParentGeom != NULL)
			{
				AddVec3_Self(pPos, GetGeomPos(pParentGeom));
			}
		}
	}
	
	
	if(m_lifeTimer >= m_totalLifeTime)
	{
		this->DeleteObject();
	}
    
    GAME->TiledLevel_DeleteObjectIfOffscreen_X(this,pPos,radius,0.0f);
    GAME->TiledLevel_DeleteObjectIfOffscreen_Y(this,pPos,radius,0.0f);
}


void BasicParticle_2D::Uninit()
{
	RenderableGeometry3D* pGeom = (RenderableGeometry3D*)COREOBJECTMANAGER->GetObjectByHandle(m_hRenderable);
	if(pGeom != NULL)
	{
		pGeom->DeleteObject();
	}
	
	if(m_pBody != NULL)
	{
		GAME->Box2D_GetWorld()->DestroyBody(m_pBody);
	}
	
	CoreObject::Uninit();
}


const vec3* BasicParticle_2D::GetPosition() const
{
	return &m_position;
}

void BasicParticle_2D::AddVelocity(const vec3* pVelAdd)
{
	AddVec3_Self(&m_velocity, pVelAdd);
}


void BasicParticle_2D::SetVelocity(const vec3* pVel)
{
	CopyVec3(&m_velocity, pVel);
}


void BasicParticle_2D::UpdateHandle()	//Call when the memory location changes
{	
	CoreObject::UpdateHandle();
    
    RenderableGeometry3D* pGeom = (RenderableGeometry3D*)COREOBJECTMANAGER->GetObjectByHandle(m_hRenderable);

	if(m_numColumns < 2)
	{
		pGeom->material.uniqueUniformValues[0] = (u8*)&m_diffuseColor;
	}
	else
	{
		pGeom->material.uniqueUniformValues[0] = (u8*)&m_texcoordOffset;
		pGeom->material.uniqueUniformValues[1] = (u8*)&m_diffuseColor;
	}
}
