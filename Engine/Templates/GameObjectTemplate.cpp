//
//  GameObjectTemplate.cpp
//  Game(OSX)
//
//  Created by Jody McAdams on 4/21/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#include "GameObjectTemplate.h"

//Engine includes
#include "Engine/MathUtil.h"
#include "Engine/matrix.h"

//----------------------------------------------------------------
//----------------------------------------------------------------
void GameObjectTemplate::InitClass()
{
    //TODO: initialize shared class globals here
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool GameObjectTemplate::LoadResourcesForType(u32 type)
{
    //TODO: call LoadItemArt and LoadItemSound here
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool GameObjectTemplate::Init(u32 type)
{
    //Base class init
    CoreGameObject::Init(type);
    
    //TODO: other init
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool GameObjectTemplate::SpawnInit(void* pSpawnStruct)
{
    //TODO: spawn init here
    //You can read in XML or other data here to
    //construct the object
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool GameObjectTemplate::PostSpawnInit(void* pSpawnStruct)
{
    //TODO: save any links to other objects here
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void GameObjectTemplate::Uninit()
{
    //Base class uninit
    CoreGameObject::Uninit();
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void GameObjectTemplate::Update(f32 timeElapsed)
{
    //TODO: update here
    
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void GameObjectTemplate::UpdateHandle()
{
    //TODO: update anything that has pointers to local
    //member variables, such as material uniform locations
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void GameObjectTemplate::ProcessMessage(u32 message)
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
