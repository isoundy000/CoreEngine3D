//
//  ObjectGroup.h
//  Game(iOS)
//
//  Created by Jody McAdams on 12/27/11.
//  Copyright (c) 2011 Jody McAdams. All rights reserved.
//

#ifndef __ObjectGroup_h
#define __ObjectGroup_h

#include "Math/MathTypes.h"
#include "Graphics/GraphicsTypes.h"

#include "CoreObjects/CoreGameObject.h"
#include "CoreObjects/CoreObject_Factory.h"
#include "Game/Game.h"
#include "Trigger_2D.h"

#define ObjectGroup_MAX_OBJECTS 16

class ObjectGroup: public CoreGameObject
{
public:
	virtual void ProcessMessage(u32 message, u32 parameter);	//Pass in a hash value

	virtual bool SpawnInit(void* pSpawnStruct);
	virtual void SetPosition(const vec3* pPosition);
	
	bool ObjectsAreContained(Trigger_2D* pContainer);
	
private:	

	CoreObjectHandle m_objectHandles[ObjectGroup_MAX_OBJECTS];
	u32 m_numObjects;
};

void ObjectGroup_Init();

DECLAREFACTORY(ObjectGroup);

#endif
