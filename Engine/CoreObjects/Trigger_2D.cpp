//
//  Trigger_2D.cpp
//  Game(iOS)
//
//  Created by Jody McAdams on 12/27/11.
//  Copyright (c) 2011 Jody McAdams. All rights reserved.
//

#include "Trigger_2D.h"

DEFINEFACTORY(Trigger_2D);

#include "Game/Game.h"

#include "Math/matrix.h"
#include "Graphics/OpenGLRenderer.h"
#include "CoreObjects/CoreObject_Manager.h"

bool Trigger_2D::SpawnInit(void* pSpawnStruct)
{
	SpawnableEntity* pSpawnableEnt = (SpawnableEntity*)pSpawnStruct;
	if(pSpawnableEnt == NULL)
	{
		return false;
	}

	const f32 halfWidth = pSpawnableEnt->scale.x/2.0f;
	const f32 halfHeight = pSpawnableEnt->scale.y/2.0f;

	SpawnInit(&pSpawnableEnt->position, -halfWidth+pSpawnableEnt->position.x, halfWidth+pSpawnableEnt->position.x, halfHeight+pSpawnableEnt->position.y, -halfHeight+pSpawnableEnt->position.y);
	
	return false;
}

void Trigger_2D::SpawnInit(const vec3* pRefPos, f32 xLeft, f32 xRight, f32 yTop, f32 yBottom)
{
	CopyVec3(&m_refPos,pRefPos);
	CopyVec3(&m_currPos,pRefPos);
	 
	m_xLeft = xLeft;
	m_xRight = xRight;
	m_yTop = yTop;
	m_yBottom = yBottom;
	
	UpdateCornerPositions();
}


void Trigger_2D::UpdatePosition(const vec3* pPos)
{
	CopyVec3(&m_currPos,pPos);
	
	UpdateCornerPositions();
}


void Trigger_2D::UpdateCornerPositions()
{
	//All we can do here is maybe draw something
	vec3 offset;
	SubVec3(&offset,&m_currPos,&m_refPos);
	
	m_boxTL.z = 0.0f;
	m_boxTR.z = 0.0f;
	m_boxBL.z = 0.0f;
	m_boxBR.z = 0.0f;
	
	m_boxTL.x = m_xLeft+offset.x;
	m_boxTL.y = m_yTop+offset.y;
	
	m_boxTR.x = m_xRight+offset.x;
	m_boxTR.y = m_boxTL.y;
	
	m_boxBL.x = m_boxTL.x;
	m_boxBL.y = m_yBottom+offset.y;
	
	m_boxBR.x = m_boxTR.x;
	m_boxBR.y = m_boxBL.y;
	
	//TODO: rename this function?
	
	m_boxCenter.x = (m_boxBL.x+m_boxBR.x) * 0.5f;
	m_boxCenter.y = (m_boxTL.y+m_boxBR.y) * 0.5f;
	m_boxCenter.z = 0.0f;
}


bool Trigger_2D::Init(u32 type)
{
	CoreObject::Init(type);
	
	m_type = (Trigger_2D_Type)type;
	
	return true;
}


bool Trigger_2D::GetPositionIsInside(const vec2* pPos)
{
	if(pPos->x > m_boxTL.x
	   && pPos->x < m_boxBR.x
	   && pPos->y > m_boxBR.y
	   && pPos->y < m_boxTL.y)
	{
		return true;
	}
	
	return false;
}

const vec3* Trigger_2D::GetBottomLeft()
{
	return &m_boxBL;
}

const vec3* Trigger_2D::GetTopLeft()
{
	return &m_boxTL;
}

const vec3* Trigger_2D::GetBottomRight()
{
	return &m_boxBR;
}

const vec3* Trigger_2D::GetBoxCenter()
{
	return &m_boxCenter;
}

bool Trigger_2D::CollideWithWorld(u32 collisionTypeFlags, CollisionResult* pOut_Result)
{
	//NOTE: This function is probably bad so you probably
	//shouldn't use it very much
	
	bool foundSomething = false;
	
	pOut_Result->numBoxes = 0;
	
	for(u32 i=0; i<g_Factory_Trigger_2D.m_numObjects; ++i)
	{
		Trigger_2D* pBox = &g_Factory_Trigger_2D.m_pObjectList[i];
		if(pBox == this)
		{
			continue;
		}
		
		if(collisionTypeFlags & (1 << pBox->m_type))
		{
			bool isInside = false;
			
			if(pBox->GetPositionIsInside((vec2*)&m_boxTL))
			{
				isInside = true;
			}
			else if(pBox->GetPositionIsInside((vec2*)&m_boxTR))
			{
				isInside = true;
			}
			else if(pBox->GetPositionIsInside((vec2*)&m_boxBL))
			{
				isInside = true;
			}
			else if(pBox->GetPositionIsInside((vec2*)&m_boxBR))
			{
				isInside = true;
			}
			
			if(isInside)
			{
				pOut_Result->pBoxList[pOut_Result->numBoxes] = pBox;
				++pOut_Result->numBoxes;
				
				foundSomething = true;
			}
			
			//Stop when we run out of boxes
			if(pOut_Result->numBoxes == Trigger_2D_MAX_COLLISION_RESULTS)
			{
				break;
			}
		}
	}
	
	return foundSomething;
}


void Trigger_2D::Update(f32 timeElapsed)
{
	
#ifdef _DEBUG	
	
	//DEBUG DRAW!
	
	vec4 color;
	
	switch((Trigger_2D_Type)m_type)
	{
		case Trigger_2D_Type_Player:
		{
			CopyVec4(&color,&color4f_green);
			
			break;
		}
		case Trigger_2D_Type_Moving:
		{
			CopyVec4(&color,&color4f_yellow);
			
			break;
		}
		case Trigger_2D_Type_Static:
		{
			CopyVec4(&color,&color4f_red);
			
			break;
		}
		case Trigger_2D_Type_Ghost:
		{
			CopyVec4(&color,&color4f_pink);
			
			break;
		}
		case Trigger_2D_Type_Trigger:
		{
			CopyVec4(&color,&color4f_white);
			
			break;
		}
		default:
		{
			CopyVec4(&color,&color4f_blue);
			
			break;
		}
	}
	
	GLRENDERER->DEBUGDRAW_DrawLineSegment(DebugDrawMode_2D, &m_boxTL, &m_boxTR, &color);
	
	GLRENDERER->DEBUGDRAW_DrawLineSegment(DebugDrawMode_2D, &m_boxBL, &m_boxBR, &color);
	
	GLRENDERER->DEBUGDRAW_DrawLineSegment(DebugDrawMode_2D, &m_boxTL, &m_boxBL, &color);
	
	GLRENDERER->DEBUGDRAW_DrawLineSegment(DebugDrawMode_2D, &m_boxTR, &m_boxBR, &color);
#endif
	
}

void Trigger_2D::Uninit()
{
	CoreObject::Uninit();
}

