//
//  SoundPlayer.cpp
//  Game(iOS)
//
//  Created by Jody McAdams on 12/27/11.
//  Copyright (c) 2011 Jody McAdams. All rights reserved.
//

#include "SoundPlayer.h"

#include "Game/Game.h"

#include "Math/matrix.h"
#include "Graphics/OpenGLRenderer.h"
#include "CoreObjects/CoreObject_Manager.h"
#include "CoreObjects/CoreObjectFactories.h"

#include "Util/Hash.h"


bool SoundPlayer::SpawnInit(void* pSpawnStruct)
{
	SpawnableEntity* pSpawnableEnt = (SpawnableEntity*)pSpawnStruct;
	if(pSpawnableEnt == NULL)
	{
		return false;
	}
	
	/*for (pugi::xml_node property = pSpawnableEnt->pProperties.child("property"); property; property = property.next_sibling("property"))
	{
		const char* propNameString = property.attribute("name").value();
		
		
		SpawnableEntity* pEnt = GAME->GetSpawnableEntityByNameHash(Hash(propNameString));
		if(pEnt->pObject != NULL)
		{
			m_objectHandles[m_numObjects] = pEnt->pObject->GetHandle();
			++m_numObjects;
		}
	}*/
	
	return true;
}


void SoundPlayer::ProcessMessage(u32 message, u32 parameter)	//Pass in a hash value
{
	
}


void SoundPlayer::Uninit()
{
	CoreObject::Uninit();
}

