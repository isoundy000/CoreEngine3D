//
//  CoreObjectAttribute_FLTK.cpp
//  CoreEngine3D
//
//  Created by Jody McAdams on 7/18/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#if defined(_DEBUG_PC)

#include "CoreObjectAttribute_FLTK.h"

#include <FL/Fl_Text_Display.H>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Choice.H>
#include <cstdio>

#include "GUI/CoreUIView.h"
#include "GUI/CoreUIImageView.h"
#include "GUI/CoreUIButton.h"

CoreObjectAttributeWidgetLink::CoreObjectAttributeWidgetLink()
{
	viewHandle = 0;
	attributeIndex = 0;
}

void CoreObjectAttributeWidgetLink::LinkAttribute(CoreObjectHandle viewHandle, s32 attributeIndex)
{
	this->viewHandle = viewHandle;
	this->attributeIndex = attributeIndex;
}

// sliderinput -- simple example of tying an fltk slider and input widget together
// 1.00 erco 10/17/04

//Int slider


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Slider_Int_Input::UpdateAttribute()
{
	CoreUIView* pView = (CoreUIView*)COREOBJECTMANAGER->GetObjectByHandle(widgetLink.viewHandle);
	if(pView != NULL)
	{
		CoreObjectAttribute* pAttrib = pView->attributes[widgetLink.attributeIndex];
		
		if(pAttrib->type == CoreObjectAttributeType_U32)
		{
			CoreObjectAttribute_U32* pAttribU32 = (CoreObjectAttribute_U32*)pAttrib;
			pAttribU32->value = slider->value();
		}
		//Must by S32
		else
		{
			CoreObjectAttribute_S32* pAttribS32 = (CoreObjectAttribute_S32*)pAttrib;
			pAttribS32->value = slider->value();
		}
		
		switch (pView->viewType)
		{
			case CoreUI_ViewType_View:
			{
				pView->LayoutView();
				
				break;
			}
			case CoreUI_ViewType_ImageView:
			{
				CoreUIImageView* pImageView = (CoreUIImageView*)pView;
				pImageView->LayoutView();
				
				break;
			}
			case CoreUI_ViewType_Button:
			{
				CoreUIImageView* pButtonView = (CoreUIImageView*)pView;
				pButtonView->LayoutView();
				
				break;
			}
			default:
			{
				break;
			}
		}
	}
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Slider_Int_Input::Slider_CB2()
{
	static int recurse = 0;
	
	if ( recurse )
	{ 
		return;
	}
	else
	{
		recurse = 1;
		char s[80];
		sprintf(s, "%d", (int)(slider->value() + .5));
		// fprintf(stderr, "SPRINTF(%d) -> '%s'\n", (int)(slider->value()+.5), s);
		input->value(s);          // pass slider's value to input
		recurse = 0;
		
		UpdateAttribute();
		
		//COREDEBUG_PrintMessage("Changed value!");
	}
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Slider_Int_Input::Slider_CB(Fl_Widget *w, void *data)
{
	((Slider_Int_Input*)data)->Slider_CB2();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Slider_Int_Input::Input_CB2()
{
	static int recurse = 0;
	if ( recurse ) {
		return;
	} else {
		recurse = 1;
		int val = 0;
		if ( sscanf(input->value(), "%d", &val) != 1 ) {
			val = 0;
		}
		// fprintf(stderr, "SCANF('%s') -> %d\n", input->value(), val);
		slider->value(val);         // pass input's value to slider
		recurse = 0;
		
		UpdateAttribute();
	}
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Slider_Int_Input::Input_CB(Fl_Widget *w, void *data)
{
	((Slider_Int_Input*)data)->Input_CB2();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
Slider_Int_Input::Slider_Int_Input(int x, int y, int w, int h, const char *l) : Fl_Group(x,y,w,h,l)
{
	int in_w = 40;
	input  = new Fl_Int_Input(x, y, in_w, h);
	input->callback(Input_CB, (void*)this);
	input->when(FL_WHEN_ENTER_KEY|FL_WHEN_NOT_CHANGED);
	
	slider = new Fl_Slider(x+in_w, y, w-in_w, h);
	slider->type(1);
	slider->callback(Slider_CB, (void*)this);
	slider->step(1);
	
	bounds(1, 10);     // some usable default
	value(5);          // some usable default
	end();             // close the group
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int  Slider_Int_Input::value() const
{ 
	return ((int)(slider->value() + 0.5));
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Slider_Int_Input::value(int val)
{
	slider->value(val);
	Slider_CB2();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Slider_Int_Input::minumum(int val)
{
	slider->minimum(val);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int  Slider_Int_Input::minumum() const
{
	return((int)slider->minimum());
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Slider_Int_Input::maximum(int val)
{
	slider->maximum(val);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int  Slider_Int_Input::maximum() const
{
	return((int)slider->maximum());
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Slider_Int_Input::bounds(int low, int high)
{
	slider->bounds(low, high);
}


//Float Slider

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Slider_Float_Input::UpdateAttribute()
{
	CoreUIView* pView = (CoreUIView*)COREOBJECTMANAGER->GetObjectByHandle(widgetLink.viewHandle);
	if(pView != NULL)
	{
		CoreObjectAttribute* pAttrib = pView->attributes[widgetLink.attributeIndex];
		
		if(pAttrib->type == CoreObjectAttributeType_F32)
		{
			CoreObjectAttribute_F32* pAttribF32 = (CoreObjectAttribute_F32*)pAttrib;
			pAttribF32->value = slider->value();
		}
		
		switch (pView->viewType)
		{
			case CoreUI_ViewType_View:
			{
				pView->LayoutView();
				
				break;
			}
			case CoreUI_ViewType_ImageView:
			{
				CoreUIImageView* pImageView = (CoreUIImageView*)pView;
				pImageView->LayoutView();
				
				break;
			}
			case CoreUI_ViewType_Button:
			{
				CoreUIImageView* pButtonView = (CoreUIImageView*)pView;
				pButtonView->LayoutView();
				
				break;
			}
			default:
			{
				break;
			}
		}
	}
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Slider_Float_Input::Slider_CB2()
{
	static int recurse = 0;
	
	if ( recurse )
	{ 
		return;
	}
	else
	{
		recurse = 1;
		char s[80];
		sprintf(s, "%.2f", slider->value());
		// fprintf(stderr, "SPRINTF(%d) -> '%s'\n", (int)(slider->value()+.5), s);
		input->value(s);          // pass slider's value to input
		recurse = 0;
		
		UpdateAttribute();
		
		//COREDEBUG_PrintMessage("Changed value!");
	}
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Slider_Float_Input::Slider_CB(Fl_Widget *w, void *data)
{
	((Slider_Float_Input*)data)->Slider_CB2();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Slider_Float_Input::Input_CB2()
{
	static int recurse = 0;
	if ( recurse ) {
		return;
	} else {
		recurse = 1;
		int val = 0;
		if ( scanf(input->value(), "%.2f", &val) != 1 ) {
			val = 0;
		}
		// fprintf(stderr, "SCANF('%s') -> %d\n", input->value(), val);
		slider->value(val);         // pass input's value to slider
		recurse = 0;
		
		UpdateAttribute();
	}
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Slider_Float_Input::Input_CB(Fl_Widget *w, void *data)
{
	((Slider_Float_Input*)data)->Input_CB2();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
Slider_Float_Input::Slider_Float_Input(int x, int y, int w, int h, const char *l) : Fl_Group(x,y,w,h,l)
{
	int in_w = 40;
	input  = new Fl_Float_Input(x, y, in_w, h);
	input->callback(Input_CB, (void*)this);
	input->when(FL_WHEN_ENTER_KEY|FL_WHEN_NOT_CHANGED);
	
	slider = new Fl_Slider(x+in_w, y, w-in_w, h);
	slider->type(1);
	slider->callback(Slider_CB, (void*)this);
	slider->step(0.01);
	
	bounds(1, 10);     // some usable default
	value(5);          // some usable default
	end();             // close the group
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float  Slider_Float_Input::value() const
{ 
	return slider->value();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Slider_Float_Input::value(float val)
{
	slider->value(val);
	Slider_CB2();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Slider_Float_Input::minumum(float val)
{
	slider->minimum(val);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float  Slider_Float_Input::minumum() const
{
	return(slider->minimum());
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Slider_Float_Input::maximum(float val)
{
	slider->maximum(val);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float  Slider_Float_Input::maximum() const
{
	return slider->maximum();
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Slider_Float_Input::bounds(float low, float high)
{
	slider->bounds(low, high);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
Fl_Widget* CreateWidgetForAttribute(CoreObjectAttribute* pAttrib, s32 posX, s32 posY, s32 width, s32 height, CoreObjectHandle viewHandle, s32 attribIndex)
{
	//Fl_Group* pGroup = new Fl_Group(0,posY,width,64,0);
	/*Fl_Text_Display* pNameText = new Fl_Text_Display(0,posY,width,32,NULL);
	Fl_Text_Buffer* pNameBuffer = new Fl_Text_Buffer();
	pNameBuffer->text((const char*)pAttrib->name);
	pNameText->buffer(pNameBuffer);
	
	return pNameText;
	
	//pGroup->add(pNameText);*/
	
	switch(pAttrib->type)
	{
		case CoreObjectAttributeType_F32:
		{
			CoreObjectAttribute_F32* pCurrAttrib = (CoreObjectAttribute_F32*)pAttrib;
			
			Slider_Float_Input *si = new Slider_Float_Input(posX,posY,width,height,(const char*)pAttrib->name);
			si->bounds(pCurrAttrib->minValue,pCurrAttrib->maxValue);       // set min/max for slider
			si->value(pCurrAttrib->value);           // set initial value
			
			si->widgetLink.LinkAttribute(viewHandle,attribIndex);
			
			return si;
		}
		case CoreObjectAttributeType_U32:
		{
			CoreObjectAttribute_U32* pCurrAttrib = (CoreObjectAttribute_U32*)pAttrib;
			
			Slider_Int_Input *si = new Slider_Int_Input(posX,posY,width,height,(const char*)pAttrib->name);
			si->bounds(pCurrAttrib->minValue,pCurrAttrib->maxValue);       // set min/max for slider
			si->value(pCurrAttrib->value);           // set initial value
			
			si->widgetLink.LinkAttribute(viewHandle,attribIndex);
			
			return si;
		}
		case CoreObjectAttributeType_S32:
		{
			CoreObjectAttribute_S32* pCurrAttrib = (CoreObjectAttribute_S32*)pAttrib;
			
			Slider_Int_Input *si = new Slider_Int_Input(posX,posY,width,height,(const char*)pAttrib->name);
			si->bounds(pCurrAttrib->minValue,pCurrAttrib->maxValue);       // set min/max for slider
			si->value(pCurrAttrib->value);           // set initial value
			
			si->widgetLink.LinkAttribute(viewHandle,attribIndex);
			
			return si;
		}
		case CoreObjectAttributeType_CoreUI_Origin:
		{
			//CoreObjectAttribute_CoreUI_Origin* pOriginAttrib = (CoreObjectAttribute_CoreUI_Origin*)pAttrib;
			
			Fl_Choice* pChoice = new Fl_Choice(posX, posY, width, height,(const char*)pAttrib->name);
			pChoice->add("hi");
			pChoice->add("bye");
			pChoice->add("die");
			
			return pChoice;
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
	
	return NULL;
}

#endif

