//
//  Box2DDebugDraw.h
//  CoreEngine3D
//
//  Created by Jody McAdams on 2/19/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#ifndef CoreEngine3D_iOS__Box2DDebugDraw_h
#define CoreEngine3D_iOS__Box2DDebugDraw_h

#include <Box2D/Box2D.h>

class Box2DDebugDraw: public b2Draw
{
public:
	//Destructor
	virtual ~Box2DDebugDraw() {}
	
	/// Draw a closed polygon provided in CCW order.
	virtual void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	
	/// Draw a solid closed polygon provided in CCW order.
	virtual void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
	
	/// Draw a circle.
	virtual void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);
	
	/// Draw a solid circle.
	virtual void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);
	
	/// Draw a line segment.
	virtual void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);
	
	/// Draw a transform. Choose your own length scale.
	/// @param xf a transform.
	virtual void DrawTransform(const b2Transform& xf);
};

#endif
