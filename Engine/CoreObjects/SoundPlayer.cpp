//
//  SoundPlayer.cpp
//  Game(iOS)
//
//  Created by Jody McAdams on 12/27/11.
//  Copyright (c) 2011 Jody McAdams. All rights reserved.
//

#include "SoundPlayer.h"

DEFINEFACTORY(SoundPlayer);

#include "Game/Game.h"

#include "Math/Matrix.h"
#include "Graphics/OpenGLRenderer.h"
#include "CoreObjects/CoreObject_Manager.h"

#include "Util/Hash.h"

bool SoundPlayer::Init(u32 type)
{
	CoreGameObject::Init(type);
	
	m_hParent = 0;
	m_isAttachedToParent = false;
	
	return true;
}


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

void SoundPlayer::SpawnInit(const vec3* pPos, u32 soundBufferID, f32 volume, f32 pitch, bool isLooping, CoreObjectHandle hParent)
{
	m_soundSource = OPENALAUDIO->CreateSoundSourceFromBuffer(soundBufferID);
	OPENALAUDIO->SetSoundSourcePosition(m_soundSource, pPos);
	OPENALAUDIO->PlaySoundSource(m_soundSource, volume, pitch, isLooping);
	
	m_hParent = hParent;
	
	if(m_hParent != 0)
	{
		m_isAttachedToParent = true;
	}
	
	//COREDEBUG_PrintDebugMessage("Sound started...");
}


void SoundPlayer::Update(f32 timeElapsed)
{
	const bool isStopped = OPENALAUDIO->GetSourceIsStopped(m_soundSource);
	if(isStopped)
	{
		this->DeleteObject();
		
		return;
	}
	
	//Get parent if it exists
	CoreObject* pParent = COREOBJECTMANAGER->GetObjectByHandle(m_hParent);
	
	//Stop sound and delete object if the parent has been deleted
	if(m_isAttachedToParent
	   && pParent == NULL)
	{
		this->DeleteObject();
		
		return;
	}
	
	//Update position of sound source if attached to a parent object
	if(pParent != NULL
	   && pParent->GetType() == CoreObjectType_CoreGameObject)
	{
		CoreGameObject* pParentGameObject = (CoreGameObject*)pParent;
		const vec3* pPosition = pParentGameObject->GetPosition();
		if(pPosition != NULL)
		{
			OPENALAUDIO->SetSoundSourcePosition(m_soundSource, pPosition);
		}
	}
}


void SoundPlayer::ProcessMessage(u32 message, u32 parameter)	//Pass in a hash value
{
	
}


void SoundPlayer::Uninit()
{
	//COREDEBUG_PrintDebugMessage("Sound deleted!");
	OPENALAUDIO->DeleteSoundSource(&m_soundSource);
	
	CoreObject::Uninit();
}

