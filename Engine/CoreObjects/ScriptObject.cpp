//
//  ScriptObject.cpp
//  Game(iOS)
//
//  Created by Jody McAdams on 12/27/11.
//  Copyright (c) 2011 Jody McAdams. All rights reserved.
//

#include "ScriptObject.h"

DEFINEFACTORY(ScriptObject);

#include "Game/Game.h"

#include "Math/matrix.h"
#include "Graphics/OpenGLRenderer.h"
#include "CoreObjects/CoreObject_Manager.h"
#include "CoreObjects/Trigger_2D.h"
#include "CoreObjects/ObjectGroup.h"

#include "Util/Hash.h"

const vec3* ScriptObject::GetPosition() const
{
	return &m_position;
}


bool ScriptObject::SpawnInit(void* pSpawnStruct)
{
	m_isFirstUpdate = true;
	
	SpawnableEntity* pSpawnableEnt = (SpawnableEntity*)pSpawnStruct;
	if(pSpawnableEnt == NULL)
	{
		return false;
	}
	
	CopyVec3(&m_position, &pSpawnableEnt->position);
	
	m_triggerParam = 0;
	m_triggerMessage = 0;
	m_untriggerMessage = 0;
	m_offMessage = 0;
	m_hTriggerObject = CoreObjectHandle();
	
	m_collisionType = 0;
	m_scriptStatus = ScriptObject::ScriptStatus_On;
	m_collMode = CollisionMode_Tile;
	
	m_toggleTimeOn = 0.0f;
	m_toggleTimeOff = 0.0f;
	
	m_action = Action_WaitForCollision;
	
	m_collMode = pSpawnableEnt->tileID == -1 ? ScriptObject::CollisionMode_Box:ScriptObject::CollisionMode_Tile;

	m_numAllowedTriggers = 1;
	
	pugi::xml_node propertiesNode = pSpawnableEnt->node.child("properties");
	
	for (pugi::xml_node property = propertiesNode.child("property"); property; property = property.next_sibling("property"))
	{
		const char* propNameString = property.attribute("name").value();
		const char* valueString = property.attribute("value").value();

		if(strcmp(propNameString,"TriggerMessage") == 0)
		{
			m_triggerMessage = Hash(valueString);
		}
		else if(strcmp(propNameString,"UntriggerMessage") == 0)
		{
			m_untriggerMessage = Hash(valueString);
		}
		else if(strcmp(propNameString,"OffMessage") == 0)
		{
			m_offMessage = Hash(valueString);
		}
		else if(strcmp(propNameString,"NumAllowedTriggers") == 0)
		{
			m_numAllowedTriggers = atoi(valueString);
		}
		else if(strcmp(propNameString,"CollisionType") == 0)
		{
			m_collisionType = Hash(valueString);
		}
		else if(strcmp(propNameString,"ScriptStatus") == 0)
		{
			if(strcmp(valueString,"Off") == 0)
			{
				m_scriptStatus = ScriptObject::ScriptStatus_Off;
			}
		}
		else if(strcmp(propNameString,"Action") == 0)
		{
			if(strcmp(valueString,"TimerToggle") == 0)
			{
				m_action = Action_TimerToggle;
			}
			else if(strcmp(valueString,"WaitForCollision") == 0)
			{
				m_action = Action_WaitForCollision;
			}
			else if(strcmp(valueString,"WaitForObjects") == 0)
			{
				m_action = Action_WaitForObjects;
			}
			else if(strcmp(valueString,"TriggerOnInit") == 0)
			{
				m_action = Action_TriggerOnInit;
			}
		}
		else if(strcmp(propNameString,"ToggleTimeOn") == 0)
		{
			m_toggleTimeOn = static_cast<f32>(atof(valueString));
		}
		else if(strcmp(propNameString,"ToggleTimeOff") == 0)
		{
			m_toggleTimeOff = static_cast<f32>(atof(valueString));
		}
	}


	m_hCollisionBox = CoreObjectHandle();
	
	//Will init the box using our own self!
	if(pSpawnableEnt->tileID == -1)
	{
		const f32 halfWidth = pSpawnableEnt->scale.x/2.0f;
		const f32 halfHeight = pSpawnableEnt->scale.y/2.0f;

		Trigger_2D* pBox = g_Factory_Trigger_2D.CreateObject(Trigger_2D_Type_Trigger);
		pBox->SpawnInit(&m_position, -halfWidth+m_position.x, halfWidth+m_position.x, halfHeight+m_position.y, -halfHeight+m_position.y);

		m_hCollisionBox = pBox->GetHandle();
	}

	m_tileIndex_X = pSpawnableEnt->tileIndexX;
	m_tileIndex_Y = pSpawnableEnt->tileIndexY;

	m_numTriggers = 0;

	m_initialScriptStatus = m_scriptStatus;
	
	m_toggleIsOn = true;
	m_toggleTimer = 0.0f;
	
	if(m_toggleIsOn)
	{
		m_toggleTimer = m_toggleTimeOn;
	}
	else
	{
		m_toggleTimer = m_toggleTimeOff;
	}
	
	return true;
}


