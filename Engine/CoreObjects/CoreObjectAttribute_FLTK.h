//
//  CoreObjectAttribute_FLTK.h
//  CoreEngine3D
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
#include <Fl/Fl_Int_Input.H>
#include <Fl/Fl_Group.H>
#include <Fl/Fl_Slider.H>
#include <Fl/Fl_Float_Input.H>
#include "CoreObject_Manager.h"

class CoreObjectAttributeWidgetLink
{
public:
	CoreObjectAttributeWidgetLink();
	void LinkAttribute(CoreObjectHandle viewHandle, s32 attributeIndex);

	CoreObjectHandle viewHandle;
	s32 attributeIndex;
};


//SLIDER INPUT
class Slider_Int_Input : public Fl_Group
{
private:
	Fl_Int_Input *input;
    Fl_Slider    *slider;
	
    // CALLBACK HANDLERS
    //    These 'attach' the input and slider's values together.
    //
    void Slider_CB2();	
    static void Slider_CB(Fl_Widget *w, void *data);
    void Input_CB2();
    static void Input_CB(Fl_Widget *w, void *data);
	void UpdateAttribute();
public:
    // CTOR
    Slider_Int_Input(int x, int y, int w, int h, const char *l=0);
	CoreObjectAttributeWidgetLink widgetLink;
    // MINIMAL ACCESSORS --  Add your own as needed
    int  value() const;
    void value(int val);
    void minumum(int val);
    int  minumum() const;
    void maximum(int val);
    int  maximum() const;
    void bounds(int low, int high);
};

class Slider_Float_Input : public Fl_Group
{
private:
	Fl_Float_Input *input;
    Fl_Slider    *slider;
	
    // CALLBACK HANDLERS
    //    These 'attach' the input and slider's values together.
    //
    void Slider_CB2();	
    static void Slider_CB(Fl_Widget *w, void *data);
    void Input_CB2();
    static void Input_CB(Fl_Widget *w, void *data);
	void UpdateAttribute();
public:
    // CTOR
    Slider_Float_Input(int x, int y, int w, int h, const char *l=0);
	CoreObjectAttributeWidgetLink widgetLink;
    // MINIMAL ACCESSORS --  Add your own as needed
    float  value() const;
    void value(float val);
    void minumum(float val);
    float  minumum() const;
    void maximum(float val);
    float  maximum() const;
    void bounds(float low, float high);
};

Fl_Widget* CreateWidgetForAttribute(CoreObjectAttribute* pAttrib, s32 posX, s32 posY, s32 width, s32 height, CoreObjectHandle viewHandle, s32 attribIndex);

#endif

#endif
