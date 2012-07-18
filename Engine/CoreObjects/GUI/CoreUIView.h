//
//  CoreUIView.h
//  CoreEngine3D
//
//  Created by Jody McAdams on 4/21/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#ifndef Game_OSX__CoreUIView_h
#define Game_OSX__CoreUIView_h

#include "../CoreGameObject.h"
#include "../MathTypes.h"
#include "../GraphicsTypes.h"

#include <string>

#define CoreUIView_MAX_CHILDREN 32

enum CoreUI_ViewType
{
	CoreUI_ViewType_View,
	CoreUI_ViewType_ImageView,
	CoreUI_ViewType_Button,
};

enum CoreUI_Origin
{
	CoreUI_Origin_Center,
	CoreUI_Origin_Left,
	CoreUI_Origin_Right,
	CoreUI_Origin_Top,
	CoreUI_Origin_Bottom,
	CoreUI_Origin_TopLeft,
	CoreUI_Origin_BottomLeft,
	CoreUI_Origin_TopRight,
	CoreUI_Origin_BottomRight,
	CoreUI_Origin_Num,
};

class CoreUIView: public CoreGameObject
{
public:
	struct Properties
	{
		CoreUI_Origin origin;
		s32 offsetX;
		s32 offsetY;
		s32 width;
		s32 height;
		f32 opacity;
		s32 sortValue;
	};
	
#if defined(_DEBUG_PC)
	std::string nameString;
#endif
	Properties* GetProperties();
	CoreUIView* GetChildViewByName(u32 nameSig);
    virtual void UpdateHandle();	//Called when the memory location changes

	virtual bool Init(u32 type);	//Init variables
	virtual bool SpawnInit(void* pSpawnStruct);	//Create object from data
	virtual bool PostSpawnInit(void* pSpawnStruct);	//Link to objects in level
	virtual void Uninit();	//Delete spawned objects here
	virtual void Update(f32 timeElapsed);	//Update the object
	virtual void ProcessMessage(u32 message, u32 parameter);	//Receive/Process messages
    
    static bool LoadResourcesForType(u32 type); //Load art/sounds
    static void InitClass();	//One-time global init

	u32 nameSig;

	vec2 position;

	bool visible;

	CoreUI_ViewType viewType;
	
	f32 opacity;

	u32 numChildren;
	CoreObjectHandle children[CoreUIView_MAX_CHILDREN];
	
	virtual void LayoutView(const CoreUIView* pParentView);
	virtual void RefreshView();
protected:
	f32 parentOpacity;
	bool parentVisible;
	
	CoreUIView::Properties m_properties;
	
	void LayoutSubViews();
private:
    //TODO: put member functions here
    //TODO: put member variables here
	
	
};

#endif
