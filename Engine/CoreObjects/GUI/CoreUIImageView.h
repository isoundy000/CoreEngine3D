//
//  CoreUIImageView.h
//  CoreEngine3D
//
//  Created by Jody McAdams on 4/21/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#ifndef Game_OSX__CoreUIImageView_h
#define Game_OSX__CoreUIImageView_h

#include "CoreObjects/CoreGameObject.h"
#include "Math/MathTypes.h"
#include "Graphics/GraphicsTypes.h"
#include "CoreUIView.h"
#include "CoreObjects/CoreObject_Factory.h"

class CoreUIImageView: public CoreUIView
{
public:
    virtual void UpdatePointers();	//Called when the memory location changes

	virtual bool Init(u32 type);	//Init variables
	virtual bool SpawnInit(void* pSpawnStruct, CoreObjectHandle hParent);	//Create object from data

	virtual void Uninit();	//Delete spawned objects here
	virtual void Update(f32 timeElapsed);	//Update the object
	virtual void ProcessMessage(u32 message, u32 parameter);	//Receive/Process messages
    
    static bool LoadResourcesForType(u32 type); //Load art/sounds
    static void InitClass();	//One-time global init
    
	virtual void LayoutView();

	s32 attrib_colorR;
	s32 attrib_colorG;
	s32 attrib_colorB;
	s32 attrib_imageType;
protected:
	
private:
	void UpdateDiffuseColor();
	void RefreshSettings();
    CoreObjectHandle m_hRenderable;
	vec4 m_diffuseColor;
};

DECLAREFACTORY(CoreUIImageView);

#endif
