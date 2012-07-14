/*
 *  FLTKGLWindow.cpp
 *  CoreEngine3D
 *
 *  Created by Jody McAdams on 10/9/10.
 *  Copyright 2010 GameJelly Online. All rights reserved.
 *
 */

#include "FLTKGLWindow.h"
#include "OpenGLRenderer.h"
#include "Game.h"
#include "CoreInput_PCInputState.h"

#if defined(PLATFORM_OSX) || defined(PLATFORM_LIN)
#include <sys/time.h>

Fl_Window* MAINWINDOW = NULL;

timeval g_lastTimeValue;

#elif defined(PLATFORM_WIN)
DWORD g_lastTimeValue = 0;
#endif

f32 g_OneOverFPS = 1.0f/60.0f;
bool g_StaticTimer = true;

static void Timer_CB(void *userdata) {
	
	FLTKGLWindow *mygl = (FLTKGLWindow*)userdata;
	
	mygl->redraw();
	
	Fl::repeat_timeout(g_OneOverFPS, Timer_CB, userdata);
	
}

void FLTKGLWindow::SetCursorHiddenWhenOnScreen(bool isHidden)
{
	m_cursorIsHiddenWhenOnScreen = isHidden;
}

FLTKGLWindow::FLTKGLWindow(int X, int Y, int W, int H, const char *L, f32 FPS, bool staticTimer)
: Fl_Gl_Window(X, Y, W, H, L)
{
	MAINWINDOW = this;
	
	m_savedPosX = 128;
	m_savedPosY = 128;
	
	m_fullScreenButtonState = FullscreenButtonState_WaitForKeys;
	
	m_width = W;
	m_height = H;
	
	g_OneOverFPS = 1.0f/FPS;
	g_StaticTimer = staticTimer;
	
#if defined(PLATFORM_OSX) || defined(PLATFORM_LIN)
	gettimeofday(&g_lastTimeValue, NULL);
#elif defined(PLATFORM_WIN)
	//NOTE: won't compile yet
	const DWORD time = timeGetTime();
	g_lastTimeValue = time;
#endif
	
	Fl::add_timeout(g_OneOverFPS, Timer_CB, (void*)this);
	
	m_isFullScreen = false;
}

//Draw
void FLTKGLWindow::draw()
{
	if(!valid())
	{
		//Initialize here
		
	}
	
	if(GAME != NULL)
	{
		CoreInput_ButtonState commandKeyState = GAME->m_keyboardState.buttonState[232];
		CoreInput_ButtonState FKeyState = GAME->m_keyboardState.buttonState[102];
		
		switch(m_fullScreenButtonState)
		{
			case FullscreenButtonState_WaitForKeys:
			{
				if(commandKeyState == CoreInput_ButtonState_Held
				   && FKeyState == CoreInput_ButtonState_Began)
				{
					m_fullScreenButtonState = FullscreenButtonState_WaitForKeysToBeReleased;

					m_isFullScreen = !m_isFullScreen;
					
					if(m_isFullScreen == true)
					{
						fullscreen();
						
						const f32 aspect = (f32)m_width/(f32)m_height;
						
						
						const s32 height = h();
						const s32 width = (f32)height*(f32)aspect;
						
						s32 posX = (w()-width)/2;
						
						GLRENDERER->SetViewport(posX,0,width, height);
					}
					else
					{
						fullscreen_off(m_savedPosX, m_savedPosY, m_width, m_height);
						
						GLRENDERER->SetViewport(0,0,m_width, m_height);
					}
				}

				break;
			}
			case FullscreenButtonState_WaitForKeysToBeReleased:
			{
				if(FKeyState == CoreInput_ButtonState_Ended)
				{
					m_fullScreenButtonState = FullscreenButtonState_WaitForKeys;
				}
				
				break;
			}
		}
		
		f32 timeElapsed;
		
		if(g_StaticTimer)
		{
			timeElapsed = g_OneOverFPS;
		}
		else
		{
#if defined(PLATFORM_OSX) || defined(PLATFORM_LIN)
			timeval time;
			gettimeofday(&time, NULL);
			timeElapsed = ((time.tv_sec - g_lastTimeValue.tv_sec)*1000000.0f + (time.tv_usec - g_lastTimeValue.tv_usec))/1000000.0f;
			g_lastTimeValue = time;
#elif defined(PLATFORM_WIN)
			//NOTE: won't compile yet
			const DWORD time = timeGetTime();
			const DWORD timeDiff = time-g_lastTimeValue;
			timeElapsed = (f32)timeDiff / 100000.0f;
			g_lastTimeValue = time;
#else
			timeElapsed = g_OneOverFPS;
#endif	
		}
		
		UpdateMousePosition();
	
		const bool mouseIsStuck = GAME->m_mouseState.position.x == GAME->m_mouseState.lastPosition.x 
		&& GAME->m_mouseState.position.y == GAME->m_mouseState.lastPosition.y;
		
		if(GAME->m_mouseState.sleeping == false
		   && mouseIsStuck == true)
		{
			//COREDEBUG_PrintDebugMessage("Mouse fell asleep...");
			GAME->m_mouseState.sleeping = true;
		}
		else if(GAME->m_mouseState.sleeping == true
				&& mouseIsStuck == false)
		{
			//COREDEBUG_PrintDebugMessage("Mouse woke up!");
			GAME->m_mouseState.sleeping = false;
		}
		
		GAME->Update(timeElapsed);
		
		//It's valid now so draw things
		if(GLRENDERER != NULL)
		{
			GLRENDERER->Render(timeElapsed);
		}
	}
	
}


