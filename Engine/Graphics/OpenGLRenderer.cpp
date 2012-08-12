//
//  OpenGLRenderer.mm (must be .mm because it can call NS functions)
//  InfiniSurv
//
//  Created by Jody McAdams on 11/21/11.
//  Copyright (c) 2011 Jody McAdams. All rights reserved.
//

#if defined (PLATFORM_OSX) || (defined PLATFORM_IOS)
#include <Foundation/Foundation.h>
#endif

#if defined (PLATFORM_IOS)
#include <UIKit/UIKit.h>
#endif

#include "OpenGLRenderer.h"
#include "Game.h"

OpenGLRenderer* GLRENDERER = NULL;

#include "matrix.h"
#include <iostream>
#include <cmath>
#include "GameUtil.h"
#include "ArrayUtil.h"
#include "ThirdParty/zlib/zlib.h"
#include "ThirdParty/libpng/png.h"
#include "ThirdParty/SOIL/SOIL.h"

#include "CoreObjects/CoreObjectFactories.h"

#include "CoreDebug.h"

#if defined (PLATFORM_OSX)
#include <OpenGL/glu.h>
#endif

#if defined (PLATFORM_WIN)
#include "glew/glew.h"
#include <GL/glu.h>
#endif

#include "MaterialDeclarations.h"

//DEBUG MODELS
#include "DEBUGMODEL_CircleXY.h"
#include "DEBUGMODEL_CircleXZ.h"
#include "DEBUGMODEL_Cylinder.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

//Circle verts: 2D
static const GLfloat g_circleVAR[] = {
	0.0000f,  1.0000f,
	0.2588f,  0.9659f,
	0.5000f,  0.8660f,
	0.7071f,  0.7071f,
	0.8660f,  0.5000f,
	0.9659f,  0.2588f,
	1.0000f,  0.0000f,
	0.9659f, -0.2588f,
	0.8660f, -0.5000f,
	0.7071f, -0.7071f,
	0.5000f, -0.8660f,
	0.2588f, -0.9659f,
	0.0000f, -1.0000f,
	-0.2588f, -0.9659f,
	-0.5000f, -0.8660f,
	-0.7071f, -0.7071f,
	-0.8660f, -0.5000f,
	-0.9659f, -0.2588f,
	-1.0000f, -0.0000f,
	-0.9659f,  0.2588f,
	-0.8660f,  0.5000f,
	-0.7071f,  0.7071f,
	-0.5000f,  0.8660f,
	-0.2588f,  0.9659f,
	0.0000f,  1.0000f,
	0.0f, 0.0f, // For an extra line to see the rotation.
};
static const int g_circleVAR_count = sizeof(g_circleVAR)/sizeof(GLfloat)/2;

//Debug verts
int g_numDebugVertices = 0;
GLfloat g_DebugVertices[DEBUG_LINES_MAXVERTICES*3];

//Verts to draw a 3D matrix
const GLfloat matrixVertices[] = {
	0.0f, 0.0f,0.0f,
	10.0f, 0.0f,0.0f,
	0.0f, 0.0f,0.0f,
	0.0f, 10.0f,0.0f,
	0.0f, 0.0f,0.0f,
	0.0f, 0.0f,10.0f,
};

//Colors for drawing the 3D matrix
const GLfloat matrixColors[] = {
	1.0f, 0.0f,0.0f,
	1.0f, 0.0f,0.0f,
	0.0f, 1.0f,0.0f,
	0.0f, 1.0f,0.0f,
	0.0f, 0.0f,1.0f,
	0.0f, 0.0f,1.0f,
};

//Verts for drawing a 2D square
const GLfloat squareVertices[] = {
	-1.0f, -1.0f,
	1.0f,  -1.0f,
	-1.0f,  1.0f,
	1.0f,   1.0f,
};

//Verts for drawing a 2D square
const GLfloat squareTexCoord_Normal[] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
};

//Verts for drawing a 2D square
const GLfloat squareTexCoord_Flipped[] = {
	0.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,
};

//Verts for drawing a 2D square
const GLfloat squareVerticesTR[] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
};

//Verts for drawing a 2D square
const GLfloat squareVerticesTL[] = {
	-1.0f, 1.0f,
	0.0f,  1.0f,
	-1.0f, 0.0f,
	0.0f,  0.0f,
};

//Verts for drawing a 2D square
const GLfloat squareVerticesBL[] = {
	-1.0f, -1.0f,
	0.0f,  -1.0f,
	-1.0f,  0.0f,
	0.0f,   0.0f,
};

//Verts for drawing a 2D square
const GLfloat squareVerticesBR[] = {
	1.0f, -1.0f,
	0.0f,  -1.0f,
	1.0f,  0.0f,
	0.0f,   0.0f,
};

//Texcoords for the 2D square
const GLfloat squareTexCoords[] = {
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
};

static Material g_Materials[NumRenderMaterials];
static u32 texture_pointSpriteAtlas = 0;
//static u32 texture_default = 0;

void OpenGLRenderer::Init(u32 screenWidthPixels, u32 screenHeightPixels,u32 screenWidthPoints, u32 screenHeightPoints)
{
	GLRENDERER = this;
    
    m_postProcessFlipper = false;
    
    m_clearColorPulseTimer = 0.0f;

	PrintOpenGLError("At init!");
	
	mat4f_LoadIdentity(m_identityMat);
	
	screenWidth_points = screenWidthPoints;
	screenHeight_points = screenHeightPoints;
	
	screenWidth_pixels = screenWidthPixels;
	screenHeight_pixels = screenHeightPixels;
	
	viewportWidth_pixels = screenWidthPoints;
	viewportHeight_pixels = screenHeightPoints;
	
	viewportPosX = 0;
	viewportPosY = 0;
	
	aspectRatio = static_cast<f32>(screenWidthPoints/screenHeightPoints);
	
	mat4f_LoadPerspective(m_projMats[ProjMatType_Perspective], 3.14f*0.25f,aspectRatio,1.0f,6000.0f);

	for(int i=0; i<NumCameraViews; ++i)
	{
		m_view[i] = m_identityMat;
	}
	
	//Set up projection matrices
	mat4f_LoadOrtho(m_projMats[ProjMatType_Orthographic_NDC], -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f);
	mat4f_LoadOrtho(m_projMats[ProjMatType_Orthographic_Points], 0.0f, (f32)screenWidth_points, (f32)screenHeight_points, 0.0f, 1.0f, -1.0f);
	
	COREDEBUG_PrintDebugMessage("%s %s", glGetString(GL_RENDERER), glGetString(GL_VERSION));

	/*** INITIALIZE INITIALIZE INITIALIZE ***/
    
	m_numVertexShaders = 0;
	m_numPixelShaders = 0;
	
	SetVec3(&m_fadeToScreenColor,1.0f,1.0f,1.0f);
	
    m_sortRenderablesByZ = false;
	
	m_numTexturedLines = 0;
	
	//m_numAnimatedPods = 0;
	

#ifdef PLATFORM_OSX
	m_supportsVAOs = true;
#endif

#ifdef PLATFORM_WIN
	m_supportsVAOs = glGenVertexArrays != NULL;
#endif

#ifdef PLATFORM_IOS
	m_supportsVAOs = [[[UIDevice currentDevice] systemVersion] floatValue] >= 4.0f;
#endif
	
#if TARGET_IPHONE_SIMULATOR
    m_supportsMultisampling = NO;
#else
    //const s32 processorCount = (s32)[[NSProcessInfo processInfo] processorCount];
    //COREDEBUG_PrintDebugMessage("This device has %d processors.",processorCount);
    
	const s32 processorCount = 1;
    m_supportsMultisampling = processorCount > 1; //Hack to enable multi-sampling only on iPad 2
#endif
    
	SetClearColor(0, 0, 0);
	
#ifdef PLATFORM_IOS
	glClearDepthf(1.0f);
#else
	glClearDepth(1.0f);
#endif
    
    SetVec3(&m_gravityDir,0.0f,-1.0f,0.0f);
    
    for (int i=0; i<5; ++i)
    {
        m_currTextureInTextureUnit[i] = 0;
    }
    
    m_screenShakeTimer = -1.0f;
    m_currScreenShakeAmount = 10.0f;
    
    m_maxNumRenderables = 0;
	
	ClearOneFrameGeometry();
    
    m_renderStateFlags = 0;
	m_blendMode = BlendMode_None;
    
    m_currTextureUnit = 999;
    
    m_requestedFadeIn = false;
	m_glBufferCount = 0;
	m_glTotalBufferSize = 0;
	m_glTotalTextureSize = 0;
    
	m_numModels = 0;
	
    m_screenShakerT_X = 0.0f;
    m_screenShakerT_Y = 0.0f;
    m_screenShakerSpeed_X = 50.0f;
    m_screenShakerSpeed_Y = 100.0f;
	
    paused = false;
	
	m_renderableObject3DsNeedSorting_UI = false;
	m_renderableObject3DsNeedSorting_Normal = false;
	
	for (s32 i=0; i<NUM_SINCOS_BUCKETS; ++i)
	{
		m_sinCosBuckets[i].multiplier = ((f32)(i+1)/(f32)NUM_SINCOS_BUCKETS)*NUM_SINCOS_BUCKET_MAX_MULT;
	}
	
	m_fadeState = FadeState_Idle;
    m_flashState = FadeState_Idle;
    m_pauseFadeState = FadeState_Idle;
    
	m_totalFadeTime = 0.0f;
	m_currFadeTime = 0.0f;
	m_currFadeValue = 1.0f;
		
	m_readyToGo = false;
	
	m_lastUsedMaterial = (RenderMaterial) RENDERMATERIAL_INVALID;
    
	PrintOpenGLError("Before loading textures");
	
	// Load all the textures
	//LoadTexture("TempTexture.tga", ImageType_TGA, &texture_default, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	
	
	//Create VBO buffers for terrain
	//CreateBuffers();
	
	PrintOpenGLError("Before creating materials");
	
	
	
	firstRun = true;
	
	m_accumulatedTime = 0.0f;
	m_accumulatedFrames = 0;
	ComputeGaussianWeights(m_gaussianWeights,GAUSSIAN_NUMSAMPLES,GAUSSIAN_STANDARDDEVIATION);
	
	m_currViewIndex = CameraView0;
	
	SetVec3(&m_lightPos, 10.0f,20.0f,10.0f);
	
	PrintOpenGLError("Before registering models");
	
    BufferModel(&g_DEBUGMODEL_CircleXY_modelData);
	BufferModel(&g_DEBUGMODEL_CircleXZ_modelData);
    BufferModel(&g_DEBUGMODEL_Cylinder_modelData);
    
	PrintOpenGLError("Initialized Renderer");

	m_numTexturedLines_saved = 0;

    
    //For post processing abilities
    
    //Create screen render target
    m_renderTarget_Screen.width = screenWidth_pixels;
    m_renderTarget_Screen.height = screenHeight_pixels;
    m_renderTarget_Screen.frameBuffer = 0;    
    
    //Create normal render target
    /*m_renderTarget_Normal.width = 320;
    m_renderTarget_Normal.height = 240;
    CreateFrameBuffer(&m_renderTarget_Normal.frameBuffer, &m_renderTarget_Normal.colorBuffer, true, NULL, false, ColorBufferType_Texture, m_renderTarget_Normal.width, m_renderTarget_Normal.height);
    
    //Create down sampled render target A
    m_renderTarget_DownsampleA.width = screenWidth_pixels/4;
    m_renderTarget_DownsampleA.height = screenHeight_pixels/4;
    CreateFrameBuffer(&m_renderTarget_DownsampleA.frameBuffer, &m_renderTarget_DownsampleA.colorBuffer, true, NULL, false, ColorBufferType_Texture, m_renderTarget_DownsampleA.width, m_renderTarget_DownsampleA.height);
    
    //Create down sampled render target B
    m_renderTarget_DownsampleB.width = screenWidth_pixels/4;
    m_renderTarget_DownsampleB.height = screenHeight_pixels/4;
    CreateFrameBuffer(&m_renderTarget_DownsampleB.frameBuffer, &m_renderTarget_DownsampleB.colorBuffer, true, NULL, false, ColorBufferType_Texture, m_renderTarget_DownsampleB.width, m_renderTarget_DownsampleB.height);*/
    
}


bool OpenGLRenderer::GetFadeFinished()
{
	return m_fadeState == FadeState_WaitingForFadeIn;
}


void OpenGLRenderer::ClearOneFrameGeometry()
{
	for(u32 i=0; i<DebugDrawMode_Num; ++i)
	{
		m_numDebugLinePoints[i] = 0;
		m_numDebugLinePoints_saved[i] = 0;
		
		m_numDebugDrawObjects[i] = 0;
		m_numDebugDrawObjects_saved[i] = 0;
	}

	m_numTexturedLines = 0;
}


void OpenGLRenderer::DisableVertexBufferObjects()
{
    //Do this to disable vertex array objects
    if(m_supportsVAOs)
    {
#ifdef PLATFORM_IOS
        glBindVertexArrayOES(0);
        
#elif defined PLATFORM_WIN
        glBindVertexArray(0);
        
#else
        glBindVertexArrayAPPLE(0);	
#endif
    }
    
    //Disable Vertex Buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    //Disable Index Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void OpenGLRenderer::SetRenderState(BlendMode blendMode, u32 renderFlags)
{
	//Depth Test
	if(m_hasSetRenderstateThisFrame == false
	   || (renderFlags & RenderFlag_EnableDepthTest) != (m_renderStateFlags & RenderFlag_EnableDepthTest))
	{
		if(renderFlags & RenderFlag_EnableDepthTest)
		{
			glEnable( GL_DEPTH_TEST );
		}
		else
		{
			glDisable( GL_DEPTH_TEST );
		}
	}
	
	//Depth read
	if(m_hasSetRenderstateThisFrame == false
	   || (renderFlags & RenderFlag_EnableDepthRead) != (m_renderStateFlags & RenderFlag_EnableDepthRead))
	{
		if(renderFlags & RenderFlag_EnableDepthRead)
		{
			glDepthFunc(GL_LEQUAL);
		}
		else
		{
			glDepthFunc(GL_ALWAYS);
		}
	}
	
	//Depth write
	if(m_hasSetRenderstateThisFrame == false
	   || (renderFlags & RenderFlag_EnableDepthWrite) != (m_renderStateFlags & RenderFlag_EnableDepthWrite))
	{
		if(renderFlags & RenderFlag_EnableDepthWrite)
		{
			glDepthMask(GL_TRUE);
		}
		else
		{
			glDepthMask(GL_FALSE);
		}
	}
	
	//Blending toggle
	if(m_hasSetRenderstateThisFrame == false
	   || (renderFlags & RenderFlag_BlendingEnabled) != (m_renderStateFlags & RenderFlag_BlendingEnabled))
	{
		if(renderFlags & RenderFlag_BlendingEnabled)
		{
			glEnable(GL_BLEND);
		}
		else
		{
			glDisable(GL_BLEND);
		}
	}
	
	//Blend mode
	if(m_hasSetRenderstateThisFrame == false
	   || blendMode != m_blendMode)
	{
		switch(blendMode)
		{
			case BlendMode_None:
			{
				break;
			}
			case BlendMode_Normal:
			{
				glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glBlendEquation(GL_FUNC_ADD);
				
				break;
			}
			case BlendMode_Premultiplied:
			{
				glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				glBlendEquation(GL_FUNC_ADD);
				
				break;
			}
			case BlendMode_DesaturatedAdd:
			{
				glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_COLOR);
				glBlendEquation(GL_FUNC_ADD);
				
				break;
			}
			case BlendMode_Add:
			{
				glBlendFunc (GL_ONE, GL_ONE);
				glBlendEquation(GL_FUNC_ADD);
				
				break;
			}
			case BlendMode_Subtract:
			{
				glBlendFunc (GL_SRC_COLOR, GL_ONE);
				glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
				
				break;
			}
			case BlendMode_Multiply:
			{
				glBlendFunc (GL_ZERO, GL_SRC_COLOR);
				glBlendEquation(GL_FUNC_ADD);
				
				break;
			}
			default:
			{
				break;
			}
		}
	}
	
	//Culling Toggle
	if(m_hasSetRenderstateThisFrame == false
	   || (renderFlags & RenderFlag_DisableCulling) != (m_renderStateFlags & RenderFlag_DisableCulling))
	{
		if(renderFlags & RenderFlag_DisableCulling)
		{
			glDisable(GL_CULL_FACE);
		}
		else
		{
			glEnable(GL_CULL_FACE);
		}
	}
	
	//Culling mode
	if(m_hasSetRenderstateThisFrame == false
	   || (renderFlags & RenderFlag_InverseCulling) != (m_renderStateFlags & RenderFlag_InverseCulling))
	{
		if(renderFlags & RenderFlag_InverseCulling)
		{
			glFrontFace(GL_CW);
		}
		else
		{
			glFrontFace(GL_CCW);
		}
	}
	
	//Save the current flags so we don't repeat renderstate calls
	m_renderStateFlags = renderFlags;
	
	m_blendMode = blendMode;
	
	m_hasSetRenderstateThisFrame = true;
}


void OpenGLRenderer::RenderLoop(u32 camViewIDX,RenderableGeometry3D* renderableObjectArray, u32 numRenderableObjects)
{
	u8* vertexData = NULL;
    DrawFunctionStruct* pLastDrawStruct = NULL;
    void (*pLastDrawEnd)() = NULL;

	for (u32 renderIDX = 0; renderIDX<numRenderableObjects; ++renderIDX)
	{
		const RenderableGeometry3D* pGeom = &renderableObjectArray[renderIDX];
		
		//Don't mess with or draw stuff not in this view
		if (!(pGeom->viewFlags & (1<<camViewIDX)))
		{
			continue;
		}
		
		const u32 renderFlags = pGeom->material.flags;
		
		//Don't mess with or draw invisible stuff
		if (!(renderFlags & RenderFlag_Visible))
		{
			continue;
		}
		
		if(renderFlags & RenderFlag_UseOrthographicProjection_NDC)
		{
			m_currProjMatType = ProjMatType_Orthographic_NDC;
		}
		else if(renderFlags & RenderFlag_UseOrthographicProjection_Points)
		{
			m_currProjMatType = ProjMatType_Orthographic_Points;
		}
		else
		{
			m_currProjMatType = ProjMatType_Perspective;
		}
		
		//Set material whenever it has changed
		
		RenderMaterial nextMaterial = pGeom->material.materialID;
		
		if(nextMaterial != MT_None)
		{
			if(nextMaterial != m_lastUsedMaterial)
			{
				SetMaterial(nextMaterial);
			}
			
			//At this point, either the texture was set by a material or its still set to
			//the last override texture
			
			//The material groups are sub-sorted by override texture, so all
			//the non-overridden things draw first.  Therefore, there is
			//never any need to reset back to the original texture from the
			//material.
			
			//Override material texture0 if needed
			//Will do nothing if the texture is set to 0 or the texture is already set
			SetTexture(pGeom->material.customTexture0, 0);
			SetTexture(pGeom->material.customTexture1, 1);
			
			//Upload uniforms that have unique values per object
			UploadUniqueUniforms(pGeom->material.uniqueUniformValues);
			
			//Draw the current object
			
			const Material* currMaterial = &g_Materials[m_lastUsedMaterial];
			//If there's a matrix at all
			if(currMaterial->uniform_worldViewProjMat != -1)
			{
				//Some objects ignore the view matrix
				if (renderFlags & RenderFlag_IgnoreViewMatrix)
				{
					UploadWorldProjMatrix(pGeom->worldMat);
				}
				//Some use the view matrix
				else
				{
					UploadWorldViewProjMatrix(pGeom->worldMat);
				}
			}
		}
		
		
		SetRenderState(pGeom->blendMode,renderFlags);
        
        void* pDrawObject = pGeom->drawObject;
        DrawFunctionStruct* pDrawStruct = pGeom->pDrawStruct;
        
		if(pDrawObject != NULL
           && pDrawStruct != NULL)
		{
            //We're drawing custom objects now, so
            //we gotta NULL this out
            vertexData = NULL;
            
            //If this draw struct is different from the last
            if(pDrawStruct != pLastDrawStruct)
            {
                //Struct changed, so uninit
                if(pLastDrawEnd != NULL)
                {
                    pLastDrawEnd();
                }
                
                //Save the last draw end function
                pLastDrawEnd = pGeom->pDrawStruct->endDrawFunc;
                
                //Struct changed, so save the new one
                pLastDrawStruct = pGeom->pDrawStruct;
                
                //Init the new drawfunc
                pDrawStruct->initDrawFunc();
            }
            
            //Draw the object
            pDrawStruct->drawFunc(pDrawObject);
		}
		else
		{
            //uninit the leftover custom draw object
            if(pLastDrawEnd != NULL)
            {
                pLastDrawEnd();
                pLastDrawEnd = NULL;
            }
            
            //We're drawing normal objects now, so
            //we gotta NULL this out
            pLastDrawStruct = NULL;
 
			const ModelData* pModelData = pGeom->pModel;
			
			for(unsigned int primIDX=0; primIDX<pModelData->numPrimitives; ++primIDX)
			{
				const PrimitiveData* currPrim = &pModelData->primitiveArray[primIDX];
				
#if defined (_DEBUG)
				assert(currPrim->vertexData != NULL);
#endif

				if (currPrim->vertexData != vertexData)
				{
					vertexData = currPrim->vertexData;
					if (m_supportsVAOs)
					{
						BindVertexArrayObject(currPrim->vertexArrayObjectID);
						
					}
					else
					{
						glBindBuffer(GL_ARRAY_BUFFER, currPrim->vertexArrayObjectID);
						EnableAttributes(pModelData);
						
					}
				}
				
				//Draw the primitive!
				if(currPrim->vertexArrayObjectID)
				{
					//NULL means non-indexed triangles
					if (currPrim->indexData == NULL)
					{
						glDrawArrays(currPrim->drawMethod, 0, currPrim->numVerts);
						
					}
					//Render using indices
					else
					{
						//VAOs don't save this I guess?
						BindIndexData(currPrim);

						glDrawElements(currPrim->drawMethod, currPrim->numVerts, GL_UNSIGNED_SHORT, 0);
						
					}
				}
			}
		}	
	}
}


