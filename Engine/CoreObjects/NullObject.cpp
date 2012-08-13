//
//  NullObject.cpp
//  CoreEngine3D
//
//  Created by Jody McAdams on 4/21/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#include "NullObject.h"

//Engine includes
#include "Game/Game.h"


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
/*static TextureAsset g_Art_SpaceShip =
{
    "ArtResources/Characters/SpaceShip/SpaceShip.png",
    ImageType_PNG,
    0,
    &g_Material_AlphaBlendedUseView,
    &g_Square1x1_modelData,
};*/

//Sound
/*static SoundAsset g_Sound_SpaceShipBeam =
{
	"ArtResources/SFX/SpaceShipBeam.wav",
	0,//soundBufferID
};*/


//----------------------------------------------------------------
//----------------------------------------------------------------
void NullObject::InitClass()
{
    //TODO: initialize shared class globals here
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool NullObject::LoadResourcesForType(u32 type)
{
    //TODO: call GAME->AddTextureResource(&g_Art_SpaceShip)
	//TODO: call GAME->AddSoundResource(&g_Sound_SpaceShipBeam)
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool NullObject::Init(u32 type)
{
    //Base class init
    CoreGameObject::Init(type);
    
    //TODO: other init
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool NullObject::SpawnInit(void* pSpawnStruct)
{
    //TODO: spawn init here
    //You can read in XML or other data here to
    //construct the object
	
	SpawnableEntity* pSpawnableEnt = (SpawnableEntity*)pSpawnStruct;
	if(pSpawnableEnt == NULL)
	{
		return false;
	}
	
	CopyVec3(&m_position, &pSpawnableEnt->position);
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool NullObject::PostSpawnInit(void* pSpawnStruct)
{
    //TODO: save any links to other objects here
	
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
const vec3* NullObject::GetPosition() const
{
	return &m_position;
}

//----------------------------------------------------------------
//----------------------------------------------------------------
void NullObject::Uninit()
{
    //Base class uninit
    CoreGameObject::Uninit();
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void NullObject::Update(f32 timeElapsed)
{
    //TODO: update here
    
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void NullObject::UpdatePointers()
{
    //TODO: update anything that has pointers to local
    //member variables, such as material uniform locations
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void NullObject::ProcessMessage(u32 message, u32 parameter)
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
