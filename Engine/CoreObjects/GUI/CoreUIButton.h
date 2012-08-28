//
//  CoreUIButton.h
//  CoreEngine3D
//
//  Created by Jody McAdams on 4/21/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#ifndef Game_OSX__CoreUIButton_h
#define Game_OSX__CoreUIButton_h

#include "CoreObjects/CoreGameObject.h"
#include "MathTypes.h"
#include "GraphicsTypes.h"
#include "CoreUIView.h"
#include "Input/CoreInput_DeviceInputState.h"
#include "CoreObjects/CoreObject_Factory.h"

class CoreUIButton: public CoreUIView
{
public:
	enum CoreUI_ButtonState
	{
		CoreUI_ButtonState_Idle,
		CoreUI_ButtonState_Hover,
		CoreUI_ButtonState_Pressed,
		CoreUI_ButtonState_Num,
	};
	
    virtual void UpdatePointers();	//Called when the memory location changes

	virtual bool Init(u32 type);	//Init variables
	virtual bool SpawnInit(void* pSpawnStruct, CoreObjectHandle hParent);	//Create object from data
	
	virtual void Uninit();	//Delete spawned objects here
	virtual void Update(f32 timeElapsed);	//Update the object
	virtual void ProcessMessage(u32 message, u32 parameter);	//Receive/Process messages
    
    static bool LoadResourcesForType(u32 type); //Load art/sounds
    static void InitClass();	//One-time global init
    
	virtual void LayoutView();
	
	bool UpdateButton(bool buttonIsDown, intVec2* pBeginTouchPos, intVec2* pCurrTouchPos);

private:
    CoreObjectHandle m_hView_Idle;
	CoreObjectHandle m_hView_Hover;
	CoreObjectHandle m_hView_Pressed;
	
	CoreUI_ButtonState m_buttonState;
};

DECLAREFACTORY(CoreUIButton);

#endif