const vec3* OpenGLRenderer::GetClearColor()
{
	return &m_clearColor;
}


void OpenGLRenderer::SetClearColor(f32 r, f32 g, f32 b)
{
	m_clearColor.x = r;
    m_clearColor.y = g;
    m_clearColor.z = b;
	
	glClearColor(m_clearColor.x,m_clearColor.y,m_clearColor.z,1.0);
}


void OpenGLRenderer::SetGravityDir(const vec3* pNewGravityDir)
{
	CopyVec3(&m_gravityDir,pNewGravityDir);
}


void OpenGLRenderer::SetSortRenderablesByZ(bool sortRenderablesByZ, RenderLayer layerStart, RenderLayer layerEnd)
{
	m_sortRenderablesByZ = sortRenderablesByZ;
	m_sortRenderablesLayerStart = layerStart;
	m_sortRenderablesLayerEnd = layerEnd;
}


void OpenGLRenderer::Render(f32 timeElapsed)
{   
	for (int i=0; i<5; ++i)
    {
        m_currTextureInTextureUnit[i] = 0;
    }
	
	if(!paused)
	{
		for(u32 i=0; i<DebugDrawMode_Num; ++i)
		{
			m_numDebugLinePoints_saved[i] = m_numDebugLinePoints[i];
			m_numDebugDrawObjects_saved[i] = m_numDebugDrawObjects[i];
			
			m_numDebugLinePoints[i] = 0;
			m_numDebugDrawObjects[i] = 0;
		}
		
		m_numTexturedLines_saved = m_numTexturedLines;
		m_numTexturedLines = 0;
	}
	
	//Multi-sampling disabled bye
   /* if (m_supportsMultisampling)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, msaaFramebuffer); 
    }*/
    
    //Update screen shake
    m_screenShakerT_X += m_screenShakerSpeed_X*timeElapsed;
    if(m_screenShakerT_X > 1.0f)
    {
        m_screenShakerT_X = 1.0f;
        m_screenShakerSpeed_X *= -1.0f;
    }
    else if(m_screenShakerT_X < -1.0f)
    {
        m_screenShakerT_X = -1.0f;
        m_screenShakerSpeed_X *= -1.0f;
    }
    
    m_screenShakerT_Y += m_screenShakerSpeed_Y*timeElapsed;
    if(m_screenShakerT_Y > 1.0f)
    {
        m_screenShakerT_Y = 1.0f;
        m_screenShakerSpeed_Y *= -1.0f;
    }
    else if(m_screenShakerT_Y < -1.0f)
    {
        m_screenShakerT_Y = -1.0f;
        m_screenShakerSpeed_Y *= -1.0f;
    }
    
	if (!paused)
	{
		++m_accumulatedFrames;
        
        if (m_accumulatedFrames == 10000)
        {
            m_accumulatedFrames = 0;
        }
		
		m_accumulatedTime = (f32)m_accumulatedFrames;
	}
	
	
	for (s32 i=0; i<NUM_SINCOS_BUCKETS; ++i)
	{
		const f32 sinCosT = (f32)m_accumulatedFrames*m_sinCosBuckets[i].multiplier;
		m_sinCosBuckets[i].sinTheta = sinf(sinCosT);
		m_sinCosBuckets[i].cosTheta = cosf(sinCosT);
	}
	
	//Sort renderables if needed
	if(m_renderableObject3DsNeedSorting_UI == true)
	{
		SortRenderableGeometry3DList(RenderableObjectType_UI);
		
		m_renderableObject3DsNeedSorting_UI = false;
	}
	
	if(m_renderableObject3DsNeedSorting_Normal == true)
	{
		SortRenderableGeometry3DList(RenderableObjectType_Normal);
	
		m_renderableObject3DsNeedSorting_Normal = false;
	}
    
    if(m_sortRenderablesByZ)
    {
        SortRenderablesInLayerRangeByZ(m_sortRenderablesLayerStart,m_sortRenderablesLayerEnd);
    }
	
	glEnable(GL_CULL_FACE);
	
	//Render renderables
	
#ifdef DEBUG_RENDERLOOP
	COREDEBUG_PrintDebugMessage("**Render start:");
	int skippedTextureSwaps = 0;
#endif
	
	//Render models in each camera view
	m_lastUsedMaterial = (RenderMaterial)RENDERMATERIAL_INVALID;
	
	
    //Reset renderstate flags
    m_renderStateFlags = 0;
	m_blendMode = BlendMode_None;
	m_hasSetRenderstateThisFrame = false;
	
	//for (u32 camViewIDX=0; camViewIDX<NumCameraViews; ++camViewIDX)
	{	
		//Set what camera we're currently rendering with
		//m_currViewIndex = camViewIDX;
        
        m_currViewIndex = 0;
		
		//Create view projection matrices once per view
		for(u32 i=0; i<NumProjMatTypes; ++i)
		{
            if (m_screenShakeTimer > 0.0f)
            {
                m_screenShakeTimer -= timeElapsed;
                
                const f32 shakeAmount = m_currScreenShakeAmount*MaxF(m_screenShakeTimer/m_screenShakeStartTime,0.0f);
                
                f32 viewMatCopy[16];
                memcpy(viewMatCopy,m_view[m_currViewIndex],sizeof(mat4f));
                
                AddScaledVec3_Self(mat4f_GetPos(viewMatCopy),mat4f_GetLeft(viewMatCopy),(m_screenShakerT_X+m_screenShakerT_Y)*shakeAmount);
                AddScaledVec3_Self(mat4f_GetPos(viewMatCopy),mat4f_GetUp(viewMatCopy),(m_screenShakerT_X-m_screenShakerT_Y)*shakeAmount);
                
                mat4f_Multiply(m_viewProjMats[m_currViewIndex][i],m_projMats[i], viewMatCopy);
                
            }
            else
            {
                mat4f_Multiply(m_viewProjMats[m_currViewIndex][i],m_projMats[i], m_view[m_currViewIndex]);
            }
		}
        
		//TODO: one day if we have multiple viewports, we'll have to link cameras to viewports
#if RENDERLOOP_ENABLED
        
        //Set normal render target
        //SetRenderTarget(&m_renderTarget_Normal);
        
        /*m_clearColorPulseTimer += timeElapsed;
        if(m_clearColorPulseTimer > 1.0f)
        {
            m_clearColorPulseTimer = 0.0f;
        }
        
        //Clear normal render target using pulse color
        SetClearColor(m_clearColorPulseTimer, 0, 0);*/
		
		SetClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z);
        
		//Make sure we can write to the depth buffer before clearing
		glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL) ;
        glDepthMask( GL_TRUE );
		
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        
        //Draw all the normal stuff
        RenderLoop(m_currViewIndex,g_Factory_Geometry_Normal.m_pObjectList,g_Factory_Geometry_Normal.m_numObjects);

        //Seriously?
#if defined(Platform_IOS)
        m_postProcessFlipper = true;
#else
        m_postProcessFlipper = false;
#endif
        
       // glDisable(GL_DEPTH_TEST);
        //PostProcess(PPMT_Copy, NULL, PPDrawArea_FullScreen, &m_renderTarget_Normal.colorBuffer, NULL, NULL);
		
        //TODO: move UI
		RenderLoop(0,g_Factory_Geometry_UI.m_pObjectList,g_Factory_Geometry_UI.m_numObjects);
        
        //Set lights render target
        //SetRenderTarget(&m_renderTarget_Lights);
        
        //Clear the lights target to black
        //SetClearColor(0, 0, 0);
        //glClear(GL_COLOR_BUFFER_BIT);
        
        //glDisable(GL_DEPTH_TEST);
        
        //Draw the lights
        //RenderLoop(m_currViewIndex,g_Factory_Geometry_Light.m_pObjectList,g_Factory_Geometry_Light.m_numObjects);
        
#endif        
	}
	
	DisableVertexBufferObjects();
	
	//Disable depth and culling
    glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
	//Enable blending
    glEnable(GL_BLEND);
    
    //Normal blending
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
	//Handle fading
	switch (m_pauseFadeState)
	{
        case FadeState_FadeOut:
		{
			m_pauseCurrFadeTime += timeElapsed;
			if (m_pauseCurrFadeTime > m_pauseTotalFadeTime)
			{
                m_pauseCurrFadeTime = m_pauseTotalFadeTime;
				m_pauseFadeState = FadeState_WaitingForFadeIn;
			}
			const f32 fadeT = m_pauseCurrFadeTime/m_pauseTotalFadeTime;
            
            const f32 finalFade = Lerp(0.0f,m_pauseFinalFade,fadeT);
			SetVec4(&m_fadeColor,0.0f,0.0f,0.0f,finalFade);
			PostProcess(PPMT_PureColor,NULL,PPDrawArea_FullScreen,NULL,NULL,NULL);
			
            
			break;
		}
        case FadeState_WaitingForFadeIn:
        {
            SetVec4(&m_fadeColor,0.0f,0.0f,0.0f,m_pauseFinalFade);
			PostProcess(PPMT_PureColor,NULL,PPDrawArea_FullScreen,NULL,NULL,NULL);
			
            
            if (m_pauseRequestedFadeIn == true)
            {
                m_pauseRequestedFadeIn = false;
                m_pauseCurrFadeTime = m_pauseTotalFadeTime;
                
                m_pauseFadeState = FadeState_FadeIn;
            }
            break;
        }
		case FadeState_FadeIn:
		{
			m_pauseCurrFadeTime -= timeElapsed;
			if (m_pauseCurrFadeTime < 0.0f)
			{
				m_pauseFadeState = FadeState_Idle;
				m_pauseCurrFadeTime = 0.0f;
			}
			const f32 fadeT = m_pauseCurrFadeTime/m_pauseTotalFadeTime;
            
            const f32 finalFade = Lerp(0.0f,m_pauseFinalFade,fadeT);
			SetVec4(&m_fadeColor,0.0f,0.0f,0.0f,finalFade);
			PostProcess(PPMT_PureColor,NULL,PPDrawArea_FullScreen,NULL,NULL,NULL);
			
			
			break;
		}
		case FadeState_Idle:
		{
			break;
		}
		default:
			break;
	}
    
    
    //Additive blending
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);
    
    //Handle flashing
	switch (m_flashState)
	{
		case FadeState_FadeOut:
		{
			m_currFlashTime += timeElapsed;
			if (m_currFlashTime > m_totalFlashTime)
			{
                m_currFlashTime = m_totalFlashTime;
				m_flashState = FadeState_FadeIn;
			}
			const f32 fadeT = m_currFlashTime/m_totalFlashTime;
            
			SetVec4(&m_fadeColor,m_flashColor.x,m_flashColor.y,m_flashColor.z,fadeT);
			PostProcess(PPMT_PureColor,NULL,PPDrawArea_FullScreen,NULL,NULL,NULL);
			
            
			break;
		}
		case FadeState_FadeIn:
		{
			m_currFlashTime -= timeElapsed;
			if (m_currFlashTime < 0.0f)
			{
				m_flashState = FadeState_Idle;
				m_currFlashTime = 0.0f;
			}
			const f32 fadeT = m_currFlashTime/m_totalFlashTime;
            
			SetVec4(&m_fadeColor,m_flashColor.x,m_flashColor.y,m_flashColor.z,fadeT);
			PostProcess(PPMT_PureColor,NULL,PPDrawArea_FullScreen,NULL,NULL,NULL);
			
			
			break;
		}
		default:
			break;
	}
    
    //Normal blending
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
	//Handle fading
	switch (m_fadeState)
	{
		case FadeState_FadeOut:
		{
			m_currFadeTime += timeElapsed;
			if (m_currFadeTime > m_totalFadeTime)
			{
                m_currFadeTime = m_totalFadeTime;
				m_fadeState = FadeState_WaitingForFadeIn;
			}
			const f32 fadeT = m_currFadeTime/m_totalFadeTime;
			
			m_currFadeValue = 1.0f-fadeT;
			
			CopyVec3((vec3*)&m_fadeColor,&m_fadeToScreenColor);
			m_fadeColor.w = fadeT;
			
			PostProcess(PPMT_PureColor,NULL,PPDrawArea_FullScreen,NULL,NULL,NULL);
			
			
			break;
		}
        case FadeState_WaitingForFadeIn:
        {
            CopyVec3((vec3*)&m_fadeColor,&m_fadeToScreenColor);
			m_fadeColor.w = 1.0f;
			
			PostProcess(PPMT_PureColor,NULL,PPDrawArea_FullScreen,NULL,NULL,NULL);
			
            
            if (m_requestedFadeIn == true)
            {
                m_requestedFadeIn = false;
                m_currFadeTime = m_totalFadeTime;
				
                m_fadeState = FadeState_FadeIn;
            }
            break;
        }
		case FadeState_FadeIn:
		{
			m_currFadeTime -= timeElapsed;
			if (m_currFadeTime < 0.0f)
			{
				m_fadeState = FadeState_Idle;
				m_currFadeTime = 0.0f;
			}
			const f32 fadeT = m_currFadeTime/m_totalFadeTime;
			
			m_currFadeValue = 1.0f-fadeT;
			
			CopyVec3((vec3*)&m_fadeColor,&m_fadeToScreenColor);
			m_fadeColor.w = fadeT;
			
			PostProcess(PPMT_PureColor,NULL,PPDrawArea_FullScreen,NULL,NULL,NULL);
			
			
			break;
		}
		case FadeState_Idle:
		{
			m_currFadeValue = 1.0f; 
			
			break;
		}
		default:
			break;
	}
	
#if DEBUG_DRAW
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    

    //Disable depth test
	glDisable( GL_DEPTH_TEST );
    
	//Disable depth write
	glDepthMask( GL_FALSE );
	
    
	//DRAW DEBUG LINES
	
	//Render debug stuff
	SetMaterial(MT_VertColors);
	
	
	for(u32 debugIDX=0; debugIDX<DebugDrawMode_Num; ++debugIDX)
	{
		//Set up view/proj
		
		switch((DebugDrawMode)debugIDX)
		{
			case DebugDrawMode_2D:
			{
				//Draw 2D lines
				m_currProjMatType = ProjMatType_Orthographic_Points;
				UploadWorldViewProjMatrix(m_identityMat);
				
				
				break;
			}
			case DebugDrawMode_3D:
			{
				//Draw 3D lines
				m_currProjMatType = ProjMatType_Perspective;
				UploadWorldViewProjMatrix(m_identityMat);
				
				
				break;
			}
			case DebugDrawMode_Screen2D:
			{
				//Draw to the screen with no view matrix in 2D
				m_currProjMatType = ProjMatType_Orthographic_Points;
				UploadWorldProjMatrix(m_identityMat);
				
				
				break;
			}
			default:
			{
				break;
			}
		}
		
		glEnableVertexAttribArray(ATTRIB_VERTEX);
		
		glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, 0, sizeof(PointData), &m_debugLinePoints[debugIDX][0].position);
		
		
		glEnableVertexAttribArray(ATTRIB_COLOR);
		
		glVertexAttribPointer(ATTRIB_COLOR, 4, GL_FLOAT, 0, sizeof(PointData), &m_debugLinePoints[debugIDX][0].color);
		
		
		const u32 numToDraw = m_numDebugLinePoints_saved[debugIDX];
		glDrawArrays(GL_LINES, 0, numToDraw);
	}
	
	
	//DRAW DEBUG OBJECTS
	
	SetMaterial(MT_VertWithColorInput);
	
	for(u32 debugIDX=0; debugIDX<DebugDrawMode_Num; ++debugIDX)
	{
		for(u32 i=0; i<m_numDebugDrawObjects_saved[debugIDX]; ++i)
		{
			DebugDrawObject* pCurrObject = &m_debugDrawObjects[debugIDX][i];
			
			switch((DebugDrawMode)debugIDX)
			{
				case DebugDrawMode_2D:
				{
					//Draw 2D lines
					m_currProjMatType = ProjMatType_Orthographic_Points;
					UploadWorldViewProjMatrix(pCurrObject->matrix);
					
					break;
				}
				case DebugDrawMode_3D:
				{
					//Draw 3D lines
					m_currProjMatType = ProjMatType_Perspective;
					UploadWorldViewProjMatrix(pCurrObject->matrix);
					
					break;
				}
				case DebugDrawMode_Screen2D:
				{
					//Draw to the screen with no view matrix in 2D
					m_currProjMatType = ProjMatType_Orthographic_Points;
					UploadWorldProjMatrix(pCurrObject->matrix);
					
					break;
				}
				default:
				{
					break;
				}
			}
			
			glUniform4fv(g_Materials[MT_VertWithColorInput].uniforms_unique[0],1,(f32*)&pCurrObject->color);
			
			ModelData* pModelData = NULL;
			
			switch(pCurrObject->objectType)
			{
				case DebugDrawObjectType_CircleXY:
				{
					pModelData = &g_DEBUGMODEL_CircleXY_modelData;
					
					break;
				}
				case DebugDrawObjectType_CircleXZ:
				{
					pModelData = &g_DEBUGMODEL_CircleXZ_modelData;
					
					break;
				}
				case DebugDrawObjectType_Cylinder:
				{
					pModelData = &g_DEBUGMODEL_Cylinder_modelData;
					
					break;
				}
				default:
				{
					break;
				}
			}
			
			for(u32 i=0; i<pModelData->numPrimitives; ++i)
			{
				PrimitiveData* pCurrPrim = &pModelData->primitiveArray[i];
				if (m_supportsVAOs)
				{
					BindVertexArrayObject(pCurrPrim->vertexArrayObjectID);
				}
				else
				{
					glBindBuffer(GL_ARRAY_BUFFER, pCurrPrim->vertexArrayObjectID);
					EnableAttributes(pModelData);
				}
				
				glDrawArrays(pCurrPrim->drawMethod, 0, pCurrPrim->numVerts);
			}
		}
	}
	
