//
//  ObjectGroup.cpp
//  Game(iOS)
//
//  Created by Jody McAdams on 12/27/11.
//  Copyright (c) 2011 Jody McAdams. All rights reserved.
//

#include "ObjectGroup.h"

DEFINEFACTORY(ObjectGroup);

#include "Game/Game.h"

#include "Math/matrix.h"
#include "Graphics/OpenGLRenderer.h"
#include "CoreObjects/CoreObject_Manager.h"
#include "CoreObjects/CoreGameObject.h"

#include "Util/Hash.h"


bool ObjectGroup::SpawnInit(void* pSpawnStruct)
{
	m_numObjects = 0;
	
	SpawnableEntity* pSpawnableEnt = (SpawnableEntity*)pSpawnStruct;
	if(pSpawnableEnt == NULL)
	{
		return false;
	}
	
	pugi::xml_node propertiesNode = pSpawnableEnt->node.child("properties");
	
	for (pugi::xml_node property = propertiesNode.child("property"); property; property = property.next_sibling("property"))
	{
		const u32 tiledUniqueID = atoi(property.attribute("value").value());
		
		SpawnableEntity* pEnt = GAME->GetSpawnableEntityByTiledUniqueID(tiledUniqueID);
		if(pEnt != NULL
		   && pEnt->pObject != NULL)
		{
			m_objectHandles[m_numObjects] = pEnt->pObject->GetHandle();
			++m_numObjects;
		}
	}
	
	return true;
}


bool ObjectGroup::ObjectsAreContained(Trigger_2D* pContainer)
{
	for(u32 i=0; i<m_numObjects; ++i)
	{
		CoreGameObject* pObject = (CoreGameObject*)COREOBJECTMANAGER->GetObjectByHandle(m_objectHandles[i]);
		if(pObject == NULL)
		{
			return false;
		}
		
		const vec3* pPos = pObject->GetPosition();
		if(pPos == NULL)
		{
			return false;
		}
		
		if(!pContainer->GetPositionIsInside((vec2*)pPos))
		{
			return false;
		}
	}
	
	return true;
}

void ObjectGroup::SetPosition(const vec3* pPosition)
{
	for(u32 i=0; i<m_numObjects; ++i)
	{
		CoreGameObject* pObject = (CoreGameObject*)COREOBJECTMANAGER->GetObjectByHandle(m_objectHandles[i]);
		if(pObject != NULL)
		{
			pObject->SetPosition(pPosition);
		}
	}
}


void ObjectGroup::ProcessMessage(u32 message, u32 parameter)	//Pass in a hash value
{
	for(u32 i=0; i<m_numObjects; ++i)
	{
		CoreObject* pObject = COREOBJECTMANAGER->GetObjectByHandle(m_objectHandles[i]);
		if(pObject != NULL)
		{
			pObject->ProcessMessage(message,parameter);
		}
	}
}


