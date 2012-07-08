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

#include "MathTypes.h"

class FLTKGLWindow : public Fl_Gl_Window
{
	void draw();
	int handle(int);
	
public:
	FLTKGLWindow(int X, int Y, int W, int H, const char *L, f32 FPS, bool staticTimer);
private:
	void UpdateMousePosition();
};

#endif
