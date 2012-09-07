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

b2Body* Box2D_CreateLineBody(f32 x0, f32 y0, f32 x1, f32 y1, u32 categoryBits, u32 maskBits, b2BodyType bodyType)
{
	b2Body* pOutBody = NULL;
	
	b2BodyDef bodyDef;
	bodyDef.type = bodyType;
	
	const vec2 pos0 = {x0,y0};
	const vec2 pos1 = {x1,y1};
	
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
		
		pOutBody->CreateFixture(&fixtureDef);
	}
	
	return pOutBody;
}

b2Body* Box2D_CreateCircleBody(f32 x, f32 y, f32 radius, u32 categoryBits, u32 maskBits, b2BodyType bodyType)
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
		fd.isSensor = false;
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