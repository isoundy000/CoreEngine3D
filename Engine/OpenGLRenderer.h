//
//  OpenGLRenderer.h
//  InfiniSurv
//
//  Created by Jody McAdams on 11/21/11.
//  Copyright (c) 2011 Jody McAdams. All rights reserved.
//

#ifndef InfiniSurv_OpenGLRenderer_h
#define InfiniSurv_OpenGLRenderer_h

class OpenGLRenderer;

extern OpenGLRenderer* GLRENDERER;

//** INCLUDES **/
#if defined (PLATFORM_OSX)
#include <OpenGL/gl.h>
#endif

#if defined (PLATFORM_WIN)
#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <wingdi.h>
#include "glew/glew.h"
#endif

#ifdef PLATFORM_IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

#include <string>

#include "MathUtil.h"
#include "GraphicsTypes.h"
#include "RenderState.h"
#include "MaterialDeclarations.h"
#include "ArrayUtil.h"

#include "PowerVR/PVRTModelPOD.h"

#include "EngineModels.h"

#include "CoreObject_Manager.h"

#define RENDERLOOP_ENABLED 1

#if defined(DEBUG) || defined(_DEBUG)
	#define DEBUG_DRAW 1
#endif


#define GAUSSIAN_NUMSAMPLES 6

#define DEBUG_LINES_MAXVERTICES 30
#define NUM_SINCOS_BUCKETS 10
#define NUM_SINCOS_BUCKET_MAX_MULT 0.3f

#define DEBUGDRAW_MAXLINESPOINTS 2048
#define DEBUGDRAW_MAXDEBUGOBJECTS 256


#define MAX_ANIMATED_PODS 16
#define MAX_RENDERABLE_SCENES 128

#define GAUSSIAN_STANDARDDEVIATION 0.9f

#define MAX_COMPILED_VERTEX_SHADERS 32
#define MAX_COMPILED_PIXEL_SHADERS 32

#define MAX_TEXTURED_LINES 256

#define MAX_NUM_TEXTURE_UNITS 5

/**ENUMS**/
enum RenderableObjectType
{
	RenderableObjectType_Normal,
    RenderableObjectType_Light,
	RenderableObjectType_UI,
    RenderableObjectType_All,
	RenderableObjectType_Num,
};

enum DebugDrawMode
{
	DebugDrawMode_None = -1,
	DebugDrawMode_2D,
	DebugDrawMode_3D,
	DebugDrawMode_Screen2D,
	DebugDrawMode_Num,
};

/**STRUCTS**/
struct SinCosBucket
{
	f32 sinTheta;
	f32 cosTheta;
	f32 multiplier;
};


struct TexturedLineObject
{
	u32 texturedID;
	LineSegment line;
	vec4 diffuseColor;
	f32 lineWidth0;
	f32 lineWidth1;
	f32 texcoordYStart;
	f32 texcoordYEnd;
	DebugDrawMode drawMode;
};


struct TexturedLineVert
{
	vec3 position;
	vec2 texcoord;
};

struct ShaderCreationStatus
{
	const char* filename;
	u32 openGLID;
};


enum DebugDrawObjectType
{
    DebugDrawObjectType_CircleXY,
    DebugDrawObjectType_CircleXZ,
	DebugDrawObjectType_Cylinder,
    DebugDrawObjectType_Cube, //unsupported
};


struct DebugDrawObject
{
    DebugDrawObjectType objectType;
    mat4f matrix;
    vec4 color;
};

class OpenGLRenderer
{
public:
	
	//public functions
	
	void Init(u32 screenWidthPixels, u32 screenHeightPixels,u32 screenWidthPoints, u32 screenHeightPoints);
	bool InitSceneFromPOD(RenderableScene3D* pScene, CPVRTModelPOD* pPod, u32 viewFlags, const char* relativePath);
	void CleanUp();
    CoreObjectHandle CreateRenderableGeometry3D(RenderableObjectType renderableType,RenderableGeometry3D** pOut_Geom);
	