bool ScriptObject::PostSpawnInit(void* pSpawnStruct)
{
	if(m_action == Action_TriggerOnInit)
	{
		Trigger(NULL);
	}
	
	SpawnableEntity* pSpawnableEnt = (SpawnableEntity*)pSpawnStruct;
	if(pSpawnableEnt == NULL)
	{
		return false;
	}
	
	pugi::xml_node propertiesNode = pSpawnableEnt->node.child("properties");
	
	for (pugi::xml_node property = propertiesNode.child("property"); property; property = property.next_sibling("property"))
	{
		const char* propNameString = property.attribute("name").value();
		const char* valueString = property.attribute("value").value();
		
		if(strcmp(propNameString,"ObjectParam") == 0)
		{
			const u32 triggerObject = atoi(valueString);
			SpawnableEntity* pEnt = GAME->GetSpawnableEntityByTiledUniqueID(triggerObject);
			if(pEnt != NULL
			   && pEnt->pObject != NULL)
			{
				m_triggerParam = pEnt->pObject->GetHandle();
			}
		}
		else if(strcmp(propNameString,"TriggerObject") == 0)
		{
			const u32 triggerObject = atoi(valueString);
			SpawnableEntity* pEnt = GAME->GetSpawnableEntityByTiledUniqueID(triggerObject);
			if(pEnt != NULL
			   && pEnt->pObject != NULL)
			{
				m_hTriggerObject = pEnt->pObject->GetHandle();
			}
		}
		else if(strcmp(propNameString,"ObjectGroup") == 0)
		{
			const u32 objectGroup = atoi(valueString);
			SpawnableEntity* pEnt = GAME->GetSpawnableEntityByTiledUniqueID(objectGroup);
			if(pEnt != NULL
			   && pEnt->pObject != NULL)
			{
				m_hObjectGroup = pEnt->pObject->GetHandle();
			}
		}
	}
	
	return true;
}

void ScriptObject::SetPosition(const vec3* pPosition)
{
	CopyVec3(&m_position,pPosition);
	
	Trigger_2D* pBox = (Trigger_2D*)COREOBJECTMANAGER->GetObjectByHandle(m_hCollisionBox);
	if(pBox != NULL)
	{
		pBox->UpdatePosition(pPosition);
	}
}


void ScriptObject::AttemptBoxTrigger(CoreGameObject* pObject, const vec3* pPosition)
{
	if(m_action != Action_WaitForCollision)
	{
		return;
	}
	
	if(m_numTriggers == m_numAllowedTriggers)
	{
		return;
	}
	
	if(m_scriptStatus == ScriptStatus_Off)
	{
		return;
	}
	
	if(m_collMode != CollisionMode_Box)
	{
		return;
	}

	if(m_collisionType != 0 && m_collisionType != pObject->GetEntityType())
	{
		return;
	}
	
	Trigger_2D* pBox = (Trigger_2D*)COREOBJECTMANAGER->GetObjectByHandle(m_hCollisionBox);
	if(pBox == NULL)
	{
		return;
	}
	
	if(!pBox->GetPositionIsInside((vec2*)pPosition))
	{
		return;
	}
	
	Trigger(pObject);
}


void ScriptObject::Reset()
{
	m_scriptStatus = m_initialScriptStatus;
	m_numTriggers = 0;
}


void ScriptObject::AttemptTileTrigger(CoreGameObject* pObject, u32 tileIndex_X, u32 tileIndex_Y)
{
	if(m_action != Action_WaitForCollision)
	{
		return;
	}
	
	if(m_numTriggers == m_numAllowedTriggers)
	{
		return;
	}
	
	if(m_scriptStatus == ScriptStatus_Off)
	{
		return;
	}
	
	if(m_collMode != CollisionMode_Tile)
	{
		return;
	}
	
	if(!(m_tileIndex_X == tileIndex_X
		  && m_tileIndex_Y == tileIndex_Y))
	{
		return;
	}

	if(m_collisionType != 0 && m_collisionType != pObject->GetEntityType())
	{
		return;
	}
	
	Trigger(pObject);
}

