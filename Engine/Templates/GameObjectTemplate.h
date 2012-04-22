//
//  GameObjectTemplate.h
//  Game(OSX)
//
//  Created by Jody McAdams on 4/21/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#ifndef Game_OSX__GameObjectTemplate_h
#define Game_OSX__GameObjectTemplate_h

#include "Engine/CoreGameObject.h"
#include "Engine/MathTypes.h"

class GameObjectTemplate: public CoreGameObject
{
public:
    virtual void UpdateHandle();	//Call when the memory location changes

	virtual bool Init(u32 type);
	virtual bool SpawnInit(void* pSpawnStruct);
	virtual bool PostSpawnInit(void* pSpawnStruct);
	virtual void Uninit();
	virtual void Update(f32 timeElapsed);
	virtual void ProcessMessage(u32 message);	//Pass in a hash value
    
    static bool LoadResourcesForType(u32 type);
    static void InitClass();
};

#endif
