//
//  CoreUIButton.cpp
//  CoreEngine3D
//
//  Created by Jody McAdams on 4/21/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#include "CoreUIButton.h"

//Engine includes
#include "Math/MathUtil.h"
#include "Math/matrix.h"
#include "Game/Game.h"

//Game includes
#include "EngineModels.h"


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIButton::InitClass()
{
    //TODO: initialize shared class globals here
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CoreUIButton::LoadResourcesForType(u32 type)
{
    //TODO: call GAME->AddItemArt(&g_Art_SpaceShip)
	//TODO: call GAME->AddItemSound(&g_Sound_SpaceShipBeam)
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CoreUIButton::Init(u32 type)
{
    //Base class init
    CoreUIView::Init(type);
    
    //TODO: other init
	viewType = CoreUI_ViewType_Button;
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CoreUIButton::SpawnInit(void* pSpawnStruct, CoreObjectHandle hParent)
{
    pugi::xml_node* pProperties = (pugi::xml_node*)pSpawnStruct;
	
	//First do the standard spawn init to bring
	//in all the attributes
	CoreUIView::SpawnInit(pProperties, hParent);
    
	//TODO: button specific attributes
	
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
bool CoreUIButton::PostSpawnInit(void* pSpawnStruct)
{
    //TODO: save any links to other objects here
    
    return true;
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIButton::Uninit()
{
    //Base class uninit
    CoreGameObject::Uninit();
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIButton::Update(f32 timeElapsed)
{
    //TODO: update here
    CoreUIView::Update(timeElapsed);
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIButton::UpdateHandle()
{
    //TODO: update anything that has pointers to local
    //member variables, such as material uniform locations
}


//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIButton::ProcessMessage(u32 message, u32 parameter)
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

//----------------------------------------------------------------
//----------------------------------------------------------------
void CoreUIButton::LayoutView()
{
	CoreUIView::LayoutView();
	
	//TODO: Do more stuff
}
