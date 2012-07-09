/*
 *  GameUtil.c
 *  CoreEngine3D
 *
 *  Created by Jody McAdams on 1/6/11.
 *  Copyright 2011 GameJelly Online. All rights reserved.
 *
 */

#include "GameUtil.h"
#include <math.h>
#include "MathUtil.h"
#include <Box2D/Box2D.h>
#include "Box2DUtil.h"
#include "Game.h"


//*****************************************************************************
// Creator: JAM
// Calculates velocity needed for simple ballistic motion so that the object
// goes from pStartPos to pEndPos with the given gravity. The top of the arc
// is at MAX(in_startPos->y,in_endPos->y) + in_jumpHeight
// Note: Takes a positive gravity.
// Note: If you count up the timeElapsed, the jump will end when
// (totalTimeElapsed == out_totalJumpTime)
//
// Note: your update code should look like this:
// VEC_AddScaledXYZ(&pPvars->boulderShrapnel.pMoby->matrix.pos,&pContent->jumpVelocity,G_dtMultiplier);
// pContent->jumpVelocity.y -= gravity*G_dtMultiplier;
// pContent->elapsedJumpTime += timeElapsed;
//
// Note: For extra accuracy with landing, make sure you subtract off your timeElapsed when/if it
// goes above out_totalJumpTime
//
// Returns NOTHING!
//*****************************************************************************
void GU_LaunchToPoint(vec3* out_jumpVel, f32* out_totalJumpTime, const vec3* in_startPos, const vec3* in_endPos, f32 in_jumpHeight, f32 in_gravity)
{
	const f32 highestY = MaxF(in_startPos->y, in_endPos->y);
	const f32 apexY = highestY + in_jumpHeight;
	
	const f32 s1 = apexY - in_startPos->y;
	const f32 s2 = apexY - in_endPos->y;
	const f32 s3 = in_endPos->z-in_startPos->z;
	const f32 s4 = in_endPos->x-in_startPos->x;
	
	const f32 t1 = sqrtf(2.0f * s1 / in_gravity);
	const f32 t2 = sqrtf(2.0f * s2 / in_gravity);
	
	const f32 totalTime = t1 + t2;
	
	
	if(out_jumpVel)
	{
		out_jumpVel->y = in_gravity * t1;
		out_jumpVel->z = s3/totalTime;
		out_jumpVel->x = s4/totalTime;
	}
	
	if(out_totalJumpTime)
	{
		*out_totalJumpTime = totalTime;
	}
}

//*****************************************************************************
//*****************************************************************************
void GU_InsertPositiveValueAsBits(u32* pOriginalNumber, u32 value, u32 bitPosition0to31, u32 numBitsToWrite)
{
	u32 insertNumberShifted = value << (32-bitPosition0to31-numBitsToWrite);
	*pOriginalNumber |= insertNumberShifted;
}


//*****************************************************************************
//*****************************************************************************
void GU_TMXStringToPoints(const char* valueString, f32 posX, f32 posY, vec2* pOut_Points, u32* pOut_NumPoints)
{
	const f32 unitConversionScale = GAME->GetTileUnitConversionScale();
	
	char buffer[64];
	u32 bufferIndex;
	
	u32 numPoints = 0;
	bufferIndex = 0;
	bool leftNumber = true;
	const s32 strLen = strlen(valueString);
	for(u32 strIDX=0; strIDX<strLen+1; ++strIDX)
	{
		const char currChar = valueString[strIDX];
		
		if(currChar == ','
		   || currChar == ' '
		   || currChar == '"'
		   || strIDX == strLen)
		{
			buffer[bufferIndex] = ' ';
			
			const f32 floatValue = atof(buffer);
			
			if(leftNumber)
			{
				pOut_Points[numPoints].x = (posX+floatValue)*unitConversionScale;
			}
			else
			{
				pOut_Points[numPoints].y = (posY+floatValue)*unitConversionScale;
				++numPoints;
			}
			
			bufferIndex = 0;
			leftNumber = !leftNumber;
		}
		else
		{
			buffer[bufferIndex] = currChar;
			++bufferIndex;
		}
	}
	
	*pOut_NumPoints = numPoints;
}


