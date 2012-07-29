//
//  Trigger_2D.h
//  Game(iOS)
//
//  Created by Jody McAdams on 12/27/11.
//  Copyright (c) 2011 Jody McAdams. All rights reserved.
//

#ifndef __Trigger_2D_h
#define __Trigger_2D_h

#include "Math/MathTypes.h"
#include "Graphics/GraphicsTypes.h"

#include "CoreObjects/CoreObject.h"

#define Trigger_2D_MAX_COLLISION_RESULTS 16

enum Trigger_2D_Type
{
	Trigger_2D_Type_Player,
	Trigger_2D_Type_Moving,
	Trigger_2D_Type_Static,
	Trigger_2D_Type_Ghost,
	Trigger_2D_Type_Trigger,
	Trigger_2D_Type_Num,
};


class Trigger_2D: public CoreObject
{
public:
	struct CollisionResult
	{
		Trigger_2D* pBoxList[Trigger_2D_MAX_COLLISION_RESULTS];
		u32 numBoxes;
	};
	
	virtual bool Init(u32 type);
	virtual void Update(f32 timeElapsed);
	virtual void Uninit();
	virtual bool SpawnInit(void* pSpawnStruct);
	void SpawnInit(const vec3* pRefPos, f32 xLeft, f32 xRight, f32 yTop, f32 yBottom);
	bool GetPositionIsInside(const vec2* pPos);
	
	const vec3* GetTopLeft();
	const vec3* GetBottomLeft();
	const vec3* GetBottomRight();
	const vec3* GetBoxCenter();

	void UpdatePosition(const vec3* pPos);
	
	bool CollideWithWorld(u32 collisionTypeFlags, CollisionResult* pOut_Result);
private:	
	
	f32 m_xLeft;
	f32 m_yTop;
	f32 m_xRight;
	f32 m_yBottom;
	
	vec3 m_refPos;
	vec3 m_currPos;
	
	vec3 m_boxTL;
	vec3 m_boxTR;
	vec3 m_boxBL;
	vec3 m_boxBR;
	
	vec3 m_boxCenter;
	
	Trigger_2D_Type m_type;

	
	void UpdateCornerPositions();
};

void Trigger_2D_Init();

#endif
