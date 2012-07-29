//
//  CoreUIView.h
//  CoreEngine3D
//
//  Created by Jody McAdams on 4/21/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#ifndef Game_OSX__CoreUIView_h
#define Game_OSX__CoreUIView_h

#include "CoreObjects/CoreGameObject.h"
#include "MathTypes.h"
#include "GraphicsTypes.h"
#include "CoreObjects/CoreObjectAttribute.h"

#include <string>

#define CoreUIView_MAX_CHILDREN 32

enum CoreUI_ViewType
{
	CoreUI_ViewType_View,
	CoreUI_ViewType_ImageView,
	CoreUI_ViewType_Button,
};


class CoreUIView: public CoreGameObject
{
public:
	CoreObjectAttributeList attributes;
	
	/*struct Attributes
	{
		Attrib_CoreUIOrigin origin;
		Attrib_S32 offsetX;
		Attrib_S32 offsetY;
		Attrib_S32 width;
		Attrib_S32 height;
		Attrib_F32 opacity;
		Attrib_S32 sortValue;
	};*/
	
	void DEBUG_SetDebugVisible(bool isVisible);
	
	CoreUIView* GetChildViewByName(u32 nameSig);
    virtual void UpdateHandle();	//Called when the memory location changes

	virtual bool Init(u32 type);	//Init variables
	virtual bool SpawnInit(void* pSpawnStruct, CoreObjectHandle hParent);	//Create object from data
	virtual bool PostSpawnInit(void* pSpawnStruct);	//Link to objects in level
	virtual void Uninit();	//Delete spawned objects here
	virtual void Update(f32 timeElapsed);	//Update the object
	virtual void ProcessMessage(u32 message, u32 parameter);	//Receive/Process messages
    
    static bool LoadResourcesForType(u32 type); //Load art/sounds
    static void InitClass();	//One-time global init

	vec2 position;

	bool visible;

	CoreUI_ViewType viewType;
	
	CoreObjectHandle hParent;
	f32 fadeAlpha;	//Lets you fade out a view (separate from opacity)
	
	u32 numChildren;
	CoreObjectHandle children[CoreUIView_MAX_CHILDREN];
	
	virtual void LayoutView();
	//virtual void RefreshView();
	
	f32 parentOpacity;
	bool parentVisible;
	
	s32 attrib_name;
	s32 attrib_offsetX;
	s32 attrib_offsetY;
	s32 attrib_width;
	s32 attrib_height;
	s32 attrib_origin;
	s32 attrib_opacity;
	s32 attrib_sortValue;
protected:
	
	void LayoutSubViews();
	
	u8 m_attribData[1024];
	
private:
	bool m_debugVisible;
    //TODO: put member functions here
    //TODO: put member variables here
};

#endif