#endif
    
	//if (parentView.drawMatrices)
	 //{	
	 //m_currProjMatType = ProjMatType_Perspective;
	 
	 //[self setMaterial: MT_VertColors];
	 
	 //for (u32 renderIndex=0; renderIndex<m_numRenderableObject3Ds; ++renderIndex)
	 //{
	 //const RenderableObject3D* pCurrRenderableObject3D = m_renderableObject3DList[renderIndex];
	 //if(pCurrRenderableObject3D->flags & RenderFlag_Visible)
	 //{
	 //[self uploadWorldViewProjMatrix:pCurrRenderableObject3D->worldMat];		
	 //Draw_matrix();
	 //}
	// }
	 //}

    
	
	// if (ENABLE_MULTISAMPLING)
	 //{
	 //glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, viewFrameBuffer);
	 //glBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE,  msaaFramebuffer);
	 //glResolveMultisampleFramebufferAPPLE();
	 
	 //const GLenum discards[]  = {GL_COLOR_ATTACHMENT0,GL_DEPTH_ATTACHMENT};
	 //glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE,2,discards);
	 
	 //glBindRenderbuffer(GL_RENDERBUFFER, viewFrameBuffer);
	 //}
	 //else
	 //{
	 //const u32 discards[]  = {GL_DEPTH_ATTACHMENT};
	 //glDiscardFramebufferEXT(GL_FRAMEBUFFER,1,discards);
	 //}
	
	
	//dump contents of buffer into the presentRenderBuffer and show on the screen
	//[context presentRenderbuffer:GL_RENDERBUFFER];
    
    
}


bool OpenGLRenderer::LoadTextureFromData(u32* out_textureName,const void* data,u32 texWidth, u32 texHeight, u32 format, u32 type, u32 filterMode, u32 wrapModeU, u32 wrapModeV)
{
	if (filterMode == GL_LINEAR_MIPMAP_LINEAR || filterMode == GL_LINEAR_MIPMAP_NEAREST)
	{
		return false;
	}
	
	glGenTextures(1, out_textureName);
	glBindTexture(GL_TEXTURE_2D, *out_textureName);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,filterMode); 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,filterMode);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapModeU);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapModeV);
	
	glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth,texHeight, 0, format, type, data);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	return true;
}


bool OpenGLRenderer::UpdateTextureFromData(u32* out_textureName, const void* data, u32 texWidth, u32 texHeight, u32 format, u32 type)
{
	glBindTexture(GL_TEXTURE_2D, *out_textureName);
	
	glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth,texHeight, 0, format, type, data);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	return true;
}


//NOTE: UNTESTED
void OpenGLRenderer::UpdateVBO(u32 VBO, void* pVerts, u32 dataSize, GLenum useage)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,dataSize,pVerts,useage);
}


//NOTE: UNTESTED
void OpenGLRenderer::CreateVBO(u32* pOut_VAO, u32* pOut_VBO, void* pVerts, u32 dataSize, GLenum useage, const AttributeData* pAttrib, u32 numAttribs, u32 vertSize)
{	
	//Use vertex array object
	if (m_supportsVAOs)
	{
#if defined (PLATFORM_IOS)
		glGenVertexArraysOES(1, pOut_VAO);
		glBindVertexArrayOES(*pOut_VAO);
#endif
		
#if defined (PLATFORM_OSX)
		glGenVertexArraysAPPLE(1, pOut_VAO);
		glBindVertexArrayAPPLE(*pOut_VAO);
#endif
		
#if defined (PLATFORM_WIN)
		glGenVertexArrays(1, pOut_VAO);
		glBindVertexArray(*pOut_VAO);
#endif
		
	}
	//Use plain old VBO
	else
	{
		//No VAO
		*pOut_VAO = 0;	
	}
	
	glGenBuffers(1,pOut_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, *pOut_VBO);
	PrintOpenGLError("Binding vertex buffer");
	
	glBufferData(GL_ARRAY_BUFFER,dataSize,pVerts,useage);
	PrintOpenGLError("Uploading vertex buffer");
	
	PrintOpenGLError("After binding VertexArrayObject");
	
	//If we're using VAO, enable the attributes here once and never again
	if (m_supportsVAOs)
	{
		for(u32 i=0; i<numAttribs; ++i)
		{
			const AttributeData* pCurrAttrib = &pAttrib[i];
			
			EnableAttribute(pCurrAttrib, vertSize);
		}

		//Make sure nothing randomly writes to our new VAO
#ifdef PLATFORM_IOS
		glBindVertexArrayOES(0);
#elif  PLATFORM_WIN
		glBindVertexArray(0);
#else
		glBindVertexArrayAPPLE(0);
#endif
		
		PrintOpenGLError("After resetting VAO");
	}
}


void OpenGLRenderer::BufferModel(ModelData* pModelData)
{
	//TODO: make this use my CreateVBO function
	
	PrintOpenGLError("/*** Entered BufferModel ***/");
	
	for(u32 i=0; i<pModelData->numPrimitives; ++i)
	{
		PrimitiveData* currPrim = &pModelData->primitiveArray[i];
		
        PrintOpenGLError("Before binding VertexArrayObject");
		
        //Use vertex array object
        if (m_supportsVAOs)
        {
            u32 vertexBufferID;
            glGenBuffers(1,&vertexBufferID);

#if defined (PLATFORM_IOS)
			glGenVertexArraysOES(1, &currPrim->vertexArrayObjectID);
            glBindVertexArrayOES(currPrim->vertexArrayObjectID);
#endif

#if defined (PLATFORM_OSX)
			glGenVertexArraysAPPLE(1, &currPrim->vertexArrayObjectID);
            glBindVertexArrayAPPLE(currPrim->vertexArrayObjectID);
#endif

#if defined (PLATFORM_WIN)
			glGenVertexArrays(1, &currPrim->vertexArrayObjectID);
            glBindVertexArray(currPrim->vertexArrayObjectID);
#endif
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
            PrintOpenGLError("Binding vertex buffer");
        }
        //Use plain old VBO
        else
        {
            glGenBuffers(1,&currPrim->vertexArrayObjectID);
            glBindBuffer(GL_ARRAY_BUFFER, currPrim->vertexArrayObjectID);
        }
		
		glBufferData(GL_ARRAY_BUFFER,currPrim->sizeOfVertexData,currPrim->vertexData,GL_STATIC_DRAW);
		PrintOpenGLError("Uploading vertex buffer");
		
        PrintOpenGLError("After binding VertexArrayObject");
        
        //If we're using VAO, enable the attributes here once and never again
        if (m_supportsVAOs)
        {
            EnableAttributes(pModelData);
			
            //Make sure nothing randomly writes to our new VAO
#ifdef PLATFORM_IOS
			glBindVertexArrayOES(0);
#elif  PLATFORM_WIN
			glBindVertexArray(0);
#else
			glBindVertexArrayAPPLE(0);
#endif
            
            PrintOpenGLError("After resetting VAO");
        }  
        
        //Use index data if it exists
		if (currPrim->indexData != NULL)
		{
			glGenBuffers(1,&currPrim->indexBufferID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, currPrim->indexBufferID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, currPrim->sizeOfIndexData, currPrim->indexData, GL_STATIC_DRAW);
		}
	}
	
	pModelData->modelID = m_numModels++;
	
	PrintOpenGLError("End of Register Model");
}


CoreObjectHandle OpenGLRenderer::CreateRenderableGeometry3D(RenderableObjectType renderableType, RenderableGeometry3D** pOut_Geom)
{
	RenderableGeometry3D* pGeom = NULL;
	
	//UI
	if(renderableType == RenderableObjectType_UI)
	{
		
		pGeom = g_Factory_Geometry_UI.CreateObject(0);
	}
	//Normal
	else
	{
		pGeom = g_Factory_Geometry_Normal.CreateObject(0);
	}
	
	if(pGeom == NULL)
	{
		COREDEBUG_PrintDebugMessage("INSANE ERROR: You're out of RenderableObject3Ds!");
		return CoreObjectHandle();
	}

	const CoreObjectHandle handle = pGeom->GetHandle();
	
	if(handle.IsValid() == false)
	{
		return CoreObjectHandle();
	}
	
	pGeom->material.flags = 0;
	
	//UI
	if(renderableType == RenderableObjectType_UI)
	{
		m_renderableObject3DsNeedSorting_UI = true;
		
		//COREDEBUG_PrintDebugMessage("Created a UI RenderableGeometry3D handle %d!  Count: %d",pGeom->GetHandle(),m_numRenderableGeom3Ds_UI);
	}
	//Normal
	else
	{
		m_renderableObject3DsNeedSorting_Normal = true;
		
		//COREDEBUG_PrintDebugMessage("Created a normal RenderableGeometry3D handle %d!  Count: %d",pGeom->GetHandle(),m_numRenderableGeom3Ds_Normal);
	}
	
	if(pOut_Geom != NULL)
	{
		*pOut_Geom = pGeom;
	}
	
	//assert(pGeom->GetHandle() != CoreObjectHandle::Invalid());
	
	return handle;
}


/*void OpenGLRenderer::InitRenderableSceneObject3D_Simple(RenderableSceneObject3D* renderableObject, RenderableScene3D* pScene, mat4f matrix4x4, u32 viewFlags)
{
	renderableObject->pScene = pScene;
	renderableObject->pMaterialOverride = NULL;
	renderableObject->visible = true;
	renderableObject->pParentMat = NULL;
	renderableObject->markedForRemoval = false;
	
	if(matrix4x4 == NULL)
	{
		mat4f_LoadIdentity(renderableObject->worldMat);
	}
	else
	{
		mat4f_Copy(renderableObject->worldMat,matrix4x4);
	}
}


void OpenGLRenderer::InitRenderableSceneObject3D(RenderableSceneObject3D* renderableObject, RenderableScene3D* pScene, RenderMaterial materialID, u32* customTexture, mat4f matrix4x4, RenderLayer renderLayer, u32 viewFlags, u32 renderFlags)
{
	renderableObject->pScene = pScene;
	renderableObject->visible = true;
	renderableObject->pParentMat = NULL;
	renderableObject->markedForRemoval = false;
	
	RenderableMaterial* pMaterial = &renderableObject->material;
	pMaterial->materialID = materialID;
	pMaterial->customTexture0 = customTexture;
	pMaterial->flags = renderFlags;
	
	renderableObject->pMaterialOverride = pMaterial;
		
	for (s32 i=0; i<MAX_UNIQUE_UNIFORM_VALUES; ++i)
	{
		pMaterial->uniqueUniformValues[i] = NULL;
	}
	
	if(matrix4x4 == NULL)
	{
		mat4f_LoadIdentity(renderableObject->worldMat);
	}
	else
	{
		mat4f_Copy(renderableObject->worldMat,matrix4x4);
	}
}*/

void OpenGLRenderer::InitRenderableGeometry3D_Shared(RenderableGeometry3D* renderableObject, RenderMaterial materialID, u32* customTexture, mat4f matrix4x4, RenderLayer renderLayer, BlendMode blendMode, u32 renderFlags)
{
	renderableObject->material.materialID = materialID;
	renderableObject->material.flags = renderFlags|RenderFlag_Initialized;
	renderableObject->renderLayer = renderLayer;
	renderableObject->material.customTexture0 = customTexture;
	renderableObject->material.customTexture1 = NULL;
	renderableObject->viewFlags = View_0;
	renderableObject->blendMode = blendMode;
	renderableObject->postRenderLayerSortValue = 0;
	
	for (s32 i=0; i<MAX_UNIQUE_UNIFORM_VALUES; ++i)
	{
		renderableObject->material.uniqueUniformValues[i] = NULL;
	}
	
	if(matrix4x4 == NULL)
	{
		mat4f_LoadIdentity(renderableObject->worldMat);
	}
	else
	{
		mat4f_Copy(renderableObject->worldMat,matrix4x4);
	}
}

void OpenGLRenderer::InitRenderableGeometry3D(RenderableGeometry3D* renderableObject, DrawFunctionStruct* pDrawStruct, void* drawObject, RenderMaterial materialID, u32* customTexture, mat4f matrix4x4, RenderLayer renderLayer, BlendMode blendMode, u32 renderFlags)
{
	if(renderableObject == NULL)
	{
		COREDEBUG_PrintDebugMessage("INSANE ERROR: You are tried to initialize a NULL renderable!  Skipping...");
		return;
	}
	
	if(pDrawStruct == NULL)
	{
		COREDEBUG_PrintDebugMessage("INSANE ERROR: You are tried to initialize a renderable with a NULL draw structure!  Skipping...");
		return;
	}
	
	renderableObject->pDrawStruct = pDrawStruct;
	renderableObject->drawObject = drawObject;
	
	InitRenderableGeometry3D_Shared(renderableObject,materialID,customTexture,matrix4x4,renderLayer,blendMode,renderFlags);
}


void OpenGLRenderer::InitRenderableGeometry3D(RenderableGeometry3D* renderableObject, ModelData* pModel, RenderMaterial materialID, u32* customTexture, mat4f matrix4x4, RenderLayer renderLayer, BlendMode blendMode, u32 renderFlags)
{
	if(renderableObject == NULL)
	{
		COREDEBUG_PrintDebugMessage("INSANE ERROR: You are tried to initialize a NULL renderable!  Skipping...");
		return;
	}
	
	if(pModel == NULL || pModel->modelID == -1)
	{
		COREDEBUG_PrintDebugMessage("INSANE ERROR: You are tried to initialize a renderable with an unregistered model '%s'!  Skipping...",pModel->modelName);
		return;
	}

	renderableObject->drawObject = NULL;
	renderableObject->pModel = pModel;
	
	InitRenderableGeometry3D_Shared(renderableObject,materialID,customTexture,matrix4x4,renderLayer,blendMode,renderFlags);
}


void OpenGLRenderer::SortRenderableGeometry3DList(RenderableObjectType type)
{
	if(type == RenderableObjectType_UI)
	{
		//SORT UI
		
		//Create sort values
		for(u32 i=0; i<g_Factory_Geometry_UI.m_numObjects; ++i)
		{
			
			RenderableGeometry3D* pGeom = &g_Factory_Geometry_UI.m_pObjectList[i];
			u32 sortValue = 0;
			
			//I got 32 bits to use up
			
			//Going to consider 0 to be the most significant bit for ease of reading
			GU_InsertPositiveValueAsBits(&sortValue,pGeom->renderLayer,0,4);
			
			GU_InsertPositiveValueAsBits(&sortValue,pGeom->postRenderLayerSortValue,4,5);
			
			//This used to do more but lets do less for now
			{
				GU_InsertPositiveValueAsBits(&sortValue,0,9,5);
			}
			
			GU_InsertPositiveValueAsBits(&sortValue,pGeom->material.materialID,14,5);
			
			const u32 texture = pGeom->material.customTexture0?*pGeom->material.customTexture0:0;
			
			GU_InsertPositiveValueAsBits(&sortValue,texture,19,5);
			
			pGeom->sortValue = sortValue;
		}
		
		g_Factory_Geometry_UI.Sort(RenderableGeometry3DCompare_SortValue);
	}
	else
	{
		//Create sort values
		for(u32 i=0; i<g_Factory_Geometry_Normal.m_numObjects; ++i)
		{
			RenderableGeometry3D* pGeom = &g_Factory_Geometry_Normal.m_pObjectList[i];
			u32 sortValue = 0;
			
			//I got 32 bits to use up
			
			//Going to consider 0 to be the most significant bit for ease of reading
			GU_InsertPositiveValueAsBits(&sortValue,pGeom->renderLayer,0,4);
			
			GU_InsertPositiveValueAsBits(&sortValue,pGeom->postRenderLayerSortValue,4,5);
			
			//This used to do more but lets do less for now
			{
				GU_InsertPositiveValueAsBits(&sortValue,0,9,5);
			}
			
			GU_InsertPositiveValueAsBits(&sortValue,pGeom->material.materialID,14,5);
			
			const u32 texture = pGeom->material.customTexture0?*pGeom->material.customTexture0:0;
			
			GU_InsertPositiveValueAsBits(&sortValue,texture,19,5);
			
			pGeom->sortValue = sortValue;
		}
		
		//Sort by RenderLayer
		g_Factory_Geometry_Normal.Sort(RenderableGeometry3DCompare_SortValue);
	}

}


s32 OpenGLRenderer::AddVertexShaderToList(const char* filename)
{
	if(m_numVertexShaders < MAX_COMPILED_VERTEX_SHADERS)
	{
		const s32 newIndex = ++m_numVertexShaders;
		m_vertexShaders[newIndex].filename = filename;
		m_vertexShaders[newIndex].openGLID = 0;

		return newIndex;
	}
	else
	{
		return -1;
	}
}

s32 OpenGLRenderer::AddPixelShaderToList(const char* filename)
{
	if(m_numPixelShaders < MAX_COMPILED_PIXEL_SHADERS)
	{
		const s32 newIndex = ++m_numPixelShaders;
		m_pixelShaders[newIndex].filename = filename;
		m_pixelShaders[newIndex].openGLID = 0;
		
		return newIndex;
	}
	else
	{
		return -1;
	}
}


