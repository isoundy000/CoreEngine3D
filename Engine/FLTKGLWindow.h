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

extern Fl_Window* MAINWINDOW;

class FLTKGLWindow : public Fl_Gl_Window
{
	void draw();
	int handle(int);
	
public:
	FLTKGLWindow(int X, int Y, int W, int H, const char *L, f32 FPS, bool staticTimer);
	void SetCursorHiddenWhenOnScreen(bool isHidden);
private:
	
	enum FullscreenButtonState
	{
		FullscreenButtonState_WaitForKeys,
		FullscreenButtonState_WaitForKeysToBeReleased,
	};
	
	void UpdateMousePosition();
	void HideCursorIfOnScreen();
	bool m_cursorIsHiddenWhenOnScreen;
	s32 m_width;
	s32 m_height;
	s32 m_savedPosX;
	s32 m_savedPosY;
	bool m_isFullScreen;
	FullscreenButtonState m_fullScreenButtonState;
};

#endif