	bool GetFadeFinished();
	void ClearOneFrameGeometry();
	void RenderLoop(u32 camViewIDX,RenderableGeometry3D* renderableObjectArray, u32 numRenderableObjects);
	bool GetSupportsVAOs();
	void SetClearColor(f32 r, f32 g, f32 b);
	const vec3* GetClearColor();
	void SetGravityDir(const vec3* pNewGravityDir);
	void SetSortRenderablesByZ(bool sortRenderablesByZ, RenderLayer layerStart, RenderLayer layerEnd);
	void Render(f32 timeElapsed);
	void DrawAnimatedPOD(AnimatedPOD* pAnimatedPod);
	void DrawSceneObject(RenderableSceneObject3D* pScene);
	CPVRTModelPOD* LoadPOD(const char* fileName);
	AnimatedPOD* AddAnimatedPOD(CPVRTModelPOD* pPod, RenderableScene3D* pScene, mat4f matrix4x4);
	CoreObjectHandle CreateRenderableSceneObject3D(RenderableSceneObject3D** pOut_SceneObject);
	bool LoadTextureFromData(u32* out_textureName,const void* data,u32 texWidth, u32 texHeight, u32 format, u32 type, u32 filterMode, u32 wrapModeU, u32 wrapModeV);
	bool UpdateTextureFromData(u32* out_textureName, const void* data, u32 texWidth, u32 texHeight, u32 format, u32 type);
	void BufferModel(ModelData* pModelData);
	void CreateVBO(u32* pOut_VAO, u32* pOut_VBO, void* pVerts, u32 dataSize, GLenum useage, const AttributeData* pAttrib, u32 numAttribs, u32 vertSize);
	void UpdateVBO(u32 VBO, void* pVerts, u32 dataSize, GLenum useage);
	void InitRenderableGeometry3D(RenderableGeometry3D* renderableObject, ModelData* pModel, RenderMaterial materialID, u32* customTexture, mat4f matrix4x4, RenderLayer renderLayer, BlendMode blendMode, u32 renderFlags);
	void InitRenderableGeometry3D(RenderableGeometry3D* renderableObject, DrawFunctionStruct* pDrawStruct, void* drawObject, RenderMaterial materialID, u32* customTexture, mat4f matrix4x4, RenderLayer renderLayer, BlendMode blendMode, u32 renderFlags);
	void InitRenderableSceneObject3D(RenderableSceneObject3D* renderableObject, RenderableScene3D* pScene, RenderMaterial materialID, u32* customTexture, mat4f matrix4x4, RenderLayer renderLayer, u32 viewFlags, u32 renderFlags);
	void InitRenderableSceneObject3D_Simple(RenderableSceneObject3D* renderableObject, RenderableScene3D* pScene, mat4f matrix4x4, u32 viewFlags);
	void SortRenderableGeometry3DList(RenderableObjectType type);
	void CreateMaterials();
	void SetViewMatrixForIndex(f32* pCameraMatrix, u32 viewIndex);
	void SetViewPos(vec3* pCamPos, u32 viewIndex);
	f32* GetCameraMatrix(u32 viewIndex);
	void StartFadeOut(f32 timeInSeconds);
	void ForceFadeIn(f32 timeInSeconds);
	void StartFadeIn();
	void StartPauseFadeIn();
	void StartPauseFadeOut(f32 finalFade, f32 timeInSeconds);
	void ResetPauseFade();
	void DeleteTexture(u32* pTextureID);

	void SetScreenFadeColor(vec3* screenFadeColor);
	void ForceRenderablesNeedSorting(RenderableObjectType renderableType);
	void SetScreenFramebuffer(u32 framebuffer);
	void FlashScreen(const vec3* pColor, f32 timeInSeconds);
	void ShakeScreen(f32 shakeAmount,f32 shakeSpeed, f32 shakeTime);
	bool LoadTexture(const char* fileName,ImageType imageType, u32* pGLTexture, u32 filterMode, u32 wrapModeU, u32 wrapModeV, bool flipY = false);

	void DRAW_DrawTexturedLine(DebugDrawMode drawMode, const vec3* p0, const vec3* p1, const vec4* pDiffuseColor, u32 texturedID, f32 lineWidth0, f32 lineWidth1, f32 texcoordYStart, f32 texcoordYEnd);
	void DEBUGDRAW_DrawRay(DebugDrawMode drawMode, const vec3* p0, const vec3* direction, f32 length, const vec4* color);
	void DEBUGDRAW_DrawLineSegment(DebugDrawMode drawMode, const vec3* p0, const vec3* p1, const vec4* color);
	void DEBUGDRAW_DrawLineSegment(DebugDrawMode drawMode, const vec3* p0, const vec3* p1, const vec4* color1, const vec4* color2);
	void DEBUGDRAW_DrawCircleXY(DebugDrawMode drawMode, mat4f matrix4x4, const vec4* color);
	void DEBUGDRAW_DrawCircleXZ(DebugDrawMode drawMode, mat4f matrix4x4, const vec4* color);
	void DEBUGDRAW_DrawCircleXY(DebugDrawMode drawMode, const vec3* pCenter, f32 radius, const vec4* color);
	void DEBUGDRAW_DrawCircleXZ(DebugDrawMode drawMode, const vec3* pCenter, f32 radius, const vec4* color);
	void DEBUGDRAW_DrawCylinder(DebugDrawMode drawMode, mat4f matrix4x4, const vec4* color);
	void DisableVertexBufferObjects();
	//public member variables
	s32 screenWidth_points;
	s32 screenHeight_points;
	s32 screenWidth_pixels;
	s32 screenHeight_pixels;
	f32 aspectRatio;
	bool paused;
	
