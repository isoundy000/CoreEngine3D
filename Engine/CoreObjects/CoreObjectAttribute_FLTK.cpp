//
//  CoreObjectAttribute_FLTK.cpp
//  CoreEngine3D(OSX)
//
//  Created by Jody McAdams on 7/18/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#if defined(_DEBUG_PC)

#include "CoreObjectAttribute_FLTK.h"

Fl_Widget* CreateWidgetForAttribute(CoreObjectAttribute* pAttrib)
{
	switch(pAttrib->type)
	{
		case CoreObjectAttributeType_F32:
		{
			break;
		}
		case CoreObjectAttributeType_U32:
		{
			break;
		}
		case CoreObjectAttributeType_S32:
		{
			break;
		}
		case CoreObjectAttributeType_CoreUI_Origin:
		{
			break;
		}
		case CoreObjectAttributeType_Char32:
		{
			break;
		}
	}
}


#endif