//*****************************************************************************
//*****************************************************************************
void GU_Create2DPathPointsFromXML(const pugi::xml_node& node, LinePointList* pOut_LinePointList, bool isCollision)
{
	vec2 polyLinePoints[64];
	u32 numPolyPoints;
	
	const f32 posX = (f32)atoi(node.attribute("x").value());
	const f32 posY = (f32)atoi(node.attribute("y").value());
	
	bool isPolygon = false;
	
	pugi::xml_node pointList = node.child("polyline");
	if(pointList.empty() == true)
	{
		pointList = node.child("polygon");
		isPolygon = true;
	}
	
	if(pointList.empty() == false)
	{
		pugi::xml_attribute points = pointList.attribute("points");
		const char* polyLineString = points.value();
		
		GU_TMXStringToPoints(polyLineString, posX, posY, polyLinePoints, &numPolyPoints);
		
		//Create a new line segment for use with shadows, etc.
		pOut_LinePointList->numPoints = numPolyPoints;
		pOut_LinePointList->pPoints = new vec2[numPolyPoints];
		
		//Process all the points
		for(u32 vertIDX=0; vertIDX<numPolyPoints; ++vertIDX)
		{
			vec2* pVec = &polyLinePoints[vertIDX];
			
			//Copy the line version transformation for use with
			//fancy things like shadows
			CopyVec2(&pOut_LinePointList->pPoints[vertIDX],pVec);
			
			if(isCollision)
			{
				//Convert the point for box2D physics
				ScaleVec2_Self(pVec,1.0f/GAME->GetPixelsPerMeter());
			}
		}

		if(isCollision == true)
		{
			for(u32 vertIDX=0; vertIDX<numPolyPoints-1; ++vertIDX)
			{
				b2BodyDef bodyDef;
				bodyDef.type = b2_staticBody;
				
				b2FixtureDef fixtureDef;
				fixtureDef.density = 1;
				fixtureDef.friction = GAME->m_Box2D_defaultCollisionFriction;
				
				b2EdgeShape shape;
				shape.Set(AsBox2DVec2(polyLinePoints[vertIDX]), AsBox2DVec2(polyLinePoints[vertIDX+1]));
				
				if(vertIDX > 0)
				{
					shape.m_hasVertex0 = true;
					shape.m_vertex0 = AsBox2DVec2(polyLinePoints[vertIDX-1]);
				}
				
				if(vertIDX < numPolyPoints-2)
				{
					shape.m_hasVertex3 = true;
					shape.m_vertex3 = AsBox2DVec2(polyLinePoints[vertIDX+2]);
				}
				
				fixtureDef.shape = &shape;
				fixtureDef.filter.categoryBits = 1 << CollisionFilter_Ground;
				fixtureDef.filter.maskBits = 0xFFFF;
				
				bodyDef.position.Set(0, 0);
				
				b2Body* pBody = GAME->Box2D_GetWorld()->CreateBody(&bodyDef);
				pBody->CreateFixture(&fixtureDef);
			}
			
			if(isPolygon == true && numPolyPoints > 2)
			{
				b2BodyDef bodyDef;
				bodyDef.type = b2_staticBody;
				
				b2FixtureDef fixtureDef;
				fixtureDef.density = 1;
				fixtureDef.friction = GAME->m_Box2D_defaultCollisionFriction;
				
				b2EdgeShape shape;
				shape.Set(AsBox2DVec2(polyLinePoints[numPolyPoints-1]), AsBox2DVec2(polyLinePoints[0]));
				
				shape.m_hasVertex0 = true;
				shape.m_vertex0 = AsBox2DVec2(polyLinePoints[numPolyPoints-2]);
				
				
				shape.m_hasVertex3 = true;
				shape.m_vertex3 = AsBox2DVec2(polyLinePoints[1]);
				
				fixtureDef.shape = &shape;
				fixtureDef.filter.categoryBits = 1 << CollisionFilter_Ground;
				fixtureDef.filter.maskBits = 0xFFFF;
				
				bodyDef.position.Set(0, 0);
				
				b2Body* pBody = GAME->Box2D_GetWorld()->CreateBody(&bodyDef);
				pBody->CreateFixture(&fixtureDef);
			}
		}
	}
}
