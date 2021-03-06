#include "GraphicsTypes.h"

//Model: "Square1x1"

static const float g_Square1x1_VertexData0[] = {
	/*v:*/-0.5f, -0.5f, 0.0f, /*t:*/0.0f,0.0f, 
	/*v:*/-0.5f,  0.5f, 0.0f, /*t:*/0.0f,1.0f,
	/*v:*/ 0.5f, -0.5f, 0.0f, /*t:*/1.0f,0.0f,  
	/*v:*/ 0.5f,  0.5f, 0.0f, /*t:*/1.0f,1.0f,
};

static const u32 g_Square1x1_numberOfVertices =	4;
static const u32 g_Square1x1_numberOfPrimitives = 1;
static const u32 g_Square1x1_numberOfAttributes = 2;

static PrimitiveData  g_Square1x1_PrimitiveArray[g_Square1x1_numberOfPrimitives]={
	{GL_TRIANGLE_STRIP,(u8*)g_Square1x1_VertexData0,NULL,g_Square1x1_numberOfVertices,sizeof(g_Square1x1_VertexData0),0,0,0},
};

static AttributeData g_Square1x1_AttributesArray[g_Square1x1_numberOfAttributes]=
{
	{ATTRIB_VERTEX,GL_FLOAT,0,3},
	{ATTRIB_TEXCOORD,GL_FLOAT,12,2},
};

ModelData g_Square1x1_modelData = {0,g_Square1x1_AttributesArray,g_Square1x1_numberOfAttributes,20,"Square1x1",g_Square1x1_PrimitiveArray,g_Square1x1_numberOfPrimitives,-1};