void FLTKGLWindow::UpdateMousePosition()
{
	Fl::get_mouse(GAME->m_mouseState.position.x, GAME->m_mouseState.position.y);
	
	GAME->m_mouseState.position.x -= x();
	GAME->m_mouseState.position.y -= y();
	
	const int mousePosX = GAME->m_mouseState.position.x;
	const int mousePosY = GAME->m_mouseState.position.y;
	
	//COREDEBUG_PrintDebugMessage("Mouse x: %d, y: %d",mousePosX,mousePosY);
	
	if(m_cursorIsHiddenWhenOnScreen)
	{
		if(m_isFullScreen)
		{
			if(mousePosY < 0
			   )
			{
				cursor(FL_CURSOR_DEFAULT);
			}
			else
			{
				cursor(FL_CURSOR_NONE);
			}
		}
		else
		{
			if(mousePosX < 0
			   || GAME->m_mouseState.position.x > m_width
			   || GAME->m_mouseState.position.y < 0
			   || GAME->m_mouseState.position.y > m_height
			   )
			{
				cursor(FL_CURSOR_DEFAULT);
			}
			else
			{
				cursor(FL_CURSOR_NONE);
			}
		}
	}
}

//NOTE:
//See this page for keycodes
//http://www.pyglet.org/doc/api/pyglet.window.key-module.html
//I subtract 65279 from the huge numbered ones to make life easy
static const u32 g_Key_StartHigh = 65279;

int FLTKGLWindow::handle(int event) {
	switch(event) {
		case FL_MOVE:
		{
			//COREDEBUG_PrintDebugMessage("FLTK Event: MOVE");

			return 1;
		}
		case FL_PUSH:
		{
			//COREDEBUG_PrintDebugMessage("FLTK Event: PUSH");
			//... mouse down event ...
			//... position in Fl::event_x() and Fl::event_y()

			if(Fl::event_button() == FL_LEFT_MOUSE)
			{
				GAME->m_mouseState.buttonState[0] = CoreInput_ButtonState_Began;
			}
			else if(Fl::event_button() == FL_RIGHT_MOUSE)
			{
				GAME->m_mouseState.buttonState[1] = CoreInput_ButtonState_Began;
			}
			
			
			return 1;
		}
		case FL_DRAG:
		{
			//COREDEBUG_PrintDebugMessage("FLTK Event: DRAG");
			//... mouse moved while down event ...

			return 1;
		}
		case FL_RELEASE: 
		{
			//COREDEBUG_PrintDebugMessage("FLTK Event: RELEASE");
			//... mouse up event ...
			//... Return 1 if you want keyboard events, 0 otherwise
			if(Fl::event_button() == FL_LEFT_MOUSE)
			{
				GAME->m_mouseState.buttonState[0] = CoreInput_ButtonState_Ended;
			}
			else if(Fl::event_button() == FL_RIGHT_MOUSE)
			{
				GAME->m_mouseState.buttonState[1] = CoreInput_ButtonState_Ended;
			}
			
			return 1;
		}
		case FL_FOCUS :
		{
			//COREDEBUG_PrintDebugMessage("FLTK Event: FOCUS");
			//... Return 1 if you want keyboard events, 0 otherwise
			return 1;
		}
		case FL_UNFOCUS :
		{
			//COREDEBUG_PrintDebugMessage("FLTK Event: UNFOCUS");
			//... Return 1 if you want keyboard events, 0 otherwise
			return 1;
		}
		case FL_KEYDOWN:
		{
			//... keypress, key is in Fl::event_key(), ascii in Fl::event_text()
			//... Return 1 if you understand/use the keyboard event, 0 otherwise...
			//GAME->m_keyboardState.buttonState[Fl::event_key()] = CoreInput_ButtonState_Began;
			
			u32 keyCode = Fl::event_key();
			if(keyCode >= g_Key_StartHigh)
			{
				keyCode -= g_Key_StartHigh;
			}
			
			COREDEBUG_PrintDebugMessage("FLTK Event: FL_KEYDOWN: %d",keyCode);
			
			const CoreInput_ButtonState currState = GAME->m_keyboardState.buttonState[keyCode];
			if(currState != CoreInput_ButtonState_Held)
			{
				GAME->m_keyboardState.buttonState[keyCode] = CoreInput_ButtonState_Began;
			}
			
			//If this is the escape key... KILL ALL THE THINGS
			if(keyCode == 28)
			{
				GAME->CleanUp();
				GLRENDERER->CleanUp();
				
				exit(EXIT_SUCCESS);
			}
			
			return 1;
		}
		case FL_KEYUP:
		{
			u32 keyCode = Fl::event_key();
			if(keyCode >= g_Key_StartHigh)
			{
				keyCode -= g_Key_StartHigh;
			}
			
			//COREDEBUG_PrintDebugMessage("FLTK Event: FL_KEYUP: %d",keyCode);
			
			const CoreInput_ButtonState currState = GAME->m_keyboardState.buttonState[keyCode];
			if(currState != CoreInput_ButtonState_None)
			{
				if(currState == CoreInput_ButtonState_Began)
				{
					GAME->m_keyboardState.skippedBegan[keyCode] = 1;
				}
				else
				{
					GAME->m_keyboardState.buttonState[keyCode] = CoreInput_ButtonState_Ended;
				}
			}
			
			return 1;
		}
		case FL_SHORTCUT:
		{
			//... shortcut, key is in Fl::event_key(), ascii in Fl::event_text()
			//... Return 1 if you understand/use the shortcut event, 0 otherwise...
			return 1;
		}
		default:
		{
			// pass other events to the base class...
			return Fl_Gl_Window::handle(event);
		}
	}
}
