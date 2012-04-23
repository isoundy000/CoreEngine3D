//
//  GameObjectTemplate.cpp
//  CoreEngine3D
//
//  Created by Jody McAdams on 4/21/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#include "GameObjectTemplate.h"

//Engine includes
#include "Engine/MathUtil.h"
#include "Engine/matrix.h"
#include "Engine/Game.h"

//Game includes
#include "GameModels.h"


//Materials
/*static const MaterialSettings g_Material_AlphaBlendedUseView =
{
	GL_LINEAR,
	GL_CLAMP_TO_EDGE,
	GL_CLAMP_TO_EDGE,
	MT_TextureOnlySimple,
	RenderFlagDefaults_2DTexture_AlphaBlended_UseView,
	true,
};*/

//Art
/*static ItemArtDescription g_Art_SpaceShip =
{
    "ArtResources/Characters/SpaceShip/SpaceShip.png",
    ImageType_PNG,
    0,
    &g_Material_AlphaBlendedUseView,
    &g_Square1x1_modelData,
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
