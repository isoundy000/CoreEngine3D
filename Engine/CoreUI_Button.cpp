//
//  CoreUI_Button.cpp
//  CoreEngine3D
//
//  Created by JodyMcAdams on 12/19/11.
//  Copyright (c) 2011 Jody McAdams. All rights reserved.
//

#include "CoreUI_Button.h"
#include "matrix.h"

void CoreUI_Button::SetActive(bool isActive)
{
	m_isActive = isActive;
}

static const f32 g_CoreUIButton_DefaultPressedBrightness = 0.5f;

void CoreUI_Button::PressButton(TouchState touchState, vec2* pBeginTouchPos, vec2* pCurrTouchPos)
{
	if(!m_isActive)
	{
		return;
	}
	
	const f32 beginX = pBeginTouchPos->x;
	const f32 beginY = pBeginTouchPos->y;
	
	
	const bool touchBeginIsInside =  beginX > m_position.x-m_halfWidth && beginX < m_position.x+m_halfWidth
		&& beginY > m_position.y-m_halfHeight && beginY < m_position.y+m_halfHeight;
	
	if(touchBeginIsInside)
	{
		const f32 currX = pCurrTouchPos->x;
		const f32 currY = pCurrTouchPos->y;
		
		const bool touchCurrIsInside =  currX > m_position.x-m_halfWidth && currX < m_position.x+m_halfWidth
		&& currY > m_position.y-m_halfHeight && currY < m_position.y+m_halfHeight;
		
		if(touchCurrIsInside)
		{
			switch(touchState)
			{
				case TouchState_Moving:	
				case TouchState_Began:
				{
					SetVec4(&m_diffuseColor,g_CoreUIButton_DefaultPressedBrightness,g_CoreUIButton_DefaultPressedBrightness,g_CoreUIButton_DefaultPressedBrightness,1.0f);
					
					m_buttonState = ButtonState_Pressed;
					
					break;
				}
				case TouchState_Ended:
				{
					if(m_buttonState == ButtonState_Pressed)
					{
						SetVec4(&m_diffuseColor,1.0f,1.0f,1.0f,1.0f);
						
						m_buttonState = ButtonState_Neutral;
						
						m_callback(m_value);					
					}
					
					break;
				}
				case TouchState_Stationary:
				{
					m_buttonState = ButtonState_Pressed;
					
					break;
				}
				case TouchState_Cancelled:
				{
					SetVec4(&m_diffuseColor,1.0f,1.0f,1.0f,1.0f);
					
					m_buttonState = ButtonState_Neutral;
					
					break;
				}
				default:
				{
					break;
				}
			}
		}
		else
		{
			SetVec4(&m_diffuseColor,1.0f,1.0f,1.0f,1.0f);
			
			m_buttonState = ButtonState_Neutral;
		}
	}
	else
	{
		SetVec4(&m_diffuseColor,1.0f,1.0f,1.0f,1.0f);
		
		m_buttonState = ButtonState_Neutral;
	}
}


void CoreUI_Button::Init(u32 width, u32 height, CoreUI_AttachSide attachSide, s32 offsetX, s32 offsetY, u32* textureHandle, s32 value, void (*callback)(s32))
{
	SetVec4(&m_diffuseColor,1.0f,1.0f,1.0f,1.0f);
	
	m_isActive = true;
	
	m_width = width;
	m_height = height;
	
	m_halfWidth = width/2;
	m_halfHeight = height/2;
	
	m_attachSide = attachSide;

	m_value = value;
	
	m_callback = callback;
	
	switch(attachSide)
	{
		case CoreUI_AttachSide_None:
		{
			m_position.x = offsetX;
			m_position.y = offsetY;
			
			break;
		}
		case CoreUI_AttachSide_Left:
		{
			m_position.x = offsetX;
			m_position.y = offsetY;
			
			break;
		}
		case CoreUI_AttachSide_Right:
		{
			m_position.x = GLRENDERER->screenWidth_points+offsetX;
			m_position.y = offsetY;
			
			break;
		}
		case CoreUI_AttachSide_Top:
		{
			m_position.x = GLRENDERER->screenWidth_points/2+offsetX;
			m_position.y = offsetY;
			
			break;
		}
		case CoreUI_AttachSide_Bottom:
		{
			m_position.x = GLRENDERER->screenWidth_points/2+offsetX;
			m_position.y = GLRENDERER->screenHeight_points+offsetY;
			
			break;
		}
		case CoreUI_AttachSide_TopLeft:
		{
			m_position.x = offsetX;
			m_position.y = offsetY;
			
			break;
		}
		case CoreUI_AttachSide_BottomLeft:
		{
			m_position.x = offsetX;
			m_position.y = GLRENDERER->screenHeight_points+offsetY;
			
			break;
		}
		case CoreUI_AttachSide_TopRight:
		{
			m_position.x = GLRENDERER->screenWidth_points+offsetX;
			m_position.y = offsetY;
			
			break;
		}
		case CoreUI_AttachSide_BottomRight:
		{
			m_position.x = GLRENDERER->screenWidth_points+offsetX;
			m_position.y = GLRENDERER->screenHeight_points+offsetY;
			
			break;
		}
		default:
		{
			break;
		}
	}
	
	GLRENDERER->InitRenderableObject3D(&m_r3D_button, &g_Square1x1_modelData, MT_TextureAndDiffuseColor, textureHandle, NULL, RenderLayer_UI, View_0, RenderFlagDefaults_2DTexture_NoAlpha);
	
	m_r3D_button.geom.material.uniqueUniformValues[0] = (u8*)&m_diffuseColor;
	
	GLRENDERER->AddRenderableObject3DToList(&m_r3D_button);

	mat4f_LoadScaleFromFloats(m_r3D_button.worldMat,width,height,1.0f);
	
	vec3* pPos = mat4f_GetPos(m_r3D_button.worldMat);
	pPos->x = m_position.x;
	pPos->y = m_position.y;
	pPos->z = 0.0f;
}


void CoreUI_Button::Show()
{
	m_r3D_button.geom.material.flags |= RenderFlag_Visible;
}


void CoreUI_Button::Hide()
{
	m_r3D_button.geom.material.flags &= ~RenderFlag_Visible;
}
