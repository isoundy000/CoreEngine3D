//
//  GameObjectTemplate.h
//  CoreEngine3D
//
//  Created by Jody McAdams on 4/21/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#ifndef Game_GameObjectTemplate_h
#define Game_GameObjectTemplate_h

//ENGINE
#include "ENGINE/CoreObjects/CoreGameObject.h"
#include "ENGINE/CoreObjects/CoreObject_Factory.h"
#include "ENGINE/Math/MathTypes.h"
#include "ENGINE/Graphics/GraphicsTypes.h"

class GameObjectTemplate: public CoreGameObject
{
public:
    virtual void UpdatePointers();	//Called when the memory location changes

	virtual bool Init(u32 type);	//Init variables
	virtual bool SpawnInit(void* pSpawnStruct);	//Create object from data
	virtual bool PostSpawnInit(void* pSpawnStruct);	//Link to objects in level
	virtual void Uninit();	//Delete spawned objects here
	virtual void Update(f32 timeElapsed);	//Update the object
	virtual void ProcessMessage(u32 message);	//Receive/Process messages
    
    static bool LoadResourcesForType(u32 type); //Load art/sounds
    static void InitClass();	//One-time global init
    
private:
    //TODO: put member functions here
    //TODO: put member variables here
};

DECLAREFACTORY(GameObjectTemplate);

#endif
