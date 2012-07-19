//
//  CoreObjectProperties.h
//  CoreEngine3D(OSX)
//
//  Created by Jody McAdams on 7/18/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#ifndef CoreEngine3D_OSX__CoreObjectProperties_h
#define CoreEngine3D_OSX__CoreObjectProperties_h

#include <string>
#include <sstream>

#include "../MathTypes.h"

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

template <class T>
class CoreObjectAttribute
{
public:
	u32 GetValueSize(){return sizeof(value);}
	
	std::string name;
	T value;
};

typedef CoreObjectAttribute<f32> Attrib_F32;
typedef CoreObjectAttribute<s32> Attrib_S32;
typedef CoreObjectAttribute<u32> Attrib_U32;
typedef CoreObjectAttribute<CoreUI_Origin> Attrib_CoreUIOrigin;

#endif
