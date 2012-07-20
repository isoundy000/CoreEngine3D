//
//  CoreObjectAttribute_FLTK.cpp
//  CoreEngine3D(OSX)
//
//  Created by Jody McAdams on 7/18/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#if defined(_DEBUG_PC)

#include "CoreObjectAttribute_FLTK.h"

#include <FL/Fl_Text_Display.H>

Fl_Widget* CreateWidgetForAttribute(CoreObjectAttribute* pAttrib, s32 posY, s32 width)
{
	//Fl_Group* pGroup = new Fl_Group(0,posY,width,64,0);
	Fl_Text_Display* pNameText = new Fl_Text_Display(0,posY,width,32,NULL);
	Fl_Text_Buffer* pNameBuffer = new Fl_Text_Buffer();
	pNameBuffer->text((const char*)pAttrib->name);
	pNameText->buffer(pNameBuffer);
	
	return pNameText;
	
	//pGroup->add(pNameText);
	
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
			/*CoreObjectAttribute_Char32* pCurrAttrib = (CoreObjectAttribute_Char32*)pAttrib;
			Fl_Text_Display* pTextDisplay = new Fl_Text_Display(0,32,width,32,0);
			Fl_Text_Buffer* pBuffer = new Fl_Text_Buffer();
			pBuffer->text((const char*)pCurrAttrib->value);
			pTextDisplay->buffer(pBuffer);
			
			pGroup->add(pTextDisplay);*/
			
			break;
		}
		default:
		{
			break;
		}
	}
	
	//return pGroup;
}


#endif