void OpenGLRenderer::CreateMaterials()
{
	PrintOpenGLError("Before create material");
	
	//Vertex Shaders
	const s32 VSH_TextureAndVertColor = AddVertexShaderToList("Shaders/TextureAndVertColor.vsh");
	const s32 VSH_WiggleUsingTexcoordV = AddVertexShaderToList("Shaders/WiggleUsingTexcoordV.vsh");
	const s32 VSH_FullScreenQuad = AddVertexShaderToList("Shaders/pp_default.vsh");
    const s32 VSH_FullScreenQuadNoTexcoord = AddVertexShaderToList("Shaders/pp_noTexcoord.vsh");
	const s32 VSH_VertColors = AddVertexShaderToList("Shaders/VertColors.vsh");
    const s32 VSH_VertWithColorInput = AddVertexShaderToList("Shaders/VertWithColorInput.vsh");
    const s32 VSH_VertWithColorInputAndTexture = AddVertexShaderToList("Shaders/VertWithColorInputAndTexture.vsh");
    const s32 VSH_VertWithColorInputWithTexcoordOffset = AddVertexShaderToList("Shaders/VertWithColorInputWithTexcoordOffset.vsh");
	const s32 VSH_TextureOnly = AddVertexShaderToList("Shaders/TextureOnly.vsh");
    const s32 VSH_TextureWithColor = AddVertexShaderToList("Shaders/TextureWithColor.vsh");
	const s32 VSH_VertWithTexcoordOffset = AddVertexShaderToList("Shaders/VertWithTexcoordOffset.vsh");
	const s32 VSH_WorldSpace_VertWithTexcoord = AddVertexShaderToList("Shaders/WorldSpace_VertWithTexcoord.vsh");
	const s32 VSH_VertWithTexcoordAndOffsetTexcoord = AddVertexShaderToList("Shaders/VertWithTexcoordAndOffsetTexcoord.vsh");
	const s32 VSH_VertWithOffsetTexcoordAndOffsetTexcoord = AddVertexShaderToList("Shaders/VertWithOffsetTexcoordAndOffsetTexcoord.vsh");
	const s32 VSH_TextureOnlyWithTexcoordAndWorldOffset = AddVertexShaderToList("Shaders/TextureOnlyWithTexcoordAndWorldOffset.vsh");
	const s32 VSH_PointSprite_Basic = AddVertexShaderToList("Shaders/PointSprite_Default.vsh");
	//const s32 VSH_SkinnedVertShader = AddVertexShaderToList("Shaders/SkinnedVertShader.vsh");
	
	//Pixel Shaders
	const s32 PP_BlendUsingTexture = AddPixelShaderToList("Shaders/BlendUsingTexture.fsh");
	const s32 PP_PureColor = AddPixelShaderToList("Shaders/PureColor.fsh");
    const s32 PS_TextureOnlySimple = AddPixelShaderToList("Shaders/TextureOnlySimple.fsh");
    const s32 PS_TextureAlphaAndDiffuseColor = AddPixelShaderToList("Shaders/TextureAlphaAndDiffuseColor.fsh");
    const s32 PS_TextureOnlyDiscard = AddPixelShaderToList("Shaders/TextureOnlyDiscard.fsh");
    const s32 PS_TextureAndDiffuseColor = AddPixelShaderToList("Shaders/TextureAndDiffuseColor.fsh");
    const s32 PS_TextureAndDiffuseColorDiscard = AddPixelShaderToList("Shaders/TextureAndDiffuseColorDiscard.fsh");
	const s32 PS_TextureAndFogColor = AddPixelShaderToList("Shaders/TextureAndFogColor.fsh");
	const s32 PS_TextureAndFogColorDiscard = AddPixelShaderToList("Shaders/TextureAndFogColorDiscard.fsh");
	//const s32 PS_TextureAndFogColor = AddPixelShaderToList("Shaders/TextureAndFogColor.fsh");
	const s32 PS_Colors = AddPixelShaderToList("Shaders/Colors.fsh");
	const s32 PS_PointSprite_ColorShine = AddPixelShaderToList("Shaders/PointSprite_ColorShine.fsh");
    const s32 PS_TextureWithColor = AddPixelShaderToList("Shaders/TextureWithColor.fsh");
	const s32 PS_MultiplyTwoSamples = AddPixelShaderToList("Shaders/MultiplyTwoSamples.fsh");
	
    const s32 PS_Trippin = AddPixelShaderToList("Shaders/Trippin.fsh");
    
	//const s32 PS_SkinnedFragShader = AddPixelShaderToList("Shaders/SkinnedFragShader.fsh");
	
	//const s32 PS_TextureAndFogColorWithMipMapBlur = AddPixelShaderToList("Shaders/TextureAndFogColorWithMipmapBlur.fsh");
	
	//Vertex Shaders
	
	//Setup common to all materials
	for(int i=0; i<NumRenderMaterials; ++i)
	{
		//nil the textures
		g_Materials[i].texture0 = NULL;
		g_Materials[i].texture1 = NULL;
		g_Materials[i].texture2 = NULL;
		
		//Zero out uniforms
		g_Materials[i].numUniforms_shared = 0;
		g_Materials[i].numUniforms_unique = 0;
		
		memset(g_Materials[i].uniforms_unique_data, 0, 256);
				
		g_Materials[i].materialName = g_MaterialNames[i];
	}
    
    const AttributeFlags attribs_V = (AttributeFlags) (ATTRIBFLAG_VERTEX);
	const AttributeFlags attribs_VT = (AttributeFlags) (ATTRIBFLAG_VERTEX | ATTRIBFLAG_TEXCOORD);
	const AttributeFlags attribs_VTC = AttributeFlags (ATTRIBFLAG_VERTEX | ATTRIBFLAG_TEXCOORD | ATTRIBFLAG_COLOR);
    const AttributeFlags attribs_VC = (AttributeFlags) (ATTRIBFLAG_VERTEX | ATTRIBFLAG_COLOR);
	//const AttributeFlags attribs_skinned_simple = (AttributeFlags) (ATTRIBFLAG_VERTEX | ATTRIBFLAG_TEXCOORD | ATTRIBFLAG_BONEINDEX | ATTRIBFLAG_BONEWEIGHT);
	
	//-- Create post process materials --

	//PPMT_BlendUsingTexture
	if(CreateShaderProgram(VSH_FullScreenQuad,PP_BlendUsingTexture,attribs_VT,&g_Materials[PPMT_BlendUsingTexture].shaderProgram))
	{
		
	}
	
	//PPMT_PureColor
	if(CreateShaderProgram(VSH_FullScreenQuadNoTexcoord,PP_PureColor,attribs_V,&g_Materials[PPMT_PureColor].shaderProgram))
	{
		AddUniform_Shared(PPMT_PureColor,"color",Uniform_Vec4,(u8*)&m_fadeColor.x,1);
	}
	 
    //PPMT_Copy
	if(CreateShaderProgram(VSH_FullScreenQuad,PS_TextureOnlySimple,attribs_VT,&g_Materials[PPMT_Copy].shaderProgram))
	{
		
	}
    
    //PPMT_Trippin
	if(CreateShaderProgram(VSH_FullScreenQuad,PS_Trippin,attribs_VT,&g_Materials[PPMT_Trippin].shaderProgram))
	{
        AddUniform_Shared(PPMT_Trippin,"accumulatedTime",Uniform_Float,(u8*)&m_accumulatedTime,1);
	}

	
	/*attribute highp   vec3 inVertex;
	 attribute mediump vec3 inNormal;
	 attribute mediump vec3 inTangent;
	 attribute mediump vec3 inBiNormal;
	 attribute mediump vec2 inTexCoord;
	 attribute mediump vec4 inBoneIndex;
	 attribute mediump vec4 inBoneWeights;
	 */
	
   
	
    //MT_VertColors

	if(CreateShaderProgram(VSH_VertColors,PS_Colors,attribs_VC,&g_Materials[MT_VertColors].shaderProgram))
	{
		
	}
	 
	
	//MT_SkinnedWithNormalMapping
    /*if(CreateShaderProgram(VSH_SkinnedVertShader,PS_SkinnedFragShader,attribs_skinned_simple,&g_Materials[MT_SkinnedWithNormalMapping].shaderProgram))
    {
		AddUniform_Shared(MT_SkinnedWithNormalMapping,"lightPos",Uniform_Vec3,(u8*)&m_lightPos,1);
		
		AddUniform_Unique(MT_SkinnedWithNormalMapping,"bUseDot3",Uniform_Int,1);
		AddUniform_Unique(MT_SkinnedWithNormalMapping,"BoneCount",Uniform_Int,1);
		AddUniform_Unique(MT_SkinnedWithNormalMapping,"BoneMatrixArray",Uniform_Mat4x4,8);
		AddUniform_Unique(MT_SkinnedWithNormalMapping,"BoneMatrixArrayIT",Uniform_Mat4x4,8);
    }*/
    
    //MT_VertWithColorInput
    if(CreateShaderProgram(VSH_VertWithColorInput,PS_Colors,attribs_V,&g_Materials[MT_VertWithColorInput].shaderProgram))
    {
        AddUniform_Unique(MT_VertWithColorInput,"inputColor",Uniform_Vec4,1);
    }
	 
	
	//MT_BasicPointSprite
	if(CreateShaderProgram(VSH_PointSprite_Basic,PS_TextureWithColor,attribs_VTC,&g_Materials[MT_BasicPointSprite].shaderProgram))
	{
		g_Materials[MT_BasicPointSprite].texture0 = &texture_pointSpriteAtlas;
	}
	 
	
	//MT_PointSpriteColorShine
	if(CreateShaderProgram(VSH_PointSprite_Basic,PS_PointSprite_ColorShine,attribs_VTC,&g_Materials[MT_PointSpriteColorShine].shaderProgram))
	{
		g_Materials[MT_PointSpriteColorShine].texture0 = &texture_pointSpriteAtlas;
	}
	 
	
	//MT_WiggleUsingTexcoordV
	if(CreateShaderProgram(VSH_WiggleUsingTexcoordV,PS_TextureAndFogColorDiscard,attribs_VT,&g_Materials[MT_WiggleUsingTexcoordV].shaderProgram))
	{
		AddUniform_Unique(MT_WiggleUsingTexcoordV,"inputColor",Uniform_Vec4,1);
		AddUniform_Unique(MT_WiggleUsingTexcoordV,"wiggleAmount",Uniform_Vec4,1);
	}
	 

	//MT_TextureAndVertColor
	if(CreateShaderProgram(VSH_TextureAndVertColor,PS_TextureAndDiffuseColor,attribs_VTC,&g_Materials[MT_TextureAndVertColor].shaderProgram))
	{
		
	}
	 
	
	//MT_TextureOnlySimple
	if(CreateShaderProgram(VSH_TextureOnly,PS_TextureOnlySimple,attribs_VT,&g_Materials[MT_TextureOnlySimple].shaderProgram))
	{
		
	}
	 
    
    //MT_TextureOnlyDiscard
	if(CreateShaderProgram(VSH_TextureOnly,PS_TextureOnlyDiscard,attribs_VT,&g_Materials[MT_TextureOnlyDiscard].shaderProgram))
	{
		
	}
	 

    //MT_TextureAndDiffuseColorDiscard
	if(CreateShaderProgram(VSH_VertWithColorInputAndTexture,PS_TextureAndDiffuseColorDiscard,attribs_VT,&g_Materials[MT_TextureAndDiffuseColorDiscard].shaderProgram))
	{
		AddUniform_Unique(MT_TextureAndDiffuseColorDiscard,"inputColor",Uniform_Vec4,1);
	}
	 
	
	//MT_TextureAndFogColorDiscard
	if(CreateShaderProgram(VSH_VertWithColorInputAndTexture,PS_TextureAndFogColorDiscard,attribs_VT,&g_Materials[MT_TextureAndFogColorDiscard].shaderProgram))
	{
		AddUniform_Unique(MT_TextureAndFogColorDiscard,"inputColor",Uniform_Vec4,1);
	}
	 
	
	//MT_TextureAndFogColor
	if(CreateShaderProgram(VSH_VertWithColorInputAndTexture,PS_TextureAndFogColor,attribs_VT,&g_Materials[MT_TextureAndFogColor].shaderProgram))
	{
		AddUniform_Unique(MT_TextureAndFogColor,"inputColor",Uniform_Vec4,1);
	}
	
	//MT_TextureAndFogColorWithTexcoordOffset
	if(CreateShaderProgram(VSH_VertWithColorInputWithTexcoordOffset,PS_TextureAndFogColor,attribs_VT,&g_Materials[MT_TextureAndFogColorWithTexcoordOffset].shaderProgram))
	{
		AddUniform_Unique(MT_TextureAndFogColorWithTexcoordOffset,"texCoordOffset",Uniform_Vec2,1);
		AddUniform_Unique(MT_TextureAndFogColorWithTexcoordOffset,"inputColor",Uniform_Vec4,1);
	}

    //MT_TextureAndDiffuseColor
	if(CreateShaderProgram(VSH_VertWithColorInputAndTexture,PS_TextureAndDiffuseColor,attribs_VT,&g_Materials[MT_TextureAndDiffuseColor].shaderProgram))
	{
		AddUniform_Unique(MT_TextureAndDiffuseColor,"inputColor",Uniform_Vec4,1);
	}
    
    //MT_TextureAlphaAndDiffuseColor
	if(CreateShaderProgram(VSH_VertWithColorInputAndTexture,PS_TextureAlphaAndDiffuseColor,attribs_VT,&g_Materials[MT_TextureAlphaAndDiffuseColor].shaderProgram))
	{
		AddUniform_Unique(MT_TextureAlphaAndDiffuseColor,"inputColor",Uniform_Vec4,1);
	}
     
	
    //MT_TextureAndDiffuseColorWithTexcoordOffsetDiscard
	if(CreateShaderProgram(VSH_VertWithColorInputWithTexcoordOffset,PS_TextureAndDiffuseColorDiscard,attribs_VT,&g_Materials[MT_TextureAndDiffuseColorWithTexcoordOffsetDiscard].shaderProgram))
	{
		AddUniform_Unique(MT_TextureAndDiffuseColorWithTexcoordOffsetDiscard,"texCoordOffset",Uniform_Vec2,1);
		AddUniform_Unique(MT_TextureAndDiffuseColorWithTexcoordOffsetDiscard,"inputColor",Uniform_Vec4,1);
	}
	 
	
	//MT_TextureAndDiffuseColorWithTexcoordOffset
	if(CreateShaderProgram(VSH_VertWithColorInputWithTexcoordOffset,PS_TextureAndDiffuseColor,attribs_VT,&g_Materials[MT_TextureAndDiffuseColorWithTexcoordOffset].shaderProgram))
	{
		AddUniform_Unique(MT_TextureAndDiffuseColorWithTexcoordOffset,"texCoordOffset",Uniform_Vec2,1);
		AddUniform_Unique(MT_TextureAndDiffuseColorWithTexcoordOffset,"inputColor",Uniform_Vec4,1);
	}
    
    //MT_TextureAlphaAndDiffuseColorWithTexcoordOffset
	if(CreateShaderProgram(VSH_VertWithColorInputWithTexcoordOffset,PS_TextureAlphaAndDiffuseColor,attribs_VT,&g_Materials[MT_TextureAlphaAndDiffuseColorWithTexcoordOffset].shaderProgram))
	{
		AddUniform_Unique(MT_TextureAlphaAndDiffuseColorWithTexcoordOffset,"texCoordOffset",Uniform_Vec2,1);
		AddUniform_Unique(MT_TextureAlphaAndDiffuseColorWithTexcoordOffset,"inputColor",Uniform_Vec4,1);
	}
     
	
    //MT_TextureWithColor
	if(CreateShaderProgram(VSH_TextureWithColor,PS_TextureWithColor,attribs_VTC,&g_Materials[MT_TextureWithColor].shaderProgram))
	{
		
	}
	 
	
	//MT_Animation_NewMethod3D
	if(CreateShaderProgram(VSH_TextureOnlyWithTexcoordAndWorldOffset,PS_TextureOnlySimple,attribs_VT,&g_Materials[MT_Animation_NewMethod3D].shaderProgram))
	{
		AddUniform_Unique(MT_Animation_NewMethod3D,"texCoordOffset",Uniform_Vec2,1);
	}
	 
	
	//MT_TextureOnlyWithTexcoordOffset
	if(CreateShaderProgram(VSH_VertWithTexcoordOffset,PS_TextureOnlySimple,attribs_VT,&g_Materials[MT_TextureOnlyWithTexcoordOffset].shaderProgram))
	{
		AddUniform_Unique(MT_TextureOnlyWithTexcoordOffset,"texCoordOffset",Uniform_Vec2,1);
	}
	
	//MT_WorldSpace_TextureOnly
	if(CreateShaderProgram(VSH_WorldSpace_VertWithTexcoord,PS_TextureOnlySimple,attribs_VT,&g_Materials[MT_WorldSpace_TextureOnly].shaderProgram))
	{
	}
	 
    
    //MT_TextureOnlyWithTexcoordOffsetDiscard
	if(CreateShaderProgram(VSH_VertWithTexcoordOffset,PS_TextureOnlyDiscard,attribs_VT,&g_Materials[MT_TextureOnlyWithTexcoordOffsetDiscard].shaderProgram))
	{
		AddUniform_Unique(MT_TextureOnlyWithTexcoordOffsetDiscard,"texCoordOffset",Uniform_Vec2,1);
	}
	 
	
	//MT_TextureWithScrollingMult
	if(CreateShaderProgram(VSH_VertWithTexcoordAndOffsetTexcoord,PS_MultiplyTwoSamples,attribs_VT,&g_Materials[MT_TextureWithScrollingMult].shaderProgram))
	{
		AddUniform_Unique(MT_TextureWithScrollingMult,"texCoordOffset",Uniform_Vec2,1);
	}
	
	//MT_ScrollingTextureWithScrollingMult
	if(CreateShaderProgram(VSH_VertWithOffsetTexcoordAndOffsetTexcoord,PS_MultiplyTwoSamples,attribs_VT,&g_Materials[MT_ScrollingTextureWithScrollingMult].shaderProgram))
	{
		AddUniform_Unique(MT_ScrollingTextureWithScrollingMult,"texCoordOffset",Uniform_Vec2,1);
		AddUniform_Unique(MT_ScrollingTextureWithScrollingMult,"texCoordOffset2",Uniform_Vec2,1);
	}

	for(int i=0; i<NumRenderMaterials; ++i)
	{
		if(g_Materials[i].shaderProgram == 0)
		{
			COREDEBUG_PrintDebugMessage("Garbage material found: %s",g_MaterialNames[i]);
		}
	}
	
	//Setup common to all materials
	//TODO: Don't do this
	for(int i=0; i<NumRenderMaterials; ++i)
	{
		if(g_Materials[i].shaderProgram == 0)
		{
			continue;
		}
		
		g_Materials[i].uniform_worldViewProjMat = glGetUniformLocation(g_Materials[i].shaderProgram, "worldViewProjMat");
		PrintOpenGLError("Fuu");
		g_Materials[i].uniform_camPos = glGetUniformLocation(g_Materials[i].shaderProgram, "camPos");
		PrintOpenGLError("Fuuuuu");
	}

	PrintOpenGLError("After create material");
}


void OpenGLRenderer::SetViewMatrixForIndex(f32* pCameraMatrix, u32 viewIndex)
{
	m_view[viewIndex] = pCameraMatrix;
}


void OpenGLRenderer::SetViewPos(vec3* pCamPos, u32 viewIndex)
{
	CopyVec3(&m_camPos[viewIndex], pCamPos);
}


f32* OpenGLRenderer::GetCameraMatrix(u32 viewIndex)
{
	return m_view[viewIndex];
}


void OpenGLRenderer::StartFadeOut(f32 timeInSeconds)
{
	m_fadeState = FadeState_FadeOut;
	m_totalFadeTime = timeInSeconds*0.5f;
	m_currFadeTime = 0.0f;
}


void OpenGLRenderer::ForceFadeIn(f32 timeInSeconds)
{
	m_fadeState = FadeState_WaitingForFadeIn;
    m_requestedFadeIn = true;
    
    m_totalFadeTime = timeInSeconds;
	m_currFadeTime = timeInSeconds;
}


void OpenGLRenderer::StartFadeIn()
{
	m_requestedFadeIn = true;
}


void OpenGLRenderer::StartPauseFadeIn()
{
	 m_pauseRequestedFadeIn = true;
}


void OpenGLRenderer::StartPauseFadeOut(f32 finalFade, f32 timeInSeconds)
{
	m_pauseFadeState = FadeState_FadeOut;
	m_pauseTotalFadeTime = timeInSeconds*0.5f;
	m_pauseCurrFadeTime = 0.0f;
    m_pauseFinalFade = 1.0f-finalFade;
}


void OpenGLRenderer::ResetPauseFade()
{
	m_pauseFadeState = FadeState_Idle;
}


void OpenGLRenderer::SetScreenFramebuffer(u32 framebuffer)
{
    m_renderTarget_Screen.frameBuffer = framebuffer;
}

void OpenGLRenderer::SetScreenFadeColor(vec3* screenFadeColor)
{
	CopyVec3(&m_fadeToScreenColor,screenFadeColor);
}

void OpenGLRenderer::ForceRenderablesNeedSorting(RenderableObjectType renderableType)
{
    switch(renderableType)
    {
        case RenderableObjectType_Normal:
        {
            m_renderableObject3DsNeedSorting_Normal = true;
            
            break;
        }
        case RenderableObjectType_UI:
        {
            m_renderableObject3DsNeedSorting_UI = true;
            
            break;
        }
        case RenderableObjectType_Light:
        {
            m_renderableObject3DsNeedSorting_Light = true;
            
            break;
        }
        case RenderableObjectType_All:
        {
            m_renderableObject3DsNeedSorting_Normal = true;
            m_renderableObject3DsNeedSorting_UI = true;
            m_renderableObject3DsNeedSorting_Light = true;
            
            break;
        }
        default:
        {
            break;
        }
    }
}



//Only for normal objects, not UI
void OpenGLRenderer::SortRenderablesWithMaterialByZ(RenderMaterial materialID)
{
	for (u32 i=0; i<g_Factory_Geometry_Normal.m_numObjects; ++i)
	{
		const RenderableGeometry3D* pCurrRenderable = &g_Factory_Geometry_Normal.m_pObjectList[i];
		if (pCurrRenderable->material.materialID == materialID)
		{
			u32 startIDX = i;
			while (i<g_Factory_Geometry_Normal.m_numObjects && pCurrRenderable->material.materialID == materialID)
			{
				++i;
			}
			
			const u32 count = i-startIDX;
			
			g_Factory_Geometry_Normal.Sort(startIDX,count, RenderableGeometry3DCompare_SortByZ);
		}
	}
}


