//
//  Box2DUtil.cpp
//  CoreEngine3D
//
//  Created by JodyMcAdams on 2/24/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#include "Box2DUtil.h"
#include "ENGINE/Game/Game.h"

vec2* AsVec2(const b2Vec2& box2DVec)
{
	return (vec2*)&box2DVec;
}

const b2Vec2& AsBox2DVec2(const vec2& vec0)
{
    return (b2Vec2&)vec0;
}

b2Body* Box2D_CreateLineBody(f32 x0, f32 y0, f32 x1, f32 y1, u32 categoryBits, u32 maskBits, b2BodyType bodyType, bool isSensor)
{
	b2Body* pOutBody = NULL;

	const f32 pixelsPerMeter = GAME->GetPixelsPerMeter();
	
	b2BodyDef bodyDef;
	bodyDef.type = bodyType;
	
	const vec2 pos0 = {x0/pixelsPerMeter,y0/pixelsPerMeter};
	const vec2 pos1 = {x1/pixelsPerMeter,y1/pixelsPerMeter};
	
	b2EdgeShape shape;
	shape.Set(AsBox2DVec2(pos0), AsBox2DVec2(pos1));
	
	bodyDef.position.Set(0, 0);
	
	pOutBody = GAME->Box2D_GetWorld()->CreateBody(&bodyDef);
	
	{
		b2FixtureDef fixtureDef;
		fixtureDef.density = 1;
		fixtureDef.friction = GAME->m_Box2D_defaultCollisionFriction;
		fixtureDef.shape = &shape;
		fixtureDef.filter.categoryBits = categoryBits;
		fixtureDef.filter.maskBits = maskBits;
		fixtureDef.isSensor = isSensor;
		
		pOutBody->CreateFixture(&fixtureDef);
	}
	
	return pOutBody;
}

void Box2D_CreateCircleFixture(b2FixtureDef* pOut_fixtureDef, b2CircleShape* pOut_Shape, f32 radius, u32 categoryBits, u32 maskBits, f32 density, f32 bounciness, f32 friction, bool isSensor)
{
	const f32 pixelsPerMeter = GAME->GetPixelsPerMeter();
	
	pOut_Shape->m_radius = radius/pixelsPerMeter;
	
	pOut_fixtureDef->shape = pOut_Shape;
	pOut_fixtureDef->isSensor = isSensor;
	pOut_fixtureDef->friction = friction;
	pOut_fixtureDef->filter.categoryBits = categoryBits;
	pOut_fixtureDef->filter.maskBits = maskBits;
	pOut_fixtureDef->restitution = bounciness;
}

b2Body* Box2D_CreateCircleBody(f32 x, f32 y, b2BodyType bodyType, const b2FixtureDef* pfixtureDef)
{
	b2Body* pOutBody = NULL;
	
	const f32 pixelsPerMeter = GAME->GetPixelsPerMeter();
	
	b2BodyDef bd;
	bd.type = bodyType;
	
	const f32 posX = x/pixelsPerMeter;
	const f32 posY = y/pixelsPerMeter;
	
	bd.position.Set(posX, posY);
	
	pOutBody = GAME->Box2D_GetWorld()->CreateBody(&bd);
	
	pOutBody->CreateFixture(pfixtureDef);
	
	return pOutBody;
}



b2Body* Box2D_CreateCircleBody(f32 x, f32 y, f32 radius, u32 categoryBits, u32 maskBits, b2BodyType bodyType, bool isSensor)
{
	b2Body* pOutBody = NULL;
	
	const f32 pixelsPerMeter = GAME->GetPixelsPerMeter();

	b2BodyDef bd;
	bd.type = bodyType;
	
	const f32 posX = x/pixelsPerMeter;
	const f32 posY = y/pixelsPerMeter;
	
	bd.position.Set(posX, posY);
	
	pOutBody = GAME->Box2D_GetWorld()->CreateBody(&bd);

	b2CircleShape shape;
	shape.m_radius = radius/pixelsPerMeter;

	{
		b2FixtureDef fd;
		fd.isSensor = isSensor;
		fd.shape = &shape;
		fd.density = 1.0f;
		fd.restitution = 0.0f;
		fd.friction = 0.0f;
		fd.filter.categoryBits = (u16)categoryBits;
		fd.filter.maskBits = (u16)maskBits;
		
		pOutBody->CreateFixture(&fd);
	}
	
	return pOutBody;
}


void Box2D_CreateAABB(b2AABB* pOut_AABB, f32 x0, f32 y0, f32 x1, f32 y1)
{
	const f32 pixelsPerMeter = GAME->GetPixelsPerMeter();
	
	pOut_AABB->lowerBound.x = x0/pixelsPerMeter;
	pOut_AABB->lowerBound.y = y0/pixelsPerMeter;
	pOut_AABB->upperBound.x = x1/pixelsPerMeter;
	pOut_AABB->upperBound.y = y1/pixelsPerMeter;
}


void Box2D_DrawAABB(const b2AABB& aabb)
{
	const f32 pixelsPerMeter = GAME->GetPixelsPerMeter();
	
	const vec2 lowerBound = {aabb.lowerBound.x*pixelsPerMeter,aabb.lowerBound.y*pixelsPerMeter};
	const vec2 upperBound = {aabb.upperBound.x*pixelsPerMeter,aabb.upperBound.y*pixelsPerMeter};
	
	const vec3 topLeft = {lowerBound.x,lowerBound.y,0.0f};
	const vec3 topRight = {upperBound.x,lowerBound.y,0.0f};
	const vec3 botLeft = {lowerBound.x,upperBound.y,0.0f};
	const vec3 botRight = {upperBound.x,upperBound.y,0.0f};
	
	GLRENDERER->DEBUGDRAW_DrawLineSegment(DebugDrawMode_2D, &topLeft, &topRight, &color4f_red);
	GLRENDERER->DEBUGDRAW_DrawLineSegment(DebugDrawMode_2D, &botLeft, &botRight, &color4f_red);
	GLRENDERER->DEBUGDRAW_DrawLineSegment(DebugDrawMode_2D, &topLeft, &botLeft, &color4f_red);
	GLRENDERER->DEBUGDRAW_DrawLineSegment(DebugDrawMode_2D, &botRight, &topRight, &color4f_red);
}