	void BindVertexArrayObject(u32 vao);
	void SetMaterial(RenderMaterial material);
	void SetTexture(const u32* pTexture,u32 textureUnit);
	void UploadWorldProjMatrix(const f32* pWorldMat); //only call after setMaterial
	void EnableAttribute(const AttributeData* pAttrib, u32 stride);
private:
	
	//private functions
	void InitRenderableGeometry3D_Shared(RenderableGeometry3D* renderableObject, RenderMaterial materialID, u32* customTexture, mat4f matrix4x4, RenderLayer renderLayer, BlendMode blendMode, u32 renderFlags);
	
	void SortRenderablesWithMaterialByZ(RenderMaterial materialID);
	void SortRenderablesInLayerRangeByZ(RenderLayer layerBegin, RenderLayer layerEnd);
	
	void DeleteScene(RenderableScene3D* pScene);
	void PostProcess(RenderMaterial ppMaterial, RenderTarget* renderTarget, PPDrawArea drawArea, u32* texture0, u32* texture1, u32* texture2);
	void PrintOpenGLError(const char* callerName);
    void PrintOpenGLFrameBufferStatus(bool printSuccess);
    bool CreateFrameBuffer(u32* pOut_FrameBuffer, u32* pInOut_colorBufferOrTexture, bool createColorBuffer, u32* pInOut_depthBuffer, bool createDepthBuffer, ColorBufferType colorBufferType, u32 width, u32 height);
	bool CreateShaderProgram(s32 vertexShaderIndex, s32 pixelShaderIndex, AttributeFlags attribs, u32* out_resultProgram);
	void SetRenderTarget(RenderTarget* renderTarget);
	void UploadWorldViewProjMatrix(const f32* pWorldMat); //only call after setMaterial
	void UploadProjMatrix(const f32* pProjMat); //only call after setMaterial
	
	void UploadSharedUniforms();
	void UploadUniqueUniforms(u8* const * pValuePointerArray);
	
	void SetRenderState(BlendMode blendMode, u32 renderFlags);
	f32 ComputeGaussianValue(f32 x, f32 stdDevSq);
	void ComputeGaussianWeights(f32* out_pWeights, s32 numWeights, f32 standardDeviationSquared);
	void EnableAttributes(const ModelData* pModelData);
	
	void BindIndexData(const PrimitiveData* pPrimitive);
	void AddUniform_Unique(RenderMaterial renderMaterial, const char* nameOfUniformInShader,UniformType uniformType, u32 amount);
	void AddUniform_Shared(RenderMaterial renderMaterial, const char* nameOfUniformInShader, UniformType uniformType, u8* pData, u32 amount);
	void AddUniform_Shared_Const(RenderMaterial renderMaterial, const char* nameOfUniformInShader, UniformType uniformType, u8* pData, u32 amount);

	bool CompileShader(u32 *shader, s32 type, s32 count, const char* filename);
	bool LinkProgram(u32 prog);
	s32 ValidateProgram(u32 prog);

	bool CreatePixelShader(s32 pixelShaderIndex);
	bool CreateVertexShader(s32 vertexShaderIndex);
	s32 AddVertexShaderToList(const char* filename);
	s32 AddPixelShaderToList(const char* filename);
	
	//private member variables
	ShaderCreationStatus m_vertexShaders[MAX_COMPILED_VERTEX_SHADERS];
	ShaderCreationStatus m_pixelShaders[MAX_COMPILED_PIXEL_SHADERS];
	s32 m_numVertexShaders;
	s32 m_numPixelShaders;
	s32 backingWidth;
	s32 backingHeight;

	vec3 m_camPos[NumCameraViews];
    
    u32 m_postProcessDefaultVertShader;
	f32 m_gaussianWeights[GAUSSIAN_NUMSAMPLES];
	f32 m_accumulatedTime;
	f32* m_view[NumCameraViews];
	mat4f m_projMats[NumProjMatTypes];
	mat4f m_viewProjMats[NumCameraViews][NumProjMatTypes];
	ProjMatType m_currProjMatType;
	vec2 m_gaussianViewportSize;
	bool firstRun;
	bool m_readyToGo;
	f32 m_totalFadeTime;
    f32 m_totalFlashTime;
    f32 m_currFlashTime;
	f32 m_currFadeTime;
	f32 m_currFadeValue;
    f32 m_finalFade;
	FadeState m_fadeState;
    FadeState m_flashState;
    bool m_requestedFadeIn;
	RenderMaterial m_lastUsedMaterial;
	vec4 m_fadeColor;
	vec3 m_fadeToScreenColor;
	u32 m_currViewIndex;
	bool m_renderableObject3DsNeedSorting_UI;
	bool m_renderableObject3DsNeedSorting_Normal;
    bool m_renderableObject3DsNeedSorting_Light;
	u32 m_glBufferCount;
	u32 m_glTotalBufferSize;
	u32 m_glTotalTextureSize;
	u32 m_accumulatedFrames;
	s32 m_numModels;
    u32 m_maxNumRenderables;