void OpenGLRenderer::SortRenderablesInLayerRangeByZ(RenderLayer layerBegin, RenderLayer layerEnd)
{
	//TODO: make this a little better.  It's a rushed version. :)
	for (u32 i=0; i<g_Factory_Geometry_Normal.m_numObjects; ++i)
	{
		const RenderableGeometry3D* pCurrRenderable = &g_Factory_Geometry_Normal.m_pObjectList[i];
        const bool isInLayer = pCurrRenderable->renderLayer >= layerBegin && pCurrRenderable->renderLayer <= layerEnd;
		if (isInLayer)
		{
			u32 startIDX = i;
            
			while (i<g_Factory_Geometry_Normal.m_numObjects && pCurrRenderable->renderLayer >= layerBegin && pCurrRenderable->renderLayer <= layerEnd)
			{
				++i;
			}
			
			const u32 count = i-startIDX;
			g_Factory_Geometry_Normal.Sort(startIDX,count, RenderableGeometry3DCompare_SortByNegativeZ);
		}
	}
}


void OpenGLRenderer::FlashScreen(const vec3* pColor, f32 timeInSeconds)
{
	CopyVec3(&m_flashColor, pColor);
    m_flashState = FadeState_FadeOut;
	m_totalFlashTime = timeInSeconds*0.5f;
	m_currFlashTime = 0.0f;
}


void OpenGLRenderer::ShakeScreen(f32 shakeAmount,f32 shakeSpeed, f32 shakeTime)
{
	//Shake how we told it to shake
    if (m_screenShakeTimer <= 0.0f)
    {
        m_screenShakeSpeed = shakeSpeed;
		
        m_currScreenShakeAmount = shakeAmount;
        
        m_screenShakeStartTime = shakeTime;
        m_screenShakeTimer = m_screenShakeStartTime;
    }
    //Shake as much as possible
    else
    {
        //If the screen wants to shake harder, use the highest possible shake speed
        if(shakeAmount > m_currScreenShakeAmount)
        {
            m_screenShakeSpeed = MaxF(shakeSpeed,m_screenShakeSpeed);
            
            //Shake is more crazy so use the passed in time
            m_screenShakeStartTime = shakeTime;
        }
        else
        {
            m_screenShakeSpeed = shakeSpeed;
            
            //Shake is less so use the higher of the two times
            m_screenShakeStartTime = MaxF(m_screenShakeStartTime,shakeTime);
        }
        
        m_currScreenShakeAmount = MaxF(m_currScreenShakeAmount,shakeAmount);
        
		
        m_screenShakeTimer = m_screenShakeStartTime;
    }
    
    //TODO: this is a little hardcoded
    const f32 shakeSpeedTFinal = 0.5f+0.5f*m_screenShakeSpeed;
    
    m_screenShakerSpeed_X = shakeSpeedTFinal*50.0f;
    m_screenShakerSpeed_Y = shakeSpeedTFinal*100.0f;
}


/*void OpenGLRenderer::DeleteScene(RenderableScene3D* pScene)
{
	//Delete all scene meshes
	delete[] pScene->pSceneMesh;
	
	//Loop through all the geometry
	for(u32 modelIDX=0; modelIDX<pScene->numModels; ++modelIDX)
	{
		ModelData* pCurrModel = &pScene->pModel[modelIDX];
		
		//Delete the attributes
		delete[] pCurrModel->attributeArray;
		
		//Delete the primitives
		delete[] pCurrModel->primitiveArray;
	}
	
	//Delete geometry
	delete[] pScene->pModel;
	
	//Delete materials
	delete[] pScene->pMaterial;
	
	for(u32 texIDX=0; texIDX<pScene->numTexture; ++texIDX)
	{
		glDeleteTextures(1, &pScene->pTexture[texIDX]);
	}
	
	//Delete texture array
	delete[] pScene->pTexture;
	
	//If there's a pod, destroy it!
	if(pScene->pPod)
	{
		pScene->pPod->Destroy();
	}
}*/

void OpenGLRenderer::CleanUp()
{
	glDeleteTextures(1,&texture_pointSpriteAtlas);
}


bool OpenGLRenderer::LoadTexture(const char* fileName,ImageType imageType, u32* pGLTexture, u32 filterMode, u32 wrapModeU, u32 wrapModeV, bool flipY)
{
	//You're not allowed to load different textures into one location
    //You're not allowed to load into NULL
    if (pGLTexture == NULL)
    {       
        return false;
    }
	else if(*pGLTexture != 0)
	{
		COREDEBUG_PrintDebugMessage("LoadTexture->Warning: Possible unitialized texture handle.  This is fine unless you see incorrect textures.");
		
		return false;
	}

	std::string filePath = GAME->GetPathToFile(fileName);
	
    switch (imageType)
    {
        //case ImageType_PNG:
		case ImageType_TGA:
		{
			*pGLTexture = SOIL_load_OGL_texture
			(
			filePath.c_str(),
			 SOIL_LOAD_AUTO,
			 SOIL_CREATE_NEW_ID,
			 flipY?0:SOIL_FLAG_INVERT_Y
			 );
			
			if(*pGLTexture == 0)
			{
				COREDEBUG_PrintDebugMessage("INSANE ERROR: LoadTexture failed to load the file: %s",fileName);
			}
			
			break;
		}
        case ImageType_PNG:
        {
			int width,height;
			bool hasAlpha;
			GLubyte *textureImage;
			
			const bool success = LoadPNGImage(filePath.c_str(), width, height, hasAlpha, &textureImage, flipY);
			
			if(success)
			{
				//NEW CODE
				glGenTextures(1, pGLTexture);
				//COREDEBUG_PrintMessage("GenTexture: %s id: %d",filePath.c_str(),*pGLTexture);
				glBindTexture(GL_TEXTURE_2D, *pGLTexture);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,filterMode); 
				
				if (filterMode == GL_LINEAR_MIPMAP_LINEAR || filterMode == GL_LINEAR_MIPMAP_NEAREST)
				{
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				}
				else
				{
					glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,filterMode);
				}
				
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapModeU);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapModeV);
				
                //TODO: use RGBA8 for the first type parameter?
				s32 type = hasAlpha?GL_RGBA:GL_RGB;
				
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, textureImage);
				
				//Generate Maps
				if (filterMode == GL_LINEAR_MIPMAP_LINEAR || filterMode == GL_LINEAR_MIPMAP_NEAREST)
				{
					glGenerateMipmap(GL_TEXTURE_2D);
				}
				
				glBindTexture(GL_TEXTURE_2D, 0);
                
                free(textureImage);
			}
			else
			{
				*pGLTexture = 0;
				COREDEBUG_PrintDebugMessage("INSANE ERROR: LoadTexture failed to load the file: %s",fileName);
			}
            
            break;
        }
        case ImageType_PVR:
        {
            //[self loadTextureFromFile:pGLTexture :fileName :filterMode: wrapModeU: wrapModeV];
            
            break;
        }
    }

	if(*pGLTexture)
	{
		return true;
	}
	else
	{
		COREDEBUG_PrintDebugMessage("INSANE ERROR: Failed to load texture: %s!",fileName);

		return false;
	}
}


void OpenGLRenderer::DRAW_DrawTexturedLine(DebugDrawMode drawMode, const vec3* p0, const vec3* p1, const vec4* pDiffuseColor, u32 texturedID, f32 lineWidth0, f32 lineWidth1, f32 texcoordYStart, f32 texcoordYEnd)
{
	if(m_numTexturedLines == MAX_TEXTURED_LINES)
	{
		return;
	}
	
	TexturedLineObject* pNextLine = &m_texturedLineObjects[m_numTexturedLines];
	CopyVec3(&pNextLine->line.p0,p0);
	CopyVec3(&pNextLine->line.p1,p1);
	pNextLine->texturedID = texturedID;
	pNextLine->lineWidth0 = lineWidth0;
	pNextLine->lineWidth1 = lineWidth1;
	pNextLine->texcoordYStart = texcoordYStart;
	pNextLine->texcoordYEnd = texcoordYEnd;
	pNextLine->drawMode = drawMode;
	CopyVec4(&pNextLine->diffuseColor,pDiffuseColor);
	
	++m_numTexturedLines;
}

void OpenGLRenderer::DEBUGDRAW_DrawRay(DebugDrawMode drawMode, const vec3* p0, const vec3* direction, f32 length, const vec4* color)
{
#ifdef DEBUG_DRAW
	//TODO: put safety check in here
	const u32 numPoints = m_numDebugLinePoints[drawMode];
	
	if(numPoints+2 > DEBUGDRAW_MAXLINESPOINTS)
	{
		return;
	}
	
    CopyVec3(&m_debugLinePoints[drawMode][numPoints].position,p0);
    CopyVec4(&m_debugLinePoints[drawMode][numPoints].color,color);
    
	vec3 p1;
	AddScaledVec3(&p1, p0, direction, length);
	
    CopyVec3(&m_debugLinePoints[drawMode][numPoints+1].position,&p1);
    CopyVec4(&m_debugLinePoints[drawMode][numPoints+1].color,color);
    
    m_numDebugLinePoints[drawMode] += 2;
#endif
}


void OpenGLRenderer::DEBUGDRAW_DrawVector(DebugDrawMode drawMode, const vec3* p0, const vec3* pVector, const vec4* color)
{
#ifdef DEBUG_DRAW
	//TODO: put safety check in here
	const u32 numPoints = m_numDebugLinePoints[drawMode];
	
	if(numPoints+2 > DEBUGDRAW_MAXLINESPOINTS)
	{
		return;
	}
	
    CopyVec3(&m_debugLinePoints[drawMode][numPoints].position,p0);
    CopyVec4(&m_debugLinePoints[drawMode][numPoints].color,color);
    
	vec3 p1;
	AddVec3(&p1, p0, pVector);
	
    CopyVec3(&m_debugLinePoints[drawMode][numPoints+1].position,&p1);
    CopyVec4(&m_debugLinePoints[drawMode][numPoints+1].color,color);
    
    m_numDebugLinePoints[drawMode] += 2;
#endif
}

void OpenGLRenderer::DEBUGDRAW_DrawLineSegment(DebugDrawMode drawMode, const vec3* p0, const vec3* p1, const vec4* color)
{
#ifdef DEBUG_DRAW
	//TODO: put safety check in here
	const u32 numPoints = m_numDebugLinePoints[drawMode];
	
	if(numPoints+2 > DEBUGDRAW_MAXLINESPOINTS)
	{
		return;
	}
	
    CopyVec3(&m_debugLinePoints[drawMode][numPoints].position,p0);
    CopyVec4(&m_debugLinePoints[drawMode][numPoints].color,color);
    
    CopyVec3(&m_debugLinePoints[drawMode][numPoints+1].position,p1);
    CopyVec4(&m_debugLinePoints[drawMode][numPoints+1].color,color);
    
    m_numDebugLinePoints[drawMode] += 2;
#endif
}


void OpenGLRenderer::DEBUGDRAW_DrawLineSegment(DebugDrawMode drawMode, const vec3* p0, const vec3* p1, const vec4* color1, const vec4* color2)
{
#ifdef DEBUG_DRAW
	//TODO: put safety check in here
	const u32 numPoints = m_numDebugLinePoints[drawMode];
	
	if(numPoints+2 > DEBUGDRAW_MAXLINESPOINTS)
	{
		return;
	}
	
    CopyVec3(&m_debugLinePoints[drawMode][numPoints].position,p0);
    CopyVec4(&m_debugLinePoints[drawMode][numPoints].color,color1);
    
    CopyVec3(&m_debugLinePoints[drawMode][numPoints+1].position,p1);
    CopyVec4(&m_debugLinePoints[drawMode][numPoints+1].color,color2);
    
    m_numDebugLinePoints[drawMode] += 2;
#endif
}


void OpenGLRenderer::DEBUGDRAW_DrawCircleXY(DebugDrawMode drawMode, mat4f matrix4x4, const vec4* color)
{
#ifdef DEBUG_DRAW
	if(m_numDebugDrawObjects[drawMode] == DEBUGDRAW_MAXDEBUGOBJECTS)
	{
		return;
	}
	
    DebugDrawObject* pObject = &m_debugDrawObjects[drawMode][m_numDebugDrawObjects[drawMode]];
    
    mat4f_Copy(pObject->matrix, matrix4x4);
    CopyVec4(&pObject->color,color);
    pObject->objectType = DebugDrawObjectType_CircleXY;
    
    ++m_numDebugDrawObjects[drawMode];
#endif
}


void OpenGLRenderer::DEBUGDRAW_DrawCircleXZ(DebugDrawMode drawMode, mat4f matrix4x4, const vec4* color)
{
#ifdef DEBUG_DRAW
	if(m_numDebugDrawObjects[drawMode] == DEBUGDRAW_MAXDEBUGOBJECTS)
	{
		return;
	}
	
    DebugDrawObject* pObject = &m_debugDrawObjects[drawMode][m_numDebugDrawObjects[drawMode]];
    
    mat4f_Copy(pObject->matrix, matrix4x4);
    CopyVec4(&pObject->color,color);
    pObject->objectType = DebugDrawObjectType_CircleXZ;
    
    ++m_numDebugDrawObjects[drawMode];
#endif
}


void OpenGLRenderer::DEBUGDRAW_DrawCircleXY(DebugDrawMode drawMode, const vec3* pCenter, f32 radius, const vec4* color)
{
#ifdef DEBUG_DRAW
	mat4f scaleMat;
	mat4f_LoadScale(scaleMat, radius);
	CopyVec3(mat4f_GetPos(scaleMat),pCenter);
	
	DEBUGDRAW_DrawCircleXY(drawMode,scaleMat,color);
#endif
}


void OpenGLRenderer::DEBUGDRAW_DrawCircleXZ(DebugDrawMode drawMode, const vec3* pCenter, f32 radius, const vec4* color)
{
#ifdef DEBUG_DRAW
	mat4f scaleMat;
	mat4f_LoadScale(scaleMat, radius);
	CopyVec3(mat4f_GetPos(scaleMat),pCenter);
	
	DEBUGDRAW_DrawCircleXZ(drawMode,scaleMat,color);
#endif
}


void OpenGLRenderer::DEBUGDRAW_DrawCylinder(DebugDrawMode drawMode, mat4f matrix4x4, const vec4* color)
{
#ifdef DEBUG_DRAW
	if(m_numDebugDrawObjects[drawMode] == DEBUGDRAW_MAXDEBUGOBJECTS)
	{
		return;
	}
	
    DebugDrawObject* pObject = &m_debugDrawObjects[drawMode][m_numDebugDrawObjects[drawMode]];
    
    mat4f_Copy(pObject->matrix, matrix4x4);
    CopyVec4(&pObject->color,color);
    pObject->objectType = DebugDrawObjectType_Cylinder;
    
    ++m_numDebugDrawObjects[drawMode];
#endif
}


void OpenGLRenderer::SetMaterial(RenderMaterial material)
{
	//Enable the shader
	const Material* currMaterial = &g_Materials[material];
	const u32 program = currMaterial->shaderProgram;
	if(program == 0)
	{
		assert(0);
	}
	
	glUseProgram(program);

	//Set the textures
	SetTexture(currMaterial->texture0,0);
    SetTexture(currMaterial->texture1,1);
    SetTexture(currMaterial->texture2,2);
	
	m_lastUsedMaterial = material;
	
	UploadSharedUniforms();
	
#ifdef DEBUG_RENDERLOOP
	//COREDEBUG_PrintDebugMessage("Changing material: %s",g_MaterialNames[material]);
#endif
}


/*void Draw_FSQuad(bool flipped)
{
	// update attribute values
	glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
	glEnableVertexAttribArray(ATTRIB_VERTEX);
    
    glVertexAttribPointer(ATTRIB_TEXCOORD, 2, GL_FLOAT, 0, 0, flipped?squareTexCoord_Flipped:squareTexCoord_Normal);
	glEnableVertexAttribArray(ATTRIB_TEXCOORD);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glDisableVertexAttribArray(ATTRIB_TEXCOORD);
    glDisableVertexAttribArray(ATTRIB_VERTEX);
}


void Draw_TLQuad(bool flipped)
{
	// update attribute values
	glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVerticesTL);
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	
    //Perform post processing by rendering a full screen quad with the given input texture
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glDisableVertexAttribArray(ATTRIB_VERTEX);
}


void Draw_TRQuad(bool flipped)
{
	// update attribute values
	glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVerticesTR);
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	
    //Perform post processing by rendering a full screen quad with the given input texture
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glDisableVertexAttribArray(ATTRIB_VERTEX);
}


void Draw_BLQuad(bool flipped)
{
	// update attribute values
	glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVerticesBL);
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	
    //Perform post processing by rendering a full screen quad with the given input texture
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glDisableVertexAttribArray(ATTRIB_VERTEX);
}


void Draw_BRQuad(bool flipped)
{
	// update attribute values
	glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVerticesBR);
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	
    //Perform post processing by rendering a full screen quad with the given input texture
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glDisableVertexAttribArray(ATTRIB_VERTEX);
}*/


void Draw_Matrix()
{
	// update attribute values
	glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, 0, 0, matrixVertices);
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	
	glVertexAttribPointer(ATTRIB_COLOR, 3, GL_FLOAT, 0, 0, matrixColors);
	glEnableVertexAttribArray(ATTRIB_COLOR);
	
    glDrawArrays(GL_LINES, 0, 6);
}

void OpenGLRenderer::PostProcess(RenderMaterial ppMaterial, RenderTarget* renderTarget, PPDrawArea drawArea, u32* texture0, u32* texture1, u32* texture2)
{
    //SetRenderTarget(renderTarget);
    
	//Set up textures to be used
	g_Materials[ppMaterial].texture0 = texture0;
	g_Materials[ppMaterial].texture1 = texture1;
	g_Materials[ppMaterial].texture2 = texture2;
	
	//Set material like normal
	SetMaterial(ppMaterial);
    UploadProjMatrix(m_projMats[ProjMatType_Orthographic_NDC]);

	//Upload uniforms for this post process
	//TODO: remove this later if there are some that share the same uniforms
	UploadSharedUniforms();
	
    const bool textureIsUsed = texture0 != NULL || texture1 != NULL || texture2 != NULL;
    
    if(textureIsUsed)
    {
        glVertexAttribPointer(ATTRIB_TEXCOORD, 2, GL_FLOAT, 0, 0, m_postProcessFlipper?squareTexCoord_Flipped:squareTexCoord_Normal);
        glEnableVertexAttribArray(ATTRIB_TEXCOORD);
    }
    
	//Draw a full screen quad
	switch (drawArea)
	{
		case PPDrawArea_FullScreen:
		{
			glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, squareVertices);
            glEnableVertexAttribArray(ATTRIB_VERTEX);
            
			break;
		}
		case PPDrawArea_TopLeft:
		{
			//Draw_TLQuad(m_postProcessFlipper);
			break;
		}
		case PPDrawArea_TopRight:
		{
			//Draw_TRQuad(m_postProcessFlipper);
			break;
		}
		case PPDrawArea_BottomLeft:
		{
			//Draw_BLQuad(m_postProcessFlipper);
			break;
		}
		case PPDrawArea_BottomRight:
		{
			//Draw_BRQuad(m_postProcessFlipper);
			break;
		}
		default:
		{
			
		}
	}
    
    m_postProcessFlipper = !m_postProcessFlipper;
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    /*if(textureIsUsed)
    {
        glDisableVertexAttribArray(ATTRIB_TEXCOORD);
    }
    
    glDisableVertexAttribArray(ATTRIB_VERTEX);*/
}


