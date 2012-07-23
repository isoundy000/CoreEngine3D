//
//  CoreUIImageView.h
//  CoreEngine3D
//
//  Created by Jody McAdams on 4/21/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#ifndef Game_OSX__CoreUIImageView_h
#define Game_OSX__CoreUIImageView_h

#include "../CoreGameObject.h"
#include "../MathTypes.h"
#include "../GraphicsTypes.h"
#include "CoreUIView.h"

class CoreUIImageView: public CoreUIView
{
public:
    virtual void UpdateHandle();	//Called when the memory location changes

	virtual bool Init(u32 type);	//Init variables
	virtual bool SpawnInit(void* pSpawnStruct);	//Create object from data
	virtual bool PostSpawnInit(void* pSpawnStruct);	//Link to objects in level
	virtual void Uninit();	//Delete spawned objects here
	virtual void Update(f32 timeElapsed);	//Update the object
	virtual void ProcessMessage(u32 message, u32 parameter);	//Receive/Process messages
    
    static bool LoadResourcesForType(u32 type); //Load art/sounds
    static void InitClass();	//One-time global init
    
	virtual void LayoutView(const CoreUIView* pParentView);
	virtual void RefreshView();
	
	s32 attrib_colorR;
	s32 attrib_colorG;
	s32 attrib_colorB;
protected:
	
private:
	void UpdateDiffuseColor();
	void RefreshSettings();
    CoreObjectHandle m_hRenderable;
	vec4 m_diffuseColor;
};

#endif
