//
//  GameObjectTemplate.cpp
//  CoreEngine3D
//
//  Created by Jody McAdams on 4/21/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#include "GameObjectTemplate.h"

DEFINEFACTORY(GameObjectTemplate);

//ENGINE
#include "ENGINE/Math/MathUtil.h"
#include "ENGINE/Math/matrix.h"
#include "ENGINE/Game/Game.h"
//#include "ENGINE/CoreObjects/SoundPlayer.h"

//GAME
#include "GameModels.h"

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
void GameObjectTemplate::InitClass()
{
    //TODO: initialize shared class globals here
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool GameObjectTemplate::LoadResourcesForType(u32 type)
{
    //TODO: call GAME->AddItemArt(&g_Art_SpaceShip)
	//TODO: call GAME->AddItemSound(&g_Sound_SpaceShipBeam)
    
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