void OpenGLRenderer::PrintOpenGLFrameBufferStatus(bool printSuccess)
{
    const GLenum frameBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    
    switch (frameBufferStatus)
    {
        case GL_FRAMEBUFFER_COMPLETE:
        {
            if(printSuccess)
            {
                 COREDEBUG_PrintDebugMessage("SUCCESS! -> CreateFrameBuffer: Frame buffer created successfully!");
            }
           
            break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        {
            COREDEBUG_PrintDebugMessage("INSANE ERROR! -> CreateFrameBuffer: Frame buffer has incomplete attachment!");
            break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        {
            COREDEBUG_PrintDebugMessage("INSANE ERROR! -> CreateFrameBuffer: Frame buffer missing attachment!");
            break;
        }
#if defined (PLATFORM_IOS)
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
        {
            COREDEBUG_PrintDebugMessage("INSANE ERROR! -> CreateFrameBuffer: Frame buffer has incomplete dimensions!");
            break;
        }
#endif
            
#if defined(PLATFORM_OSX) || defined(PLATFORM_WIN)
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        {
            COREDEBUG_PrintDebugMessage("INSANE ERROR! -> CreateFrameBuffer: Frame buffer has incomplete draw buffer!");
            
            break;
        }
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        {
            COREDEBUG_PrintDebugMessage("INSANE ERROR! -> CreateFrameBuffer: Frame buffer has incomplete read buffer!");
            
            break;
        }
#endif
        case GL_FRAMEBUFFER_UNSUPPORTED:
        {
            COREDEBUG_PrintDebugMessage("INSANE ERROR! -> CreateFrameBuffer: Frame buffer is unsupported!");
            break;
        }
        default:
            break;
    }
}

void OpenGLRenderer::PrintOpenGLError(const char* callerName)
{
	GLenum errCode = glGetError();
	if (errCode != GL_NO_ERROR)
	{
#if defined (PLATFORM_OSX)
		const GLubyte *errString;
	
		errString = gluErrorString(errCode);
		COREDEBUG_PrintDebugMessage ("OpenGL Error: %s", errString);
	
#else
		COREDEBUG_PrintDebugMessage("GL error: %d (0x%4x)", errCode, errCode);
#endif
		
		//assert(0);
	}
}


void OpenGLRenderer::SetViewport(int posX, int posY, int width, int height)
{
	glViewport(posX, posY, width, height);
	
	viewportPosX = posX;
	viewportPosY = posY;
	viewportWidth_pixels = width;
	viewportHeight_pixels = height;
}


void OpenGLRenderer::SetRenderTarget(RenderTarget* pRenderTarget)
{
    if(pRenderTarget == NULL)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_renderTarget_Screen.frameBuffer);
        glViewport(0, 0, m_renderTarget_Screen.width, m_renderTarget_Screen.height);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pRenderTarget->frameBuffer);
        glViewport(0, 0, pRenderTarget->width, pRenderTarget->height);
    }
    
    PrintOpenGLFrameBufferStatus(false);
}


bool OpenGLRenderer::CreateFrameBuffer(u32* pOut_FrameBuffer, u32* pInOut_colorBufferOrTexture, bool createColorBuffer, u32* pInOut_depthBuffer, bool createDepthBuffer, ColorBufferType colorBufferType, u32 width, u32 height)
{
    //Create a new frame buffer
	glGenFramebuffers(1, pOut_FrameBuffer);
    PrintOpenGLError("FrameBufferError");
    
	glBindFramebuffer(GL_FRAMEBUFFER, *pOut_FrameBuffer);
    PrintOpenGLError("FrameBufferError");

    //Probably want a color buffer no matter what
    
    //Create/Attach color buffer
    switch(colorBufferType)
    {
        case ColorBufferType_Normal:
        {
            //Create a color render buffer
            if(createColorBuffer)
            {
                glGenRenderbuffers(1, pInOut_colorBufferOrTexture);
                PrintOpenGLError("FrameBufferError");
                
                glBindRenderbuffer(GL_RENDERBUFFER, *pInOut_colorBufferOrTexture);
                PrintOpenGLError("FrameBufferError");
                
                //TODO: confirm this works
#if defined(PLATFORM_IOS)
                glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8_OES, width, height);
                PrintOpenGLError("FrameBufferError");
#else
                glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);
                PrintOpenGLError("FrameBufferError");
#endif
            }
            
            //Attach it to the frame buffer
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, *pInOut_colorBufferOrTexture);
            PrintOpenGLError("FrameBufferError");
            
            break;
        }
        case ColorBufferType_Texture:
        {
            //Create texture
            if(createColorBuffer)
            {
                //Create a texture to hold the color data
                glGenTextures(1, pInOut_colorBufferOrTexture);
                PrintOpenGLError("FrameBufferError");
                
                glBindTexture(GL_TEXTURE_2D, *pInOut_colorBufferOrTexture);
                PrintOpenGLError("FrameBufferError");
                
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                
                PrintOpenGLError("FrameBufferError");

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
                
                PrintOpenGLError("FrameBufferError");
            }
            
            
            //Attach the texture to the frame buffer
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *pInOut_colorBufferOrTexture, 0);
            PrintOpenGLError("FrameBufferError");
            
            break;
        }
    }
    
    
    //If this is NULL, we don't want a depth buffer at all
    if(pInOut_depthBuffer != NULL)
    {
        //Create a depth buffer
        if(createDepthBuffer == true)
        {
            glGenRenderbuffers(1, pInOut_depthBuffer);
            PrintOpenGLError("FrameBufferError");
            glBindRenderbuffer(GL_RENDERBUFFER, *pInOut_depthBuffer);
            PrintOpenGLError("FrameBufferError");
            
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
            PrintOpenGLError("FrameBufferError");
        }
        
        //Attach the depth buffer to the frame buffer
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *pInOut_depthBuffer);
        PrintOpenGLError("FrameBufferError");
    }
    
    PrintOpenGLFrameBufferStatus(true);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
    PrintOpenGLError("FrameBufferError");
    
    return true;
}


//NOTE: You are responsible for deleting this memory
char* FileToCharArray(const char* filename)
{
    FILE* file = fopen(filename,"r");
    if(file == NULL)
    {
        return NULL;
    }
	
    fseek(file, 0, SEEK_END);
    long int size = ftell(file);
    rewind(file);
	
    char* content = (char*)calloc(size + 1, 1);
	
    fread(content,1,size,file);
	
	fclose(file);
	
    return content;
}


/* Create and compile a shader from the provided source(s) */
bool OpenGLRenderer::CompileShader(u32 *shader, s32 type, s32 count, const char* filename)
{
	s32 status;
   
    const GLchar* source = FileToCharArray(filename);
	
    if (!source)
	{
		COREDEBUG_PrintDebugMessage("ERROR: CompileShader-> Failed to load shader file: %s",filename);
        return false;
    }
    
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);
	
	//Release the source code!
	free((void*)source);

    s32 logLength = 0;
    glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
	{
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(*shader, logLength, &logLength, log);
        COREDEBUG_PrintDebugMessage("Compile log for %s: %s",filename,log);
        free(log);
    }

    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (status == 0)
	{
        glDeleteShader(*shader);
        return false;
    }
    
    return true;
}


/* Link a program with all currently attached shaders */
bool OpenGLRenderer::LinkProgram(u32 prog)
{
	s32 status;
    glLinkProgram(prog);
    
    s32 logLength;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
	{
        GLchar *log = (GLchar *)malloc(logLength);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        COREDEBUG_PrintDebugMessage("Program link log: %s",log);
        free(log);
    }
    
    glGetProgramiv(prog, GL_LINK_STATUS, &status);
    if (status == 0)
	{
        return false;
    }
    
    return true;
}


/* Validate a program (for i.e. inconsistent samplers) */
s32 OpenGLRenderer::ValidateProgram(u32 prog)
{
	s32 logLength, status;
	
	glValidateProgram(prog);
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength+1);
        glGetProgramInfoLog(prog, logLength, &logLength, log);
        COREDEBUG_PrintDebugMessage("Program validate log:%s", log);
        free(log);
    }
    
    glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE)
		COREDEBUG_PrintDebugMessage("Failed to validate program %d", prog);
	
	return status;
}


bool OpenGLRenderer::CreateShaderProgram(s32 vertexShaderIndex, s32 pixelShaderIndex,  AttributeFlags attribs, u32* out_resultProgram)
{
	PrintOpenGLError("Before creating shader program.");
	
	//ALog(@"Creating Shader: VSH:'%@' + FSH:'%@'",g_VertexShader_Filenames[vertexShader],g_PixelShader_Filenames[pixelShader]);

	// create shader program
	const u32 shaderProgram = glCreateProgram();

	if (!shaderProgram)
	{
		return false;
	}

	// create and compile vertex shader
	if (!CreateVertexShader(vertexShaderIndex))
	{
		glDeleteProgram(shaderProgram);
		
		return false;
	}


	// create and compile pixel shader
	if (!CreatePixelShader(pixelShaderIndex))
	{
		glDeleteProgram(shaderProgram);
		
		return false;
	}

	// attach vertex shader to program
	glAttachShader(shaderProgram, m_vertexShaders[vertexShaderIndex].openGLID);

	// attach pixel shader to program
	glAttachShader(shaderProgram, m_pixelShaders[pixelShaderIndex].openGLID);

	// bind attribute locations
	// this needs to be done prior to linking

	PrintOpenGLError("Before binding attributes.");
	
	if (attribs & ATTRIBFLAG_VERTEX)
	{
		glBindAttribLocation(shaderProgram, ATTRIB_VERTEX, "in_position");
	}
	if (attribs & ATTRIBFLAG_VERTEX2)
	{
		glBindAttribLocation(shaderProgram, ATTRIB_VERTEX2, "in_position2");
	}
	if (attribs & ATTRIBFLAG_NORMAL)
	{
		glBindAttribLocation(shaderProgram, ATTRIB_NORMAL, "in_normal");
	}
	if (attribs & ATTRIBFLAG_NORMAL2)
	{
		glBindAttribLocation(shaderProgram, ATTRIB_NORMAL2, "in_normal2");
	}
	if (attribs & ATTRIBFLAG_COLOR)
	{
		glBindAttribLocation(shaderProgram, ATTRIB_COLOR, "in_color");
	}
	if (attribs & ATTRIBFLAG_TEXCOORD)
	{
		glBindAttribLocation(shaderProgram, ATTRIB_TEXCOORD, "in_texcoord");
	}
	if (attribs & ATTRIBFLAG_TEXCOORD2)
	{
		glBindAttribLocation(shaderProgram, ATTRIB_TEXCOORD2, "in_texcoord2");
	}
	if (attribs & ATTRIBFLAG_TANGENT)
	{
		glBindAttribLocation(shaderProgram, ATTRIB_TANGENT, "in_tangent");
	}
	if (attribs & ATTRIBFLAG_BINORMAL)
	{
		glBindAttribLocation(shaderProgram, ATTRIB_BINORMAL, "in_binormal");
	}
	if (attribs & ATTRIBFLAG_BONEINDEX)
	{
		glBindAttribLocation(shaderProgram, ATTRIB_BONEINDEX, "in_boneindex");
	}
	if (attribs & ATTRIBFLAG_BONEWEIGHT)
	{
		glBindAttribLocation(shaderProgram, ATTRIB_BONEWEIGHT, "in_boneweight");
	}
	

	PrintOpenGLError("After binding attributes.");

	// link program
	if (!LinkProgram(shaderProgram))
	{	
		s32 logLength;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0)
		{
			char* log = (GLchar *)malloc(logLength);
			glGetProgramInfoLog(shaderProgram, logLength, &logLength, log);
			COREDEBUG_PrintDebugMessage("Reason: %s", log);
			free(log);
		}
		
		return false;
	}

	if (out_resultProgram != NULL)
	{
		*out_resultProgram = shaderProgram;
	}

	PrintOpenGLError("Before texture uniforms.");
	
	glUseProgram(shaderProgram);
	
	//TODO: make this only set up as many as the material uses (if it even matters)
	u32 uniform_texture0 = glGetUniformLocation(shaderProgram,"texture0");
	u32 uniform_texture1 = glGetUniformLocation(shaderProgram,"texture1");
	u32 uniform_texture2 = glGetUniformLocation(shaderProgram,"texture2");

	glUniform1i(uniform_texture0,0);
	glUniform1i(uniform_texture1,1);
	glUniform1i(uniform_texture2,2);

	PrintOpenGLError("After creating shader program.");
	
	return true;
}


void OpenGLRenderer::UploadWorldViewProjMatrix(const f32* pWorldMat)
{
	f32 worldViewProj[16];
	mat4f_Multiply(worldViewProj, m_viewProjMats[m_currViewIndex][m_currProjMatType], pWorldMat);
	
	const Material* currMaterial = &g_Materials[m_lastUsedMaterial];
	glUniformMatrix4fv(currMaterial->uniform_worldViewProjMat, 1, GL_FALSE, worldViewProj);
} //only call after setMaterial


void OpenGLRenderer::UploadProjMatrix(const f32* pProjMat)
{
	const Material* currMaterial = &g_Materials[m_lastUsedMaterial];
	glUniformMatrix4fv(currMaterial->uniform_worldViewProjMat, 1, GL_FALSE, pProjMat);
} //only call after setMaterial


void OpenGLRenderer::UploadWorldProjMatrix(const f32* pWorldMat)
{
	f32 viewProj[16];
	mat4f_Multiply(viewProj, m_projMats[m_currProjMatType], pWorldMat);
	
	const Material* currMaterial = &g_Materials[m_lastUsedMaterial];
	glUniformMatrix4fv(currMaterial->uniform_worldViewProjMat, 1, GL_FALSE, viewProj);
} //only call after setMaterial


void OpenGLRenderer::UploadSharedUniforms()
{
	const Material* currMaterial = &g_Materials[m_lastUsedMaterial];
	
	const int numUniforms = currMaterial->numUniforms_shared;
	
	for(int i=0; i<numUniforms; ++i)
	{
		const UniformBlob* currUniform = &currMaterial->uniforms_shared[i];
		
		switch(currUniform->value.type)
		{
			case Uniform_Int:
			{
				glUniform1iv(currMaterial->uniforms_shared[i].uniform,currUniform->size,(s32*)currUniform->value.data);
				break;
			}
			case Uniform_Float:
			{
				glUniform1fv(currMaterial->uniforms_shared[i].uniform,currUniform->size,(f32*)currUniform->value.data);
				break;
			}
			case Uniform_Vec2:
			{
				glUniform2fv(currMaterial->uniforms_shared[i].uniform,currUniform->size,(f32*)currUniform->value.data);
				break;
			}
			case Uniform_Vec3:
			{
				glUniform3fv(currMaterial->uniforms_shared[i].uniform,currUniform->size,(f32*)currUniform->value.data);
				break;
			}
			case Uniform_Vec4:
			{
				glUniform4fv(currMaterial->uniforms_shared[i].uniform,currUniform->size,(f32*)currUniform->value.data);
				break;
			}
            case Uniform_Mat3x3:
			{
				glUniformMatrix3fv(currMaterial->uniforms_shared[i].uniform, currUniform->size, GL_FALSE, (f32*)currUniform->value.data);
				break;
			}
			case Uniform_Mat4x4:
			{
				glUniformMatrix4fv(currMaterial->uniforms_shared[i].uniform, currUniform->size, GL_FALSE, (f32*)currUniform->value.data);
				break;
			}
			default:
			{
				break;
			}
		}
		
		
	}
}


void OpenGLRenderer::UploadUniqueUniforms(u8* const * pValuePointerArray)
{
	Material* currMaterial = &g_Materials[m_lastUsedMaterial];
	
	const int numUniforms = currMaterial->numUniforms_unique;
	
	u32 byteIndex = 0;
	
	for(int i=0; i<numUniforms; ++i)
	{
		const u8* pCurrValue = pValuePointerArray[i];
		
		const s32 uniformID = currMaterial->uniforms_unique[i];
		
		if (uniformID == -1 || pCurrValue == NULL)
		{
			continue;
		}
		
		const UniformType type = currMaterial->uniforms_unique_types[i];
		
		const s32 sizeOfData = g_UniformSizes[type];
		
#if defined (_DEBUG)
		assert(byteIndex + sizeOfData < 256);	//Why are you uploading like 256 bytes of uniforms?
		//Seriously that's like 4, 4x4 matrices
#endif
		void* pCachedData = (void*)&currMaterial->uniforms_unique_data[byteIndex];
		
		if(memcmp(pCurrValue, pCachedData, sizeOfData) == 0)
		{
			continue;
		}
		
		//Cache values to check against later
		memcpy(pCachedData, pCurrValue, sizeOfData);
		byteIndex += sizeOfData;
		
		const GLsizei uniformCount = currMaterial->uniforms_unique_sizes[i];
		switch(type)
		{
			case Uniform_Int:
			{
				glUniform1iv(uniformID,uniformCount,(s32*)pCachedData);
				break;
			}
			case Uniform_Float:
			{
				glUniform1fv(uniformID,uniformCount,(f32*)pCachedData);
				break;
			}
			case Uniform_Vec2:
			{
				glUniform2fv(uniformID,uniformCount,(f32*)pCachedData);
				break;
			}
			case Uniform_Vec3:
			{
				glUniform3fv(uniformID,uniformCount,(f32*)pCachedData);
				break;
			}
			case Uniform_Vec4:
			{
				glUniform4fv(uniformID,uniformCount,(f32*)pCachedData);
				break;
			}
            case Uniform_Mat3x3:
			{
				glUniformMatrix3fv(uniformID, uniformCount, GL_FALSE, (f32*)pCachedData);
				break;
			}
			case Uniform_Mat4x4:
			{
				glUniformMatrix4fv(uniformID, uniformCount, GL_FALSE, (f32*)pCachedData);
				break;
			}
			default:
			{
				break;
			}
		}
	}
}


void OpenGLRenderer::SetTexture(const u32* pTexture,u32 textureUnit)
{
	//If the texture is NULL or the deferenced texture is 0, do nothing
    //else use the deferenced texture
    u32 textureToUse = (pTexture && *pTexture) ? *pTexture : 0;

	//assert(textureToUse);
	
    if (textureToUse && m_currTextureInTextureUnit[textureUnit] != textureToUse)
    {
        m_currTextureInTextureUnit[textureUnit] = textureToUse;
        
        if(m_currTextureUnit != textureUnit)
        {
            m_currTextureUnit = textureUnit;
            glActiveTexture(GL_TEXTURE0+textureUnit);
			
        }
        
		//COREDEBUG_PrintDebugMessage("Using Texture: %d",textureToUse);
		
        glBindTexture(GL_TEXTURE_2D, textureToUse);
		
    }
}


bool OpenGLRenderer::CreatePixelShader(s32 pixelShaderIndex)
{
	ShaderCreationStatus* pStatus = &m_pixelShaders[pixelShaderIndex];
	
	if(pStatus->openGLID)
	{
		return true;
	}
	
	std::string filePath =  GAME->GetPathToFile(pStatus->filename, true);

	if (!CompileShader(&pStatus->openGLID, GL_FRAGMENT_SHADER, 1, filePath.c_str()))
	{
		glDeleteShader(pStatus->openGLID);
		pStatus->openGLID = 0;
		
		return false;
	}

	return true;
}


bool OpenGLRenderer::CreateVertexShader(s32 vertexShaderIndex)
{
	ShaderCreationStatus* pStatus = &m_vertexShaders[vertexShaderIndex];
	
	if(pStatus->openGLID)
	{
		return true;
	}

	std::string filePath = GAME->GetPathToFile(pStatus->filename, true);
	
	if (!CompileShader(&pStatus->openGLID, GL_VERTEX_SHADER, 1, filePath.c_str()))
	{
		glDeleteShader(pStatus->openGLID);
		pStatus->openGLID = 0;
		
		return false;
	}
	
	return true;
}

f32 OpenGLRenderer::ComputeGaussianValue(f32 x, f32 stdDevSq)
{
	// The gaussian equation is defined as such:
	/*    
	 |-(x - mean)^2|
	 |-------------|
	 |		  1.0		  |		| 2*std_dev^2 |
	 f(x,mean,std_dev) = |--------------------| * e^
	 |sqrt(2*pi*std_dev^2)|
	 
	 */
	
	return exp(-(x*x)/(2.0f*stdDevSq))/sqrtf(2.0f*PI*stdDevSq);
}


