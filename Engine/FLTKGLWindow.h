#ifndef __FLTKGLWINDOW_H
#define __FLTKGLWINDOW_H

/*
 *  FLTKGLWindow.h
 *  CoreEngine3D
 *
 *  Created by Jody McAdams on 10/9/10.
 *  Copyright 2010 GameJelly Online. All rights reserved.
 *
 */

#if defined (PLATFORM_OSX)
#include <OpenGL/gl.h>
#endif

#if defined (PLATFORM_WIN)
#include "glew/glew.h"
#endif
#include <FL/Fl_Gl_Window.H>
#include <FL/Fl.H>

const unsigned int FPS = 60;
const float ONE_OVER_FPS = 1.0f/60.0f;

class FLTKGLWindow : public Fl_Gl_Window {
	static void Timer_CB(void *userdata) {
		
        FLTKGLWindow *mygl = (FLTKGLWindow*)userdata;
		
       
        mygl->redraw();
		
        Fl::repeat_timeout(ONE_OVER_FPS, Timer_CB, userdata);
		
    }

	void draw();
	int handle(int);
	
public:
	FLTKGLWindow(int X, int Y, int W, int H, const char *L)
    : Fl_Gl_Window(X, Y, W, H, L)
	{
		Fl::add_timeout(ONE_OVER_FPS, Timer_CB, (void*)this);  
	}
private:
	void UpdateMousePosition();
};

#endif