	u32 m_currTexture;
    vec3 m_flashColor;
    u32 m_currTextureInTextureUnit[MAX_NUM_TEXTURE_UNITS];
    u32 m_currTextureUnit;
    SinCosBucket m_sinCosBuckets[NUM_SINCOS_BUCKETS];

	AnimatedPOD m_animatedPODs[MAX_ANIMATED_PODS];
	u32 m_numAnimatedPods;

    PointData m_debugLinePoints[DebugDrawMode_Num][DEBUGDRAW_MAXLINESPOINTS];
    s32 m_numDebugLinePoints[DebugDrawMode_Num];
	s32 m_numDebugLinePoints_saved[DebugDrawMode_Num];
    TexturedLineObject m_texturedLineObjects[MAX_TEXTURED_LINES];
	s32 m_numTexturedLines;
	s32 m_numTexturedLines_saved;
	TexturedLineVert m_texturedLineVerts[4];

    u32 m_renderStateFlags;
	BlendMode m_blendMode;
	bool m_hasSetRenderstateThisFrame;
	
    f32 m_screenShakerT_X;
    f32 m_screenShakerT_Y;
    f32 m_screenShakerSpeed_X;
    f32 m_screenShakerSpeed_Y;
    f32 m_currScreenShakeAmount;
    f32 m_screenShakeTimer;
    f32 m_screenShakeStartTime;
    f32 m_screenShakeSpeed;
    vec3 m_gravityDir;
    bool m_supportsVAOs;
    bool m_supportsMultisampling;
    FadeState m_pauseFadeState;
	f32 m_pauseTotalFadeTime;
    f32 m_pauseFadeTimeAtStart;
	f32 m_pauseCurrFadeTime;
    f32 m_pauseFinalFade;
    bool m_pauseRequestedFadeIn;
    bool m_sortRenderablesByZ;
    vec3 m_clearColor;
    DebugDrawObject m_debugDrawObjects[DebugDrawMode_Num][DEBUGDRAW_MAXDEBUGOBJECTS];
    u32 m_numDebugDrawObjects[DebugDrawMode_Num];
	u32 m_numDebugDrawObjects_saved[DebugDrawMode_Num];
	char* m_pArtPath;
	f32 m_identityMat[16];
	vec3 m_lightPos;
    
    RenderTarget m_renderTarget_Normal;
    RenderTarget m_renderTarget_DownsampleA;
    RenderTarget m_renderTarget_DownsampleB;
    RenderTarget m_renderTarget_Screen;
	
	RenderLayer m_sortRenderablesLayerStart;
	RenderLayer m_sortRenderablesLayerEnd;
    
    f32 m_clearColorPulseTimer;
    
    bool m_postProcessFlipper;
};


//Helpers
char* FileToCharArray(const char* filename);
bool LoadPNGImage(const char *name, int &outWidth, int &outHeight, bool &outHasAlpha, u8 **outData, bool flipY);
bool RenderableGeometry3DCompare_SortByZ(const RenderableGeometry3D& lhs, const RenderableGeometry3D& rhs);
bool RenderableGeometry3DCompare_SortByNegativeZ(const RenderableGeometry3D& lhs, const RenderableGeometry3D& rhs);
bool RenderableGeometry3DCompare_SortValue(const RenderableGeometry3D& lhs, const RenderableGeometry3D& rhs);
void PackFloat16(float myFloat, float* out_x, float* out_y);
float UnpackFloat16(float x, float y);
void DoubleRenderTarget_Flip(DoubleRenderTarget* doubleRenderTarget);

void Draw_Matrix();

//Helpers
RenderableGeometry3D* GetGeomPointer(CoreObjectHandle handle);
vec3* GetGeomPos(RenderableGeometry3D* pGeom);
void SetGeomPos(RenderableGeometry3D* pGeom, const vec3* pPos);
vec3* GetGeomLeft(RenderableGeometry3D* pGeom);
vec3* GetGeomUp(RenderableGeometry3D* pGeom);
void GetGeomNormalizedLeft(RenderableGeometry3D* pGeom, vec3* pOut_left);
RenderableGeometry3D* CreateSpriteSimple(TextureAsset* pTexture, const vec3* pPosition, f32 width, f32 height, RenderLayer renderLayer);
RenderableGeometry3D* CreateSprite(TextureAsset* pTexture, const vec3* pPosition, f32 width, f32 height, RenderLayer renderLayer, RenderMaterial material, BlendMode blendMode);
#endif
