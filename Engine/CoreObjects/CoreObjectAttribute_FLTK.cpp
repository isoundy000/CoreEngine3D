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

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Slider.H>
#include <stdio.h>

// sliderinput -- simple example of tying an fltk slider and input widget together
// 1.00 erco 10/17/04

class Slider_Int_Input : public Fl_Group {
    Fl_Int_Input *input;
    Fl_Slider    *slider;
	
    // CALLBACK HANDLERS
    //    These 'attach' the input and slider's values together.
    //
    void Slider_CB2() {
        static int recurse = 0;
        if ( recurse ) { 
            return;
        } else {
            recurse = 1;
            char s[80];
            sprintf(s, "%d", (int)(slider->value() + .5));
            // fprintf(stderr, "SPRINTF(%d) -> '%s'\n", (int)(slider->value()+.5), s);
            input->value(s);          // pass slider's value to input
            recurse = 0;
        }
    }
	
    static void Slider_CB(Fl_Widget *w, void *data) {
        ((Slider_Int_Input*)data)->Slider_CB2();
    }
	
    void Input_CB2() {
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
        }
    }
    static void Input_CB(Fl_Widget *w, void *data) {
        ((Slider_Int_Input*)data)->Input_CB2();
    }
	
public:
    // CTOR
    Slider_Int_Input(int x, int y, int w, int h, const char *l=0) : Fl_Group(x,y,w,h,l) {
        int in_w = 40;
        input  = new Fl_Int_Input(x, y, in_w, h);
        input->callback(Input_CB, (void*)this);
        input->when(FL_WHEN_ENTER_KEY|FL_WHEN_NOT_CHANGED);
		
        slider = new Fl_Slider(x+in_w, y, w-in_w, h);
        slider->type(1);
        slider->callback(Slider_CB, (void*)this);
		
        bounds(1, 10);     // some usable default
        value(5);          // some usable default
        end();             // close the group
    }
	
    // MINIMAL ACCESSORS --  Add your own as needed
    int  value() const    { return((int)(slider->value() + 0.5)); }
    void value(int val)   { slider->value(val); Slider_CB2(); }
    void minumum(int val) { slider->minimum(val); }
    int  minumum() const  { return((int)slider->minimum()); }
    void maximum(int val) { slider->maximum(val); }
    int  maximum() const  { return((int)slider->maximum()); }
    void bounds(int low, int high) { slider->bounds(low, high); }
};


Fl_Widget* CreateWidgetForAttribute(CoreObjectAttribute* pAttrib, s32 posX, s32 posY, s32 width, s32 height)
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
			break;
		}
		case CoreObjectAttributeType_U32:
		{
			CoreObjectAttribute_U32* pCurrAttrib = (CoreObjectAttribute_U32*)pAttrib;
			
			Slider_Int_Input *si = new Slider_Int_Input(posX,posY,width,height,(const char*)pAttrib->name);
			si->bounds(pCurrAttrib->minValue,pCurrAttrib->maxValue);       // set min/max for slider
			si->value(pCurrAttrib->value);           // set initial value
			
			return si;
			
			break;
		}
		case CoreObjectAttributeType_S32:
		{
			CoreObjectAttribute_S32* pCurrAttrib = (CoreObjectAttribute_S32*)pAttrib;
			
			Slider_Int_Input *si = new Slider_Int_Input(posX,posY,width,height,(const char*)pAttrib->name);
			si->bounds(pCurrAttrib->minValue,pCurrAttrib->maxValue);       // set min/max for slider
			si->value(pCurrAttrib->value);           // set initial value
			
			return si;
			
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
	
	return NULL;
}


#endif

