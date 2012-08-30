//
//  TileAffector.cpp
//  Game(iOS)
//
//  Created by Jody McAdams on 12/27/11.
//  Copyright (c) 2011 Jody McAdams. All rights reserved.
//

#include "TileAffector.h"
DEFINEFACTORY(TileAffector);

#include "Game/Game.h"

#include "Math/Matrix.h"
#include "Graphics/OpenGLRenderer.h"
#include "CoreObjects/CoreObject_Manager.h"

#include "Util/Hash.h"


bool TileAffector::SpawnInit(void* pSpawnStruct)
{
	SpawnableEntity* pSpawnableEnt = (SpawnableEntity*)pSpawnStruct;
	if(pSpawnableEnt == NULL)
	{
		return false;
	}
	
	m_tileIndex_X = pSpawnableEnt->tileIndexX;
	m_tileIndex_Y = pSpawnableEnt->tileIndexY;
	
	pugi::xml_node propertiesNode = pSpawnableEnt->node.child("properties");
	
	for (pugi::xml_node property = propertiesNode.child("property"); property; property = property.next_sibling("property"))
	{
		
	}
	
	return true;
}


void TileAffector::ProcessMessage(u32 message, u32 parameter)	//Pass in a hash value
{
	if(message == Hash("On"))
	{
		GAME->ToggleTileVisibility(LevelLayer_Main1,m_tileIndex_X,m_tileIndex_Y,true);
	}
	else if(message == Hash("Off"))
	{
		GAME->ToggleTileVisibility(LevelLayer_Main1,m_tileIndex_X,m_tileIndex_Y,false);
	}
}


void TileAffector::Uninit()
{
	CoreObject::Uninit();
}

