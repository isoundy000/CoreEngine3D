//
//  CoreObjectAttribute_FLTK.h
//  CoreEngine3D(OSX)
//
//  Created by Jody McAdams on 7/18/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#ifndef CoreEngine3D_OSX__CoreObjectAttribute_FLTK_h
#define CoreEngine3D_OSX__CoreObjectAttribute_FLTK_h

#if defined(_DEBUG_PC)

#include "CoreObjectAttribute.h"
#include <FL/Fl.H>
#include <FL/Fl_Widget.H>

Fl_Widget* CreateWidgetForAttribute(CoreObjectAttribute* pAttrib, s32 posX, s32 posY, s32 width, s32 height);

#endif

#endif
