//
//  CoreUI_Button.h
//  CoreEngine3D
//
//  Created by JodyMcAdams on 12/19/11.
//  Copyright (c) 2011 Jody McAdams. All rights reserved.
//

#ifndef CoreEngine3D_CoreUI_Button_h
#define CoreEngine3D_CoreUI_Button_h

#include "GraphicsTypes.h"
#include "OpenGLRenderer.h"
#include "CoreInput_DeviceInputState.h"

enum CoreUI_ButtonState
{
	CoreUI_ButtonState_Neutral,
	CoreUI_ButtonState_Pressed,
	CoreUI_ButtonState_Num,
};

enum CoreUI_Origin
{
	CoreUI_Origin_None,
	CoreUI_Origin_Center,
	CoreUI_Origin_Left,
	CoreUI_Origin_Right,
	CoreUI_Origin_Top,
	CoreUI_Origin_Bottom,
	CoreUI_Origin_TopLeft,
	CoreUI_Origin_BottomLeft,
	CoreUI_Origin_TopRight,
	CoreUI_Origin_BottomRight,
	CoreUI_Origin_Num,
};

struct CoreUI_Button
{
public:
	void Init(u32 width, u32 height, CoreUI_Origin Origin, s32 offsetX, s32 offsetY, u32* textureHandle, s32 value, void (*callback)(s32));
	void PressButton(TouchState touchState, vec2* pBeginTouchPos, vec2* pCurrTouchPo);
	
	void Show();
	void Hide();
	void SetActive(bool isActive);
	
	u32 m_width;
	u32 m_height;
	
	u32 m_halfWidth;
	u32 m_halfHeight;
	
	vec3 m_position;
	vec4 m_diffuseColor;

	CoreUI_ButtonState m_buttonState;
	CoreUI_Origin m_Origin;
	
	CoreObjectHandle m_handle_r3D_button;
	
	s32 m_value;
	
	void (*m_callback)(s32);
	
private:
	bool m_isActive;
};

#endif