void OpenGLRenderer::ComputeGaussianWeights(f32* out_pWeights, s32 numWeights, f32 standardDeviationSquared)
{
	const float numWeightsMin1 = (float)(numWeights-1);
	
	float sum = 0.0f;
	for (int i=0;i<numWeights; i++)
	{
		float x = (2.0f * (float)i - numWeightsMin1) / numWeightsMin1;
		out_pWeights[i] = ComputeGaussianValue(x,standardDeviationSquared);
		sum += out_pWeights[i];
	}
	
	for(int i=0; i<numWeights; i++)
	{
		out_pWeights[i] /= sum;
	}
}

void OpenGLRenderer::EnableAttribute(const AttributeData* pAttrib, u32 stride)
{
	glEnableVertexAttribArray(pAttrib->attribute);
	
	PrintOpenGLError("After glEnableVertexAttribArray");
	
	bool isNormalized = pAttrib->attribute == ATTRIB_COLOR ? true:false;
	glVertexAttribPointer(pAttrib->attribute, pAttrib->size, pAttrib->type, isNormalized, stride, BUFFER_OFFSET(pAttrib->offset));
	
	PrintOpenGLError("After glVertexAttribPointer");
}


void OpenGLRenderer::EnableAttributes(const ModelData* pModelData)
{
	const u32 stride = pModelData->stride;
	
	for(u32 i=0; i<pModelData->numAttributes; ++i)
	{
		const AttributeData* pAttrib = &pModelData->attributeArray[i];
		EnableAttribute(pAttrib,stride);
	}
		
    PrintOpenGLError("After enabling attributes");
}


void OpenGLRenderer::BindVertexArrayObject(u32 vao)
{
#ifdef PLATFORM_IOS
	glBindVertexArrayOES(vao);
#elif defined PLATFORM_WIN
	glBindVertexArray(vao);
#else
	glBindVertexArrayAPPLE(vao);
#endif
	
}


void OpenGLRenderer::BindIndexData(const PrimitiveData* pPrimitive)
{
	//If it's NULL, it won't use indices
	if (pPrimitive->indexData != NULL)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pPrimitive->indexBufferID);
	}
}


void OpenGLRenderer::AddUniform_Unique(RenderMaterial renderMaterial, const char* nameOfUniformInShader,UniformType uniformType, u32 amount)
{
	const int index = g_Materials[renderMaterial].numUniforms_unique;
	const s32 uniformID = glGetUniformLocation(g_Materials[renderMaterial].shaderProgram,nameOfUniformInShader);
	
	if (uniformID == -1)
	{
		COREDEBUG_PrintDebugMessage("ERROR: addUniform_Unique -> Unable to add uniform: %s for material %s!", nameOfUniformInShader,g_MaterialNames[renderMaterial]);
	}
	
	g_Materials[renderMaterial].uniforms_unique[index] = uniformID;
	g_Materials[renderMaterial].uniforms_unique_sizes[index] = amount;
	g_Materials[renderMaterial].uniforms_unique_types[index] = uniformType;
	++g_Materials[renderMaterial].numUniforms_unique;
}


void OpenGLRenderer::AddUniform_Shared(RenderMaterial renderMaterial, const char* nameOfUniformInShader, UniformType uniformType, u8* pData, u32 amount)
{
	const int index = g_Materials[renderMaterial].numUniforms_shared;
	g_Materials[renderMaterial].uniforms_shared[index].uniform = glGetUniformLocation(g_Materials[renderMaterial].shaderProgram,nameOfUniformInShader);
	
	if (g_Materials[renderMaterial].uniforms_shared[index].uniform == -1)
	{
		COREDEBUG_PrintDebugMessage("ERROR: addUniform_Shared -> Unable to add uniform: %s for material %s!", nameOfUniformInShader,g_MaterialNames[renderMaterial]);
		
		//Return early and don't add this uniform
		return;
	}
	
	g_Materials[renderMaterial].uniforms_shared[index].value.type = uniformType;
	g_Materials[renderMaterial].uniforms_shared[index].value.data = pData;
	g_Materials[renderMaterial].uniforms_shared[index].size = amount;
	++g_Materials[renderMaterial].numUniforms_shared;
}


/*CPVRTModelPOD* OpenGLRenderer::LoadPOD(const char* fileName)
{
	CPVRTModelPOD* newPod = new CPVRTModelPOD;
	
	EPVRTError readResult = newPod->ReadFromFile(GAME->GetPathToFile(fileName).c_str());
	
	if(readResult == PVR_SUCCESS)
	{
		return newPod;
	}
	
	delete newPod;
	
	return NULL;
}



CoreObjectHandle OpenGLRenderer::CreateRenderableSceneObject3D(RenderableSceneObject3D** pOut_SceneObject)
{
	RenderableSceneObject3D* pScene = NULL;
	
	//UI
	pScene = g_Factory_RenderableSceneObject.CreateObject(0);
	
	if(pScene == NULL)
	{
		COREDEBUG_PrintDebugMessage("INSANE ERROR: You're out of RenderableSceneObject3Ds!");
		return CoreObjectHandle();
	}

	const CoreObjectHandle handle = pScene->GetHandle();
	
	if(handle.IsValid() == false)
	{
		return CoreObjectHandle();
	}
	
	pScene->material.flags = 0;
	
	if(pOut_SceneObject != NULL)
	{
		*pOut_SceneObject = pScene;
	}
	
	//COREDEBUG_PrintDebugMessage("Created a RenderableSceneObject3D handle: %d!  Count: %d",pScene->GetHandle(),m_numRenderableScenes);
	
	return handle;
}


AnimatedPOD* OpenGLRenderer::AddAnimatedPOD(CPVRTModelPOD* pPod, RenderableScene3D* pScene, mat4f matrix4x4)
{
	if(m_numAnimatedPods == MAX_ANIMATED_PODS)
	{
		COREDEBUG_PrintDebugMessage("Insane Error: AddAnimatedPOD -> Reached max animated pods!  Added nothing.");
		return NULL;
	}
	
	AnimatedPOD* pAnimatedPod = &m_animatedPODs[m_numAnimatedPods];
	pAnimatedPod->pPod = pPod;
	pAnimatedPod->frameRate = static_cast<f32>(pPod->nFPS);
	pAnimatedPod->currFrame = 0.0f;
	pAnimatedPod->pScene = pScene;
	
	if(matrix4x4 == NULL)
	{
		mat4f_LoadIdentity(pAnimatedPod->worldMat);
	}
	else
	{
		mat4f_Copy(pAnimatedPod->worldMat,matrix4x4);
	}
	
	++m_numAnimatedPods;
	
	return pAnimatedPod;
}


void AnimatedPOD::Update(f32 timeElapsed)
{
	currFrame += timeElapsed*frameRate;
	
	const s32 maxFrame = pPod->nNumFrame - 1;
	
	if(currFrame > maxFrame)
	{
		currFrame -= maxFrame;
	}
}



void OpenGLRenderer::DrawSceneObject(RenderableSceneObject3D* pSceneObject)
{
	//bool hasUploadedWorldMat = false;
	
	const RenderableMaterial* pMaterialOverride = pSceneObject->pMaterialOverride;
	if(pMaterialOverride != NULL)
	{
		SetMaterial(pMaterialOverride->materialID);
		SetTexture(pMaterialOverride->customTexture0, 0);
		const u32 renderFlags = pMaterialOverride->flags;
		SetRenderState(BlendMode_Normal,renderFlags);
		
		//Upload uniforms that have unique values per object
		UploadUniqueUniforms(pMaterialOverride->uniqueUniformValues);
	}
	
	f32 worldMat[16];
	
	// To draw a scene, you must go through all the MeshNodes and draw the referenced meshes.
	for (unsigned int meshIDX = 0; meshIDX < pSceneObject->pScene->numSceneMeshes; ++meshIDX)
	{
		SceneMesh* pCurrSceneMesh = &pSceneObject->pScene->pSceneMesh[meshIDX];
		ModelData* pCurrModel = pCurrSceneMesh->pModelData;
		RenderableMaterial* pCurrMaterial = pCurrSceneMesh->pMaterial;

		//Get scene from 
		PrimitiveData* pCurrPrim = &pCurrModel->primitiveArray[0];
		
		if (m_supportsVAOs)
		{
			BindVertexArrayObject(pCurrPrim->vertexArrayObjectID);
		}
		else
		{
			glBindBuffer(GL_ARRAY_BUFFER, pCurrPrim->vertexArrayObjectID);
			EnableAttributes(pCurrModel);
		}
		
		if(pMaterialOverride == NULL
		   && pCurrMaterial != NULL)
		{
			//Set material	
			RenderMaterial nextMaterial;
			if(pCurrModel->attribFlags & ATTRIBFLAG_BONEWEIGHT)
			{
				//nextMaterial = MT_SkinnedWithNormalMapping;
			}
			else if(pCurrModel->attribFlags & ATTRIBFLAG_COLOR)
			{
				//nextMaterial = MT_TextureOnlySimple;
				nextMaterial = MT_TextureAndVertColor;
			}
			else
			{
				nextMaterial = MT_TextureOnlySimple;
			}
			
			if(nextMaterial != m_lastUsedMaterial)
			{
				SetMaterial(nextMaterial);
			}
			
			//Override material texture0 if needed
			//Will do nothing if the texture is set to 0 or the texture is already set
			SetTexture(pCurrMaterial->customTexture0, 0);
			
			const u32 renderFlags = pCurrMaterial->flags;
			SetRenderState(BlendMode_Normal,renderFlags);
		}

		mat4f_Multiply(worldMat, pSceneObject->worldMat, pCurrSceneMesh->worldMat);
		if(pSceneObject->pParentMat != NULL)
		{
			mat4f_Multiply_Self_RHS(worldMat, *pSceneObject->pParentMat);
		}
		
		UploadWorldViewProjMatrix(worldMat);

		//Draw the primitive!
		if(pCurrPrim->vertexArrayObjectID)
		{
			//NULL means non-indexed triangles
			if (pCurrPrim->indexData == NULL)
			{
				glDrawArrays(pCurrPrim->drawMethod, 0, pCurrPrim->numVerts);
			}
			//Render using indices
			else
			{
				//VAOs don't save this I guess?
				BindIndexData(pCurrPrim);
				
				glDrawElements(pCurrPrim->drawMethod, pCurrPrim->numVerts, GL_UNSIGNED_SHORT, 0);
			}
		}
	}
}*/


void OpenGLRenderer::DeleteTexture(u32* pTextureID)
{
	if(*pTextureID != 0)
    {
        glDeleteTextures(1,pTextureID);
        *pTextureID = 0;
    }
}


/*void OpenGLRenderer::DrawAnimatedPOD(AnimatedPOD* pAnimatedPod)
{
	bool hasUploadedWorldMat = false;
	
	CPVRTModelPOD* pPod = pAnimatedPod->pPod;
	
	pPod->SetFrame(pAnimatedPod->currFrame);
	
	RenderableScene3D* pScene = pAnimatedPod->pScene;
	
	bool useDot3 = false;

	// To draw a scene, you must go through all the MeshNodes and draw the referenced meshes.
	for (unsigned int meshIDX = 0; meshIDX < pScene->numSceneMeshes; ++meshIDX)
	{
		SceneMesh* pCurrSceneMesh = &pScene->pSceneMesh[meshIDX];
		ModelData* pCurrModel = pCurrSceneMesh->pModelData;
		RenderableMaterial* pCurrMaterial = pCurrSceneMesh->pMaterial;
		
		//Get scene from 
		pCurrMaterial->uniqueUniformValues[0] = (u8*)&useDot3;
		
		PrimitiveData* pCurrPrim = &pCurrModel->primitiveArray[0];
		
		if (m_supportsVAOs)
		{
			BindVertexArrayObject(pCurrPrim->vertexArrayObjectID);
		}
		else
		{
			glBindBuffer(GL_ARRAY_BUFFER, pCurrPrim->vertexArrayObjectID);
			EnableAttributes(pCurrModel);
		}
		
		//Set material	
		RenderMaterial nextMaterial = pCurrMaterial->materialID;
		if(nextMaterial != m_lastUsedMaterial)
		{
			SetMaterial(nextMaterial);

			UploadWorldViewProjMatrix(pAnimatedPod->worldMat);
		}
		else if(!hasUploadedWorldMat)
		{
			hasUploadedWorldMat = true;
			
			UploadWorldViewProjMatrix(pAnimatedPod->worldMat);
		}
		
		//Override material texture0 if needed
		//Will do nothing if the texture is set to 0 or the texture is already set
		SetTexture(pCurrMaterial->customTexture0, 0);
		
		const u32 renderFlags = pCurrMaterial->flags;
		SetRenderState(BlendMode_Normal,renderFlags);
		
		SPODNode& Node = pPod->pNode[meshIDX];
		
		// Get the mesh data from the POD file
		int i32MeshIndex = pPod->pNode[meshIDX].nIdx;
		
		SPODMesh& Mesh = pPod->pMesh[i32MeshIndex];
		
		for(int i32Batch = 0; i32Batch < Mesh.sBoneBatches.nBatchCnt; ++i32Batch)
		{
			// Set the number of bones that will influence each vertex in the mesh
			pCurrMaterial->uniqueUniformValues[1] = (u8*)&Mesh.sBoneIdx.n;
			//COREDEBUG_PrintDebugMessage("Num bones: %d",Mesh.sBoneIdx.n);
			
			
			 //If the current mesh has bone index and weight data then we need to
			 //set up some additional variables in the shaders.
			 
			
			// Go through the bones for the current bone batch
			PVRTMat4 amBoneWorld[8];
			PVRTMat3 afBoneWorldIT[8];
			
			int i32Count = Mesh.sBoneBatches.pnBatchBoneCnt[i32Batch];
			
			for(int i = 0; i < i32Count; ++i)
			{
				// Get the Node of the bone
				int i32NodeID = Mesh.sBoneBatches.pnBatches[i32Batch * Mesh.sBoneBatches.nBatchBoneMax + i];
				
				SPODNode& nodeBone = pPod->pNode[i32NodeID];
				// Get the World transformation matrix for this bone and combine it with our app defined
				// transformation matrix
				amBoneWorld[i] = pPod->GetBoneWorldMatrix(Node, nodeBone);
				
				// Calculate the inverse transpose of the 3x3 rotation/scale part for correct lighting
				afBoneWorldIT[i] = PVRTMat3(amBoneWorld[i]).inverse().transpose();
			}
			
			//Draw the primitive!
			
			
			 //As we are using bone batching we don't want to draw all the faces contained within pMesh, we only want
			 //to draw the ones that are in the current batch. To do this we pass to the drawMesh function the offset
			 //to the start of the current batch of triangles (Mesh.sBoneBatches.pnBatchOffset[i32Batch]) and the
			 //total number of triangles to draw (i32Tris)
			 
			
			pCurrMaterial->uniqueUniformValues[2] = (u8*)amBoneWorld[0].ptr();
			pCurrMaterial->uniqueUniformValues[3] = (u8*)afBoneWorldIT[0].ptr();
			
			//Upload uniforms that have unique values per object
			UploadUniqueUniforms(pCurrMaterial->uniqueUniformValues);

			int i32Tris;
			if(i32Batch+1 < Mesh.sBoneBatches.nBatchCnt)
				i32Tris = Mesh.sBoneBatches.pnBatchOffset[i32Batch+1] - Mesh.sBoneBatches.pnBatchOffset[i32Batch];
			else
				i32Tris = Mesh.nNumFaces - Mesh.sBoneBatches.pnBatchOffset[i32Batch];
			
			//VAOs don't save this I guess?
			BindIndexData(pCurrPrim);
			
			//PrintOpenGLError("Tried to render something");
			const u32 batchOffset = Mesh.sBoneBatches.pnBatchOffset[i32Batch]*3*sizeof(u16);
			//COREDEBUG_PrintDebugMessage("Batch offset: %d, Tris: %d",batchOffset,i32Tris * 3);
			glDrawElements(pCurrPrim->drawMethod, i32Tris * 3, GL_UNSIGNED_SHORT, BUFFER_OFFSET(batchOffset));
		}
	}
}*/


bool OpenGLRenderer::GetSupportsVAOs()
{
	return m_supportsVAOs;
}