void ScriptObject::Trigger(CoreGameObject* pObject)
{
	if(m_triggerMessage == Hash("YouWin"))
	{
		GAME->FinishedCurrentLevel();
	}
	else if(m_triggerMessage == Hash("SetFollowCamera"))
	{
		GAME->SetCameraMode(CameraMode_FollowCam);
	}
	else if(m_triggerMessage == Hash("SetCamera"))
	{
		Trigger_2D* pBox = (Trigger_2D*)COREOBJECTMANAGER->GetObjectByHandle(m_hCollisionBox);
		if(pBox == NULL)
		{
			return;
		}
		
		vec3 camPos;
		CopyVec3(&camPos,pBox->GetBottomLeft());
		GAME->SetCameraPosition(&camPos,0.0f);
		
		GAME->SetCameraMode(CameraMode_Anchor);
	}
	else if(m_triggerMessage == Hash("SetCameraAndParallax"))
	{
		Trigger_2D* pBox = (Trigger_2D*)COREOBJECTMANAGER->GetObjectByHandle(m_hCollisionBox);
		if(pBox == NULL)
		{
			return;
		}
		
		vec3 camPos;
		CopyVec3(&camPos,pBox->GetBottomLeft());
		GAME->SetCameraPosition(&camPos,0.0f);
		
		GAME->SetCameraMode(CameraMode_Anchor);
		
		GAME->SetParallaxPosition(&camPos);
	}
	else if(m_triggerMessage == Hash("MoveCamera"))
	{
		Trigger_2D* pCameraBox = (Trigger_2D*)COREOBJECTMANAGER->GetObjectByHandle(m_hTriggerObject);
		
		if(pCameraBox != NULL)
		{
			vec3 camPos;
			CopyVec3(&camPos,pCameraBox->GetBottomLeft());
			GAME->SetCameraPosition(&camPos,1.5f);
		}
		else
		{
			Trigger_2D* pBox = (Trigger_2D*)COREOBJECTMANAGER->GetObjectByHandle(m_hCollisionBox);
			if(pBox == NULL)
			{
				return;
			}
			
			vec3 camPos;
			CopyVec3(&camPos,pBox->GetBottomLeft());
			GAME->SetCameraPosition(&camPos,1.5f);
		}
		
		GAME->SetCameraMode(CameraMode_Anchor);
	}
	else
	{
		//If there's no specified object, this trigger will happen on the
		//object that is calling it
		CoreObject* pTriggerObject = m_hTriggerObject.IsValid() ?
			COREOBJECTMANAGER->GetObjectByHandle(m_hTriggerObject) : pObject;
		
		if(pTriggerObject != NULL)
		{
			pTriggerObject->ProcessMessage(m_triggerMessage,m_triggerParam);
		}
	}
	
	if(m_numAllowedTriggers != -1)
	{
		++m_numTriggers;
		
		if(m_numTriggers == m_numAllowedTriggers)
		{
			//TODO: not sure if I should leave this
			m_scriptStatus = ScriptStatus_Off;
		}
	}
}

void ScriptObject::ProcessMessage(u32 message, u32 parameter)	//Pass in a hash value
{
	if(message == Hash("On"))
	{
		COREDEBUG_PrintDebugMessage("ScriptObject: On");
		m_scriptStatus = ScriptStatus_On;
	}
	else if(message == Hash("Off"))
	{
		COREDEBUG_PrintDebugMessage("ScriptObject: Off");
		m_scriptStatus = ScriptStatus_Off;
		
		CoreObject* pObject = COREOBJECTMANAGER->GetObjectByHandle(m_hTriggerObject);
		
		if(pObject != NULL)
		{
			pObject->ProcessMessage(m_offMessage,0);
		}
	}
}


void ScriptObject::Update(f32 timeElapsed)
{
	//GLRENDERER->DEBUGDRAW_DrawCircleXY(DebugDrawMode_2D, &m_position, 64.0f, &color4f_red);
	
	if(m_scriptStatus == ScriptStatus_Off)
	{
		return;
	}
	
	switch(m_action)
	{
		case Action_TimerToggle:
		{
			m_toggleTimer -= timeElapsed;
			if(m_toggleTimer < 0.0f)
			{
				CoreObject* pObject = COREOBJECTMANAGER->GetObjectByHandle(m_hTriggerObject);
				if(pObject != NULL)
				{
					if(m_toggleIsOn)
					{
						m_toggleTimer = m_toggleTimeOff;
						pObject->ProcessMessage(m_untriggerMessage,0);
					}
					else
					{
						m_toggleTimer = m_toggleTimeOn;
						pObject->ProcessMessage(m_triggerMessage,m_triggerParam);
					}
					
					m_toggleIsOn = !m_toggleIsOn;
				}
			}
			
			break;
		}
		case Action_WaitForCollision:
		{
#ifdef _DEBUG
			
#endif
			break;
		}
		case Action_WaitForObjects:
		{
			Trigger_2D* pBox = (Trigger_2D*)COREOBJECTMANAGER->GetObjectByHandle(m_hCollisionBox);
			if(pBox != NULL)
			{
				ObjectGroup* pGroup = (ObjectGroup*)COREOBJECTMANAGER->GetObjectByHandle(m_hObjectGroup);
				if(pGroup != NULL)
				{
					const bool contained = pGroup->ObjectsAreContained(pBox);
					if(contained)
					{
						CoreObject* pObject = COREOBJECTMANAGER->GetObjectByHandle(m_hTriggerObject);
						if(pObject != NULL)
						{
							pObject->ProcessMessage(m_triggerMessage,m_triggerParam);
							m_scriptStatus = ScriptStatus_Off;
						}
					}
				}
			}

			break;
		}
		default:
		{
			break;
		}
	}
}


bool ScriptObject::GetPositionIsInside(const vec2* pTouchPos)
{
	return true;
}


