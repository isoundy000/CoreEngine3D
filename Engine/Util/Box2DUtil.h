//
//  Box2DUtil.h
//  CoreEngine3D
//
//  Created by JodyMcAdams on 2/24/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#ifndef CoreEngine3D_iOS__Box2DUtil_h
#define CoreEngine3D_iOS__Box2DUtil_h

#include <Box2D/Box2D.h>
#include "MathTypes.h"

vec2* AsVec2(const b2Vec2& box2DVec);
const b2Vec2& AsBox2DVec2(const vec2& vec0);

//Bodies
b2Body* Box2D_CreateCircleBody(f32 x, f32 y, f32 radius, u32 categoryBits, u32 maskBits, b2BodyType bodyType, bool isSensor = false);
b2Body* Box2D_CreateCircleBody(f32 x, f32 y, b2BodyType bodyType, const b2FixtureDef* pfixtureDef);

b2Body* Box2D_CreateLineBody(f32 x0, f32 y0, f32 x1, f32 y1, u32 categoryBits, u32 maskBits, b2BodyType bodyType, bool isSensor = false);

void Box2D_CreateAABB(b2AABB* pOut_AABB, f32 x0, f32 y0, f32 x1, f32 y1);

void Box2D_DrawAABB(const b2AABB& aabb);

//Fixtures
void Box2D_CreateCircleFixture(b2FixtureDef* pOut_fixtureDef, b2CircleShape* pOut_Shape, f32 radius, u32 categoryBits, u32 maskBits, f32 density, f32 bounciness, f32 friction, bool isSensor);
//TODO: make these
//b2Body* Box2D_CreateLineBody(f32 x0, f32 y0, f32 x1, f32 y1, b2BodyType bodyType, const b2FixtureDef* pfixtureDef);

#endif
