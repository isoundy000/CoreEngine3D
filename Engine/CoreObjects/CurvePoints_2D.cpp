//
//  CurvePoints_2D.cpp
//  CoreEngine3D
//
//  Created by Jody McAdams on 4/21/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#include "CurvePoints_2D.h"

DEFINEFACTORY(CurvePoints_2D);

//Engine includes
#include "Math/MathUtil.h"
#include "Math/Matrix.h"
#include "Game/Game.h"
#include "Util/GameUtil.h"

//Art
/*static TextureAsset g_Art_SpaceShip =
{
	"ArtResources/Characters/SpaceShip/SpaceShip.png",,
	ImageType_PNG,
	0,
	GL_LINEAR,
	GL_CLAMP_TO_EDGE,
	GL_CLAMP_TO_EDGE,
	true,
};*/


//Sound
/*static ItemSoundDescription g_Sound_SpaceShipBeam =
{
	"ArtResources/SFX/SpaceShipBeam.wav",
	0,//soundBufferID
};*/


//----------------------------------------------------------------
//----------------------------------------------------------------
void CurvePoints_2D::InitClass()
{
    //TODO: initialize shared class globals here
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CurvePoints_2D::LoadResourcesForType(u32 type)
{
    //TODO: call GAME->AddItemArt(&g_Art_SpaceShip)
	//TODO: call GAME->AddItemSound(&g_Sound_SpaceShipBeam)
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
const LinePointList* CurvePoints_2D::GetPointList()
{
	return &m_pointList;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CurvePoints_2D::Init(u32 type)
{
    //Base class init
    CoreGameObject::Init(type);
    
    //TODO: other init
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CurvePoints_2D::SpawnInit(void* pSpawnStruct)
{
    //TODO: spawn init here
    //You can read in XML or other data here to
    //construct the object
	SpawnableEntity* pSpawnableEnt = (SpawnableEntity*)pSpawnStruct;
	if(pSpawnableEnt == NULL)
	{
		return false;
	}
	
	GU_Create2DPathPointsFromXML(pSpawnableEnt->node, &m_pointList, false);
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CurvePoints_2D::PostSpawnInit(void* pSpawnStruct)
{
    //TODO: save any links to other objects here
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CurvePoints_2D::Uninit()
{
    //Base class uninit
    CoreGameObject::Uninit();
	
	delete[] m_pointList.pPoints;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CurvePoints_2D::Update(f32 timeElapsed)
{
    //TODO: update here
#if defined(_DEBUG)
	const u32 numIterations = m_pointList.numPoints-1;
    for(u32 i=0; i<numIterations; ++i)
	{
		vec3 pointA;
		vec3 pointB;
		CopyVec2(AsVec2(&pointA), &m_pointList.pPoints[i]);
		CopyVec2(AsVec2(&pointB), &m_pointList.pPoints[i+1]);
		pointA.z = 0.0f;
		pointB.z = 0.0f;
		
		GLRENDERER->DEBUGDRAW_DrawLineSegment(DebugDrawMode_2D, &pointA, &pointB, &color4f_yellow);
	}
#endif
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CurvePoints_2D::UpdatePointers()
{
	CoreObject::UpdatePointers();
	
    //TODO: update anything that has pointers to local
    //member variables, such as material uniform locations
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CurvePoints_2D::ProcessMessage(u32 message, u32 parameter)
{
    //TODO: switch statement based on message
    
    /*switch(message)
    {
        case 0:
        {
            break;
        }
        default:
        {
            break;
        }
    }*/
};