/*bool OpenGLRenderer::InitSceneFromPOD(RenderableScene3D* pScene, CPVRTModelPOD* pPod, u32 viewFlags, const char* relativePath)
{
	const size_t pathLen = relativePath ? strlen(relativePath):0;
	
	pScene->pPod = pPod;

	if(pPod)
	{
		pScene->numSceneMeshes = pPod->nNumMeshNode;
		if(pScene->numSceneMeshes > 0)
		{
			pScene->pSceneMesh = new SceneMesh[pPod->nNumMeshNode];
		}
		else
		{
			pScene->pSceneMesh = NULL;
		}

		pScene->numModels = pPod->nNumMesh;
		if(pScene->numModels > 0)
		{
			pScene->pModel = new ModelData[pPod->nNumMesh];
		}
		else
		{
			pScene->pModel = NULL;
		}
		
		pScene->numTexture = pPod->nNumTexture;
		if(pScene->numTexture > 0)
		{
			pScene->pTexture = new u32[pPod->nNumTexture];
		}
		else
		{
			pScene->pTexture = NULL;
		}
		
		pScene->numMaterial = pPod->nNumMaterial;
		if(pScene->numMaterial > 0)
		{
			pScene->pMaterial = new RenderableMaterial[pPod->nNumMaterial];
		}
		else
		{
			pScene->pMaterial = NULL;
		}

		//Load up all the textures
		for(u32 texIDX = 0; texIDX < pPod->nNumTexture; ++texIDX)
		{
			SPODTexture* pCurrTexture = &pPod->pTexture[texIDX];
			
			char nameBuffer[512];
			if(pathLen > 0)
			{
				strcpy(nameBuffer, relativePath);
			}

			const size_t nameLen = strlen(pCurrTexture->pszName);
			const size_t totalLen = pathLen+nameLen;
			
			strcpy(&nameBuffer[pathLen], pCurrTexture->pszName);
			
			nameBuffer[totalLen-1] = 'g';
			nameBuffer[totalLen-2] = 'n';
			nameBuffer[totalLen-3] = 'p';
			
			LoadTexture(nameBuffer, ImageType_PNG, &pScene->pTexture[texIDX], GL_LINEAR_MIPMAP_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
			COREDEBUG_PrintDebugMessage("Texture %d: %s",texIDX,nameBuffer);
		}
		
		//Load up all the materials
		for(u32 matIDX = 0; matIDX < pPod->nNumMaterial; ++matIDX)
		{
			COREDEBUG_PrintDebugMessage("---- Material %d ----",matIDX);
			
			SPODMaterial* pCurrMaterial = &pPod->pMaterial[matIDX];
			RenderableMaterial* pSceneMaterial = &pScene->pMaterial[matIDX];
			
			pSceneMaterial->customTexture0 = 0;
			pSceneMaterial->customTexture1 = 0;
			
			if(pCurrMaterial->nIdxTexDiffuse != -1)
			{
				pSceneMaterial->customTexture0 = &pScene->pTexture[pCurrMaterial->nIdxTexDiffuse];
				
				SPODTexture* pCurrTexture = &pPod->pTexture[pCurrMaterial->nIdxTexDiffuse];
				COREDEBUG_PrintDebugMessage("Diffuse: %s", pCurrTexture->pszName);
			}
			
			if(pCurrMaterial->nIdxTexAmbient != -1)
			{
				SPODTexture* pCurrTexture = &pPod->pTexture[pCurrMaterial->nIdxTexAmbient];
				COREDEBUG_PrintDebugMessage("Ambient: %s", pCurrTexture->pszName);
			}
			
			if(pCurrMaterial->nIdxTexSpecularColour != -1)
			{
				SPODTexture* pCurrTexture = &pPod->pTexture[pCurrMaterial->nIdxTexSpecularColour];
				COREDEBUG_PrintDebugMessage("Specular: %s", pCurrTexture->pszName);
			}
			
			if(pCurrMaterial->nIdxTexBump != -1)
			{
				pSceneMaterial->customTexture1 = &pScene->pTexture[pCurrMaterial->nIdxTexBump];

				SPODTexture* pCurrTexture = &pPod->pTexture[pCurrMaterial->nIdxTexBump];
				COREDEBUG_PrintDebugMessage("Bump: %s", pCurrTexture->pszName);
			}
			
			if(pCurrMaterial->nIdxTexEmissive != -1)
			{
				SPODTexture* pCurrTexture = &pPod->pTexture[pCurrMaterial->nIdxTexEmissive];
				COREDEBUG_PrintDebugMessage("Emissive: %s", pCurrTexture->pszName);
			}
			
			if(pCurrMaterial->nIdxTexGlossiness != -1)
			{
				SPODTexture* pCurrTexture = &pPod->pTexture[pCurrMaterial->nIdxTexGlossiness];
				COREDEBUG_PrintDebugMessage("Glossiness: %s", pCurrTexture->pszName);
			}
			
			if(pCurrMaterial->nIdxTexOpacity != -1)
			{
				SPODTexture* pCurrTexture = &pPod->pTexture[pCurrMaterial->nIdxTexOpacity];
				COREDEBUG_PrintDebugMessage("Opacity: %s", pCurrTexture->pszName);
			}
			
			if(pCurrMaterial->nIdxTexReflection != -1)
			{
				SPODTexture* pCurrTexture = &pPod->pTexture[pCurrMaterial->nIdxTexReflection];
				COREDEBUG_PrintDebugMessage("Reflection: %s", pCurrTexture->pszName);
			}
			
			if(pCurrMaterial->nIdxTexRefraction != -1)
			{
				SPODTexture* pCurrTexture = &pPod->pTexture[pCurrMaterial->nIdxTexRefraction];
				COREDEBUG_PrintDebugMessage("Refraction: %s", pCurrTexture->pszName);
			}

			COREDEBUG_PrintDebugMessage("");
			
			pSceneMaterial->flags = RenderFlagDefaults_3DCard_NoAlpha|RenderFlag_Visible|RenderFlag_Initialized;
			
			//pSceneMaterial->materialID = pPod->nNumFrame > 0 ? MT_SkinnedWithNormalMapping:MT_TextureOnlySimple;
		}
		
		//Load up all the models
		for (u32 modelIDX = 0; modelIDX < pPod->nNumMesh; ++modelIDX)
		{
			SPODMesh& Mesh = pPod->pMesh[modelIDX];
			
			//Create model data
			ModelData* pModelData = &pScene->pModel[modelIDX];
			pModelData->modelName = NULL;
			pModelData->primitiveArray = new PrimitiveData[1];	//TODO: I think POD just has one primitive unless it's a triangle strip
			pModelData->numPrimitives = 1;	//TODO: this will be wrong if a triangle strip comes in
			pModelData->numAttributes = 0;
			pModelData->stride = Mesh.sVertex.nStride;
			pModelData->attributeArray = new AttributeData[8];	//TODO: if this goes beyond 8 we're all gunna die
			pModelData->modelID = 0;
			pModelData->attribFlags = 0;
			
			AttributeData* pAttribs = &pModelData->attributeArray[0];
			
			u32 attribOffset = 0;
			
			if(Mesh.sVertex.n > 0)
			{
				AttributeData* pCurrAttrib = &pAttribs[pModelData->numAttributes];
				
				pCurrAttrib->attribute = ATTRIB_VERTEX;
				pCurrAttrib->type = GL_FLOAT;
				pCurrAttrib->size = Mesh.sVertex.n;
				pCurrAttrib->offset = attribOffset;
				attribOffset += pCurrAttrib->size*sizeof(f32); //TODO: won't hold up if you change to non-floats
				
				++pModelData->numAttributes;
				
				pModelData->attribFlags |= ATTRIBFLAG_VERTEX;
			}
			
			if(Mesh.sNormals.n > 0)
			{
				AttributeData* pCurrAttrib = &pAttribs[pModelData->numAttributes];
				
				pCurrAttrib->attribute = ATTRIB_NORMAL;
				pCurrAttrib->type = GL_FLOAT;
				pCurrAttrib->size = Mesh.sNormals.n;
				pCurrAttrib->offset = attribOffset;
				attribOffset += pCurrAttrib->size*sizeof(f32); //TODO: won't hold up if you change to non-floats
				
				++pModelData->numAttributes;
				
				pModelData->attribFlags |= ATTRIBFLAG_NORMAL;
			}
			
			if(Mesh.sBinormals.n > 0)
			{
				AttributeData* pCurrAttrib = &pAttribs[pModelData->numAttributes];
				
				pCurrAttrib->attribute = ATTRIB_BINORMAL;
				pCurrAttrib->type = GL_FLOAT;
				pCurrAttrib->size = Mesh.sBinormals.n;
				pCurrAttrib->offset = attribOffset;
				attribOffset += pCurrAttrib->size*sizeof(f32); //TODO: won't hold up if you change to non-floats
				
				++pModelData->numAttributes;
				
				pModelData->attribFlags |= ATTRIBFLAG_BINORMAL;
			}
			
			if(Mesh.sTangents.n > 0)
			{
				AttributeData* pCurrAttrib = &pAttribs[pModelData->numAttributes];
				
				pCurrAttrib->attribute = ATTRIB_TANGENT;
				pCurrAttrib->type = GL_FLOAT;
				pCurrAttrib->size = Mesh.sTangents.n;
				pCurrAttrib->offset = attribOffset;
				attribOffset += pCurrAttrib->size*sizeof(f32); //TODO: won't hold up if you change to non-floats
				
				++pModelData->numAttributes;
				
				pModelData->attribFlags |= ATTRIBFLAG_TANGENT;
			}
			
			if(Mesh.sVtxColours.n > 0)
			{
				AttributeData* pCurrAttrib = &pAttribs[pModelData->numAttributes];
				
				pCurrAttrib->attribute = ATTRIB_COLOR;
				pCurrAttrib->type = GL_UNSIGNED_BYTE;
				pCurrAttrib->size = Mesh.sVtxColours.n*4;
				pCurrAttrib->offset = attribOffset;
				attribOffset += pCurrAttrib->size*sizeof(u8);
				
				++pModelData->numAttributes;
				
				pModelData->attribFlags |= ATTRIBFLAG_COLOR;
			}
			
			if(Mesh.psUVW != NULL && Mesh.psUVW->n > 0)
			{
				AttributeData* pCurrAttrib = &pAttribs[pModelData->numAttributes];
				
				pCurrAttrib->attribute = ATTRIB_TEXCOORD;
				pCurrAttrib->type = GL_FLOAT;
				
				pCurrAttrib->size = Mesh.psUVW->n;
				pCurrAttrib->offset = attribOffset;
				attribOffset += pCurrAttrib->size*sizeof(f32); //TODO: won't hold up if you change to non-floats or nNumUVW is more than 1
				
				++pModelData->numAttributes;
				
				pModelData->attribFlags |= ATTRIBFLAG_TEXCOORD;
			}

			
			if(Mesh.sBoneWeight.n > 0)
			{
				AttributeData* pCurrAttrib = &pAttribs[pModelData->numAttributes];
				
				pCurrAttrib->attribute = ATTRIB_BONEWEIGHT;
				pCurrAttrib->type = GL_FLOAT;
				pCurrAttrib->size = Mesh.sBoneWeight.n;
				pCurrAttrib->offset = attribOffset;
				attribOffset += pCurrAttrib->size*sizeof(f32); //TODO: won't hold up if you change to non-floats
				
				++pModelData->numAttributes;
				
				pModelData->attribFlags |= ATTRIBFLAG_BONEWEIGHT;
			}
			
			if(Mesh.sBoneIdx.n > 0)
			{
				AttributeData* pCurrAttrib = &pAttribs[pModelData->numAttributes];
				
				pCurrAttrib->attribute = ATTRIB_BONEINDEX;
				pCurrAttrib->type = GL_UNSIGNED_BYTE;
				pCurrAttrib->size = Mesh.sBoneIdx.n;
				pCurrAttrib->offset = attribOffset;
				
				++pModelData->numAttributes;
				
				pModelData->attribFlags |= ATTRIBFLAG_BONEINDEX;
			}
			
			PrimitiveData* pCurrData = &pModelData->primitiveArray[0];
			
			pCurrData->vertexArrayObjectID = 0;
			pCurrData->indexBufferID = 0;
			
			pCurrData->numVerts = Mesh.nNumVertex;
			
			unsigned int vertexBufferSize = Mesh.nNumVertex * Mesh.sVertex.nStride;
			
			pCurrData->drawMethod = GL_TRIANGLES;
			
			pCurrData->vertexData = Mesh.pInterleaved;
			pCurrData->sizeOfVertexData = vertexBufferSize;			
			
			if (Mesh.sFaces.pData)
			{
				const u32 numIndices = PVRTModelPODCountIndices(Mesh);
				pCurrData->numVerts = numIndices;
				const u32 sizeOfGLShort = sizeof(GLshort);
				const u32 indexDataSize = numIndices * sizeOfGLShort;
				
				pCurrData->indexData = (GLushort*)Mesh.sFaces.pData;
				pCurrData->sizeOfIndexData = indexDataSize;
			}
			
			BufferModel(pModelData);
		}
		
		// To draw a scene, you must go through all the MeshNodes and draw the referenced meshes.
		for (u32 nodeIDX = 0; nodeIDX < pPod->nNumMeshNode; ++nodeIDX)
		{
			SPODNode& Node = pPod->pNode[nodeIDX];
			SceneMesh* pSceneMesh = &pScene->pSceneMesh[nodeIDX];
			
			// Get the mesh data from the POD file
			int i32MeshIndex = pPod->pNode[nodeIDX].nIdx;
			
			pSceneMesh->pModelData = &pScene->pModel[i32MeshIndex];
			pSceneMesh->pMaterial = pScene->pMaterial ? &pScene->pMaterial[Node.nIdxMaterial] : NULL;
			
			// Get the node model matrix
			PVRTMat4 mWorld = pPod->GetWorldMatrix(Node);
			for(u32 matIDX=0; matIDX<16; ++matIDX)
			{
				pSceneMesh->worldMat[matIDX] = mWorld.f[matIDX];
			}
			

			//DEBUG!
			//COREDEBUG_PrintDebugMessage("****************Num indices: %d, Triangles: %d****************",pSceneMesh->pModelData->primitiveArray[0].numVerts,pSceneMesh->pModelData->primitiveArray[0].numVerts/3);
			//for(int indexIDX=0; indexIDX<pSceneMesh->pModelData->primitiveArray[0].numVerts; ++indexIDX)
			//{
			//	const u32 index = pSceneMesh->pModelData->primitiveArray[0].indexData[indexIDX];
			//	COREDEBUG_PrintDebugMessage("Index %d: %d",indexIDX,index);

			//	u8* pData = &pSceneMesh->pModelData->primitiveArray[0].vertexData[index*pSceneMesh->pModelData->stride];
			//	vec3* pPos = (vec3*)&pData[0];
			//	vec2* pUV = (vec2*)&pData[12];
			//	COREDEBUG_PrintDebugMessage("Vert: V:<%f,%f,%f>, T:<%f,%f>",pPos->x,pPos->y,pPos->z,pUV->x,pUV->y);
			//}
		}
		
		return true;
	}
	
	return false;
}*/



//helpers

bool LoadPNGImage(const char* fileName, int &outWidth, int &outHeight, bool &outHasAlpha, GLubyte **outData, bool flipY)
{
	FILE         *infile;         /* PNG file pointer */
	png_structp   png_ptr;        /* internally used by libpng */
	png_infop     info_ptr;       /* user requested transforms */
	
	GLubyte         *image_data;      /* raw png image data */
	char         sig[8];           /* PNG signature array */
	/*char         **row_pointers;   */
	
	int           bit_depth;
	int           color_type;
	int			  interlace_type;
	
	png_uint_32 width;            /* PNG image width in pixels */
	png_uint_32 height;           /* PNG image height in pixels */
	png_size_t rowbytes;         /* raw bytes at row n in image */
	
	image_data = NULL;
	
	png_bytepp row_pointers = NULL;
	
	/* Open the file. */
	infile = fopen(fileName, "rb");
	if (!infile)
	{
		return false;
	}
	
	
	/*
	 * 		13.3 readpng_init()
	 */
	
	/* Check for the 8-byte signature */
	fread(sig, 1, 8, infile);
	
	if (!png_check_sig((unsigned char *) sig, 8))
	{
		COREDEBUG_PrintMessage("LoadPNGImage->ERROR: Failed to load PNG: bad sig FILE: %s",fileName);
		fclose(infile);
		return false;
	}
	
	/* 
	 * Set up the PNG structs 
	 */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		COREDEBUG_PrintDebugMessage("LoadPNGImage->ERROR: Failed to load PNG: could not create read struct FILE: %s",fileName);
		fclose(infile);
		return false;    /* out of memory */
	}
	
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		COREDEBUG_PrintMessage("LoadPNGImage->ERROR: Failed to load PNG: could not create info struct FILE: %s",fileName);
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		fclose(infile);
		return false;    /* out of memory */
	}
	
	
	/*
	 * block to handle libpng errors, 
	 * then check whether the PNG file had a bKGD chunk
	 */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		COREDEBUG_PrintMessage("LoadPNGImage->ERROR: Failed to load PNG: no idea (setjump) FILE: %s",fileName);
		
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(infile);
		return false;
	}
	
	/* 
	 * takes our file stream pointer (infile) and 
	 * stores it in the png_ptr struct for later use.
	 */
	/* png_ptr->io_ptr = (png_voidp)infile;*/
	png_init_io(png_ptr, infile);
	
	/*
	 * lets libpng know that we already checked the 8 
	 * signature bytes, so it should not expect to find 
	 * them at the current file pointer location
	 */
	png_set_sig_bytes(png_ptr, 8);
	
	/* Read the image info.*/
	
	/*
	 * reads and processes not only the PNG file's IHDR chunk 
	 * but also any other chunks up to the first IDAT 
	 * (i.e., everything before the image data).
	 */
	
	/* read all the info up to the image data  */
	png_read_info(png_ptr, info_ptr);
	
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
				 &interlace_type, NULL, NULL);
	
	outWidth = width;
	outHeight = height;
	
	/* Set up some transforms. */
	if (color_type & PNG_COLOR_MASK_ALPHA)
	{
		outHasAlpha = true;
		//png_set_strip_alpha(png_ptr);
	}
	else
	{
		outHasAlpha = false;
	}
	
	if (bit_depth > 8)
	{
		png_set_strip_16(png_ptr);
	}
	
	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
		png_set_gray_to_rgb(png_ptr);
	}
	
	if (color_type == PNG_COLOR_TYPE_PALETTE)
	{
		png_set_palette_to_rgb(png_ptr);
	}
	
	/* Update the png info struct.*/
	png_read_update_info(png_ptr, info_ptr);
	
	/* Rowsize in bytes. */
	rowbytes = png_get_rowbytes(png_ptr, info_ptr);
	
	
	/* Allocate the image_data buffer. */
	if ((image_data = (GLubyte *) malloc(rowbytes * height))==NULL)
	{
		COREDEBUG_PrintMessage("LoadPNGImage->ERROR: Failed to load PNG: not enough RAM for image_data FILE: %s",fileName);
		
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return false;
    }
	
	if ((row_pointers = (png_bytepp)malloc(height*sizeof(png_bytep))) == NULL)
	{
		COREDEBUG_PrintMessage("LoadPNGImage->ERROR: Failed to load PNG: not enough RAM for row_pointers FILE: %s",fileName);
		
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        free(image_data);
        image_data = NULL;
        return false;
    }
	
	
    /* set the individual row_pointers to point at the correct offsets */
	
	if(flipY)
	{
		for (unsigned int i = 0;  i < height;  ++i)
		{
			row_pointers[i] = image_data + i*rowbytes;
		}
	}
	else
	{
		for (unsigned int i = 0;  i < height;  ++i)
		{
			row_pointers[height - 1 - i] = image_data + i*rowbytes;
		}
	}
	
    /* now we can go ahead and just read the whole image */
    png_read_image(png_ptr, row_pointers);
	
    /* and we're done!  (png_read_end() can be omitted if no processing of
     * post-IDAT text/time/etc. is desired) */
	
	/* Clean up. */
	free(row_pointers);
	
	/* Clean up. */
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(infile);
	
	*outData = (GLubyte*)image_data;
	
	return true;
}


bool RenderableGeometry3DCompare_SortValue(const RenderableGeometry3D& lhs, const RenderableGeometry3D& rhs)
{
	return lhs.sortValue < rhs.sortValue;
}


bool RenderableGeometry3DCompare_SortByZ(const RenderableGeometry3D& lhs, const RenderableGeometry3D& rhs)
{
	const vec3* pPosLHS = mat4f_GetPos((f32*)lhs.worldMat);
	const vec3* pPosRHS = mat4f_GetPos((f32*)rhs.worldMat);
	
	return pPosLHS->z < pPosRHS->z;
}


bool RenderableGeometry3DCompare_SortByNegativeZ(const RenderableGeometry3D& lhs, const RenderableGeometry3D& rhs)
{
	const vec3* pPosLHS = mat4f_GetPos(lhs.worldMat);
	const vec3* pPosRHS = mat4f_GetPos(rhs.worldMat);

	return (pPosLHS->z > pPosRHS->z) || (pPosLHS->z == pPosRHS->z && pPosLHS->x < pPosRHS->x);
}


RenderableGeometry3D* GetGeomPointer(CoreObjectHandle handle)
{
	return (RenderableGeometry3D*)COREOBJECTMANAGER->GetObjectByHandle(handle);
}

vec3* GetGeomPos(RenderableGeometry3D* pGeom)
{
	return mat4f_GetPos(pGeom->worldMat);
}

void SetGeomPos(RenderableGeometry3D* pGeom, const vec3* pPos)
{
	CopyVec3(mat4f_GetPos(pGeom->worldMat), pPos);
}

vec3* GetGeomLeft(RenderableGeometry3D* pGeom)
{
	return mat4f_GetLeft(pGeom->worldMat);
}

void GetGeomNormalizedLeft(RenderableGeometry3D* pGeom, vec3* pOut_left)
{
	TryNormalizeVec3(pOut_left, GetGeomLeft(pGeom));
}

vec3* GetGeomUp(RenderableGeometry3D* pGeom)
{
	return mat4f_GetUp(pGeom->worldMat);
}

RenderableGeometry3D* CreateSprite(TextureAsset* pTexture, const vec3* pPosition, f32 width, f32 height, RenderLayer renderLayer, RenderMaterial material, BlendMode blendMode)
{
	mat4f scaleMat;
	mat4f_LoadScaleFromFloats(scaleMat, width,height,1.0f);
	
	RenderableGeometry3D* pGeom = NULL;
	GLRENDERER->CreateRenderableGeometry3D(RenderableObjectType_Normal,&pGeom);
	
#if defined (_DEBUG)
	assert(pGeom != NULL);
#endif

	GLRENDERER->InitRenderableGeometry3D(pGeom, &g_Square1x1_modelData, material, &pTexture->textureHandle, scaleMat, renderLayer, blendMode, RenderFlagDefaults_2DTexture_AlphaBlended_UseView);
	
	SetGeomPos(pGeom,pPosition);
	
	return pGeom;
}

RenderableGeometry3D* CreateSpriteSimple(TextureAsset* pTexture, const vec3* pPosition, f32 width, f32 height, RenderLayer renderLayer)
{
	return CreateSprite(pTexture,pPosition,width,height,renderLayer,MT_TextureOnlySimple,BlendMode_Normal);
}
