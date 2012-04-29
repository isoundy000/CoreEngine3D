//Author: Jody McAdams
//Website: http://www.jodymcadams.com
//Company Website: http://www.gamejellyonline.com

unsigned long long tmpseed; 
int newseed;
int mlcg,p,q;

#include "MathUtil.h"
#include <math.h>
#include <stdlib.h>
#include <cstdio>

static const f32 g_MathUtil_Epsilon = 0.0001f;


// This Function Gets The First Power Of 2 >= The
// Int That We Pass It.
s32 PowerOf2(s32 num)
{
    int rval=1;
    // rval<<=1 Is A Prettier Way Of Writing rval*=2;
    while(rval<num) rval<<=1;
    return rval;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void SwapS32(s32* l, s32* r)
{
	int temp = *l;
	*l = *r;
	*r = temp;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32 MaxF(f32 l, f32 r)
{
	return l>r?l:r;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
s32 MaxS32(int l, int r)
{
	return l>r?l:r;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
s32 MinS32(int l, int r)
{
	return l<r?l:r;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
u32 MaxU32(unsigned int l, unsigned int r)
{
	return l>r?l:r;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
u32 MinU32(unsigned int l, unsigned int r)
{
	return l<r?l:r;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32 MinF(f32 l, f32 r)
{
	return l<r?l:r;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
s32	  ClampS32(s32 n, s32 clampMin, s32 clampMax)
{
	if(n < clampMin)
	{
		n = clampMin;
	}
	else if(n > clampMax)
	{
		n = clampMax;
	}
	
	return n;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
u32	  ClampU32(u32 n, u32 clampMin, u32 clampMax)
{
	if(n < clampMin)
	{
		n = clampMin;
	}
	else if(n > clampMax)
	{
		n = clampMax;
	}
	
	return n;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32 ClampF(f32 n, f32 clampMin, f32 clampMax)
{
	if(n < clampMin)
	{
		n = clampMin;
	}
	else if(n > clampMax)
	{
		n = clampMax;
	}
	
	return n;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32 DegreesToRadians(f32 degrees)
{
	return degrees * 0.0174532925f;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  SetVec2(vec2* out_resultVec,f32 x, f32 y)
{
	out_resultVec->x = x;
	out_resultVec->y = y;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32 DotVec2(const vec2* vec0, const vec2* vec1)
{
    return vec0->x*vec1->x + vec0->y*vec1->y;
}


//----------------------------------------------------------------------------
//Projects vec0 onto normalized vec1
//----------------------------------------------------------------------------
void ProjectVec2_Norm(vec2* out_resultVec, const vec2* vec0, const vec2* vec1)
{
    const f32 dotResult = DotVec2(vec0, vec1);
    ScaleVec2(out_resultVec, vec0, dotResult);   
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32 DotVec3(const vec3* vec0, const vec3* vec1)
{
	return vec0->x*vec1->x + vec0->y*vec1->y + vec0->z*vec1->z;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32 Lerp(f32 f0, f32 f1, f32 t)
{
	return f0 + t*(f1-f0);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  LerpVec2(vec2* out_lerpedPos, vec2* p0, vec2* p1, f32 t)
{
    out_lerpedPos->x = p0->x + t*(p1->x-p0->x);
    out_lerpedPos->y = p0->y + t*(p1->y-p0->y);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool ApproxEqualsZeroVec3(vec3* pTestvec)
{
	return (pTestvec->x < g_MathUtil_Epsilon && pTestvec->x > -g_MathUtil_Epsilon)
	&& (pTestvec->y < g_MathUtil_Epsilon && pTestvec->y > -g_MathUtil_Epsilon)
	&& (pTestvec->z < g_MathUtil_Epsilon && pTestvec->z > -g_MathUtil_Epsilon);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  LerpVec3(vec3* out_lerpedPos, const vec3* p0, const vec3* p1, f32 t)
{
    out_lerpedPos->x = p0->x + t*(p1->x-p0->x);
    out_lerpedPos->y = p0->y + t*(p1->y-p0->y);
    out_lerpedPos->z = p0->z + t*(p1->z-p0->z);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  LerpVec4(vec4* out_lerpedPos, const vec4* p0, const vec4* p1, f32 t)
{
    out_lerpedPos->x = p0->x + t*(p1->x-p0->x);
    out_lerpedPos->y = p0->y + t*(p1->y-p0->y);
    out_lerpedPos->z = p0->z + t*(p1->z-p0->z);
    out_lerpedPos->w = p0->w + t*(p1->w-p0->w);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  MakeRotatedVec2(vec2* out_resultVec, f32 angleRadians)
{
	out_resultVec->x = cosf(angleRadians);
	out_resultVec->y = -sinf(angleRadians);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  MakeRotatedVec2_SinCos(vec2* out_resultVec, f32 sinTheta, f32 cosTheta)
{
    out_resultVec->x = cosTheta;
	out_resultVec->y = -sinTheta;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32 MagnitudeVec2(const vec2* vec)
{
	return sqrtf(DotVec2(vec,vec));
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32 DistVec3_2D(const vec3* vec0, const vec3* vec1)
{
	vec3 vec2D;
	SubVec3(&vec2D, vec0, vec1);
	vec2D.y = 0.0f;
	return MagnitudeSqVec3(&vec2D);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32 DistVec3(const vec3* vec0, const vec3* vec1)
{
	return sqrtf(DistSqVec3(vec0,vec1));
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32 DistSqVec3(const vec3* vec0, const vec3* vec1)
{
	vec3 distVec;
	SubVec3(&distVec, vec0, vec1);
	return MagnitudeSqVec3(&distVec);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32 MagnitudeSqVec2(const vec2* vec)
{
	return DotVec2(vec,vec);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32 DistSqVec2(const vec2* vec0, const vec2* vec1)
{
	vec2 distVec;
	SubVec2(&distVec, vec0, vec1);
	return MagnitudeSqVec2(&distVec);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32 DistVec2(const vec2* vec0, const vec2* vec1)
{
    return sqrtf(DistSqVec2(vec0, vec1));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool PosIsBehind(const vec3* frontPos, const vec3* frontAt, const vec3* behindPos)
{
	vec3 dirVec;
	SubVec3(&dirVec, behindPos, frontPos);
	
	return DotVec3(frontAt, &dirVec) <= 0.0f;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32 MagnitudeSqVec3(const vec3* vec)
{
	return DotVec3(vec,vec);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32 MagnitudeVec3(const vec3* vec)
{
	return sqrtf(MagnitudeSqVec3(vec));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32  NormalizeVec2_Self(vec2* vec)
{
	const f32 length = MagnitudeVec2(vec);
	vec->x/=length;
	vec->y/=length;
    
    return length;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32  TryNormalizeVec2_Self(vec2* vec)
{
    const f32 lengthSq = MagnitudeSqVec2(vec);
    
    if(lengthSq > g_MathUtil_Epsilon)
    {
        const f32 length = sqrtf(lengthSq);
        
        vec->x/=length;
        vec->y/=length;
        
        return length;
    }
    
    return 0.0f;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32  NormalizeVec2(vec2* out_resultVec, const vec2* vec)
{
	const f32 length = MagnitudeVec2(vec);
	out_resultVec->x = vec->x/length;
	out_resultVec->y = vec->y/length;
    
    return length;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32  NormalizeVec3(vec3* out_resultVec, const vec3* vec)
{
	const f32 length = MagnitudeVec3(vec);
	out_resultVec->x = vec->x/length;
	out_resultVec->y = vec->y/length;
	out_resultVec->z = vec->z/length;
    
    return length;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32 NormalizeVec3_Self(vec3* vec)
{
	const f32 length = MagnitudeVec3(vec);
	vec->x/=length;
	vec->y/=length;
	vec->z/=length;
    
    return length;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32  TryNormalizeVec3(vec3* out_resultVec, const vec3* vec)
{
    const f32 lengthSq = MagnitudeSqVec3(vec);
    
    if(lengthSq > g_MathUtil_Epsilon)
    {
        const f32 length = sqrtf(lengthSq);
        
        out_resultVec->x = vec->x/length;
        out_resultVec->y = vec->y/length;
        out_resultVec->z = vec->z/length;
        
        return length;
    }
    
    return 0.0f;
}



//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ProjectVec3_Norm(vec3* out_resultVec, vec3* vec0, vec3* vec1)
{
    const f32 dotResult = DotVec3(vec0, vec1);
    ScaleVec3(out_resultVec, vec1, dotResult);  
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32  TryNormalizeVec3_Self(vec3* vec)
{
    const f32 lengthSq = MagnitudeSqVec3(vec);
    
    if(lengthSq > g_MathUtil_Epsilon)
    {
        const f32 length = sqrtf(lengthSq);
        
        vec->x/=length;
        vec->y/=length;
        vec->z/=length;
        
        return length;
    }
    
    return 0.0f;
}


//----------------------------------------------------------------------------
//TODO: I think this might be wrong
//----------------------------------------------------------------------------
void  RotateVec2(vec2* out_resultVec, const vec2* vec, f32 sinTheta, f32 cosTheta)
{
	const f32 x = vec->x;
	const f32 y = vec->y;
	
	out_resultVec->x = x*cosTheta - y*sinTheta;
	out_resultVec->y = x*sinTheta + y*cosTheta;
}

//----------------------------------------------------------------------------
//TODO: I think this might be wrong
//----------------------------------------------------------------------------
void  RotateVec2_Self(vec2* vec, f32 sinTheta, f32 cosTheta)
{
	const f32 x = vec->x;
	const f32 y = vec->y;
	
	vec->x = x*cosTheta - y*sinTheta;
	vec->y = x*sinTheta + y*cosTheta;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  SubVec2(vec2* out_resultVec, const vec2* p0, const vec2* p1)
{
	out_resultVec->x = p0->x-p1->x;
	out_resultVec->y = p0->y-p1->y;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  SubVec2_Self(vec2* out_resultVec, const vec2* p)
{
	out_resultVec->x -= p->x;
	out_resultVec->y -= p->y;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  SubVec3(vec3* out_resultVec, const vec3* p0, const vec3* p1)
{
	out_resultVec->x = p0->x-p1->x;
	out_resultVec->y = p0->y-p1->y;
	out_resultVec->z = p0->z-p1->z;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  SubVec3_Self(vec3* out_resultVec, const vec3* p0)
{
	out_resultVec->x -= p0->x;
	out_resultVec->y -= p0->y;
	out_resultVec->z -= p0->z;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  ScaleVec2_Self(vec2* vec, f32 scale)
{
	vec->x *= scale;
	vec->y *= scale;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  ScaleVec2(vec2* out_resultVec, const vec2* vec, f32 scale)
{
	out_resultVec->x = vec->x*scale;
	out_resultVec->y = vec->y*scale;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  ScaleVec3_Self(vec3* vec, f32 scale)
{
	vec->x *= scale;
	vec->y *= scale;
	vec->z *= scale;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  ScaleVec3(vec3* out_resultVec, const vec3* vec, f32 scale)
{
	out_resultVec->x = vec->x*scale;
	out_resultVec->y = vec->y*scale;
	out_resultVec->z = vec->z*scale;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  ScaleVec4(vec4* out_resultVec, const vec4* vec, f32 scale)
{
    out_resultVec->x = vec->x*scale;
	out_resultVec->y = vec->y*scale;
	out_resultVec->z = vec->z*scale;
    out_resultVec->w = vec->w*scale;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  ScaleVec4_Self(vec4* out_resultVec, f32 scale)
{
	out_resultVec->x *= scale;
	out_resultVec->y *= scale;
	out_resultVec->z *= scale;
    out_resultVec->w *= scale;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  AddScaledVec2_Self(vec2* out_resultVec, const vec2* vec, f32 scale)
{
	out_resultVec->x += vec->x*scale;
	out_resultVec->y += vec->y*scale;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  AddScaledVec2(vec2* out_resultVec, const vec2* vec0, const vec2* vec1, f32 scale)
{
	out_resultVec->x = vec0->x + vec1->x*scale;
	out_resultVec->y = vec0->y + vec1->y*scale;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  SubScaledVec2_Self(vec2* out_resultVec, vec2* vec, f32 scale)
{
	out_resultVec->x -= vec->x*scale;
	out_resultVec->y -= vec->y*scale;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  SubScaledVec2(vec2* out_resultVec, vec2* vec0, vec2* vec1, f32 scale)
{
	out_resultVec->x = vec0->x - vec1->x*scale;
	out_resultVec->y = vec0->y - vec1->y*scale;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  AddVec2_Self(vec2* out_resultVec, const vec2* vec)
{
	out_resultVec->x += vec->x;
	out_resultVec->y += vec->y;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  AddVec2(vec2* out_resultVec, const vec2* vec0, const vec2* vec1)
{
	out_resultVec->x = vec0->x + vec1->x;
	out_resultVec->y = vec0->y + vec1->y;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  CopyVec2(vec2* out_result, const vec2* point)
{
	out_result->x = point->x;
	out_result->y = point->y;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  CopyVec3(vec3* out_result, const vec3* point)
{
	out_result->x = point->x;
	out_result->y = point->y;
	out_result->z = point->z;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  CopyVec4(vec4* out_result, const vec4* point)
{
	out_result->x = point->x;
	out_result->y = point->y;
	out_result->z = point->z;
	out_result->w = point->w;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  CopyVec3IntoVec4(vec4* out_result, const vec3* point)
{
	out_result->x = point->x;
	out_result->y = point->y;
	out_result->z = point->z;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  ReflectVec2_Self(vec2* vec, const vec2* normal)
{
	ReflectVec2(vec, vec, normal);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  ReflectVec2(vec2* out_resultVec, vec2* vec, const vec2* normal)
{
	f32 dot = -DotVec2(normal, vec);
	vec2 reflect;
	ScaleVec2(&reflect, normal, 2.0f * dot);
	AddVec2(out_resultVec, vec, &reflect);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  CreatePlaneFromPointsVec2(struct PlaneVec2* out_resultPlane, vec2* p0, vec2* p1)
{
	//Copy point
	CopyVec2(&out_resultPlane->point, p0);
	
	//Create normal
	SubVec2(&out_resultPlane->normal,p1,p0);
	NormalizeVec2_Self(&out_resultPlane->normal);
	
	const f32 tempX = out_resultPlane->normal.x;
	
	//Rotate 90 degrees (normal faces to the right of the direction)
	out_resultPlane->normal.x = out_resultPlane->normal.y;
	out_resultPlane->normal.y = -tempX;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool  PointInsidePlaneVec2(vec2* point,struct PlaneVec2* plane)
{
	vec2 dirVec;
	SubVec2(&dirVec,point,&plane->point);
	return DotVec2(&dirVec,&plane->normal) < 0.0f;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32 AngleBetweenVec2(vec2* vec0, vec2* vec1)
{
	return atan2(vec1->y,vec1->x) - atan2(vec0->y,vec0->x);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32 AngleOfVec2(vec2* vec0)
{
	return atan2(vec0->y,vec0->x);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
f32 AngleBetweenVec3(vec3* vec0, vec3* vec1)
{
	//TODO: get the angle
	return 0;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  CrossVec3(vec3* out_resultVec, const vec3* vec0, const vec3* vec1)
{
	const f32 x = vec0->y*vec1->z - vec1->y*vec0->z;
	const f32 y = vec0->z*vec1->x - vec1->z*vec0->x;
	const f32 z = vec0->x*vec1->y - vec1->x*vec0->y;
	
	out_resultVec->x = x;
	out_resultVec->y = y;
	out_resultVec->z = z;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  SetVec3(vec3* out_resultVec,f32 x, f32 y, f32 z)
{
	out_resultVec->x = x;
	out_resultVec->y = y;
	out_resultVec->z = z;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  SetVec4(vec4* out_resultVec,f32 x, f32 y, f32 z, f32 w)
{
	out_resultVec->x = x;
	out_resultVec->y = y;
	out_resultVec->z = z;
	out_resultVec->w = w;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  AddVec3_Self(vec3* out_resultVec, const vec3* vec)
{
	out_resultVec->x += vec->x;
	out_resultVec->y += vec->y;
	out_resultVec->z += vec->z;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  AddVec3(vec3* out_resultVec, const vec3* vec0, const vec3* vec1)
{
	out_resultVec->x = vec0->x+vec1->x;
	out_resultVec->y = vec0->y+vec1->y;
	out_resultVec->z = vec0->z+vec1->z;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  AddVec3ThenScale_Self(vec3* out_resultVec, vec3* vec, f32 scale)
{
    AddVec3_Self(out_resultVec,vec);
    ScaleVec3_Self(out_resultVec, scale);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  AddVec3ThenScale(vec3* out_resultVec, vec3* vec0, vec3* vec1, f32 scale)
{
    AddVec3(out_resultVec,vec0,vec1);
    ScaleVec3_Self(out_resultVec, scale);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  AddScaledVec3_Self(vec3* out_resultVec, const vec3* vec, f32 scale)
{
	out_resultVec->x += vec->x*scale;
	out_resultVec->y += vec->y*scale;
	out_resultVec->z += vec->z*scale;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  AddScaledVec3(vec3* out_resultVec, const vec3* vec0, const vec3* vec1, f32 scale)
{
	out_resultVec->x = vec0->x+vec1->x*scale;
	out_resultVec->y = vec0->y+vec1->y*scale;
	out_resultVec->z = vec0->z+vec1->z*scale;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  SubScaledVec3_Self(vec3* out_resultVec, vec3* vec, f32 scale)
{
	out_resultVec->x -= vec->x*scale;
	out_resultVec->y -= vec->y*scale;
	out_resultVec->z -= vec->z*scale;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  SubScaledVec3(vec3* out_resultVec, vec3* vec0, vec3* vec1, f32 scale)
{
	out_resultVec->x = vec0->x-vec1->x*scale;
	out_resultVec->y = vec0->y-vec1->y*scale;
	out_resultVec->z = vec0->z-vec1->z*scale;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void RotateVec3AboutAxis(vec3* out_resultVec,const vec3* vec0, const vec3* pAxis,f32 sinAngle, f32 cosAngle)
{
    // A: Vector to rotate.
	// R: Axis of rotation.
	// U: R x A, perpendicular to the axis and A.
	// V: A projected onto the axis.
	// L: Component of A perpendicular to the axis.
    
	// get up perpendicular to the axis and input
	vec3 up;
    CrossVec3(&up, pAxis, vec0);
	
	// project intput onto axis
	const f32 projectScale = DotVec3(vec0, pAxis );
	vec3 projected;
    ScaleVec3(&projected, pAxis, projectScale);
    
	// get component of input perpendicular to the axis
	vec3 perp;
    SubVec3(&perp,vec0,&projected);
    
	// build the rotated vector
    AddScaledVec3(out_resultVec, &projected, &perp, cosAngle);
    AddScaledVec3_Self(out_resultVec, &up, sinAngle);
}


void RotateVec3AboutAxis_Self(vec3* out_resultVec,const vec3* pAxis,f32 sinAngle, f32 cosAngle)
{
    // A: Vector to rotate.
	// R: Axis of rotation.
	// U: R x A, perpendicular to the axis and A.
	// V: A projected onto the axis.
	// L: Component of A perpendicular to the axis.
    
	// get up perpendicular to the axis and input
	vec3 up;
    CrossVec3(&up, pAxis, out_resultVec);
    
	// project intput onto axis
	const f32 projectScale = DotVec3(out_resultVec, pAxis );
	vec3 projected;
    ScaleVec3(&projected, pAxis, projectScale);
    
	// get component of input perpendicular to the axis
	vec3 perp;
    SubVec3(&perp,out_resultVec,&projected);
    
	// build the rotated vector
    AddScaledVec3(out_resultVec, &projected, &perp, cosAngle);
    AddScaledVec3_Self(out_resultVec, &up, sinAngle);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void  MakeRandomVec3WithinBounds(vec3* pOut_resultVec, f32 x0, f32 x1, f32 y0, f32 y1, f32 z0, f32 z1)
{
    pOut_resultVec->x = rand_FloatRange(x0, x1);
    pOut_resultVec->y = rand_FloatRange(y0, y1);
    pOut_resultVec->z = rand_FloatRange(z0, z1);
}

//----------------------------------------------------------------------------
//Returns a random float in the range (a,b)
//----------------------------------------------------------------------------
f32 rand_FloatRange(f32 a, f32 b)
{
	return ((b-a)*((f32)rand()/RAND_MAX))+a;
}


//----------------------------------------------------------------------------
//Returns a random int in the range (a,b)
//----------------------------------------------------------------------------
s32 rand_IntRange(s32 a, s32 b)
{
	return a+rand()%((b-a)+1);
}

//----------------------------------------------------------------------------
//Returns a random int in the range (a,b)
//----------------------------------------------------------------------------
bool rand_Bool()
{
	return rand_FloatRange( 0.0f, 1.0f ) >= 0.5f;
}


//The base Catmull-Rom evaluation function which requires 4 points
void CatmullRom_Evaluate(vec3* pOut_result, const vec3* p0, const vec3* p1, const vec3* p2, const vec3* p3, f32 t)
{
	const f32 c0 = ((-t + 2.0f) * t - 1.0f) * t * 0.5f;
	const f32 c1 = (((3.0f * t - 5.0f) * t) * t + 2.0f) * 0.5f;
	const f32 c2 = ((-3.0f * t + 4.0f) * t + 1.0f) * t * 0.5f;
	const f32 c3 = ((t - 1.0f) * t * t) * 0.5f;
	
	ScaleVec3(pOut_result, p0, c0);
	AddScaledVec3_Self(pOut_result, p1, c1);
	AddScaledVec3_Self(pOut_result, p2, c2);
	AddScaledVec3_Self(pOut_result, p3, c3);
}


//Evaluates a curve with any number of points using the Catmull-Rom method
void CatmullRom_EvaluateCurve(vec3* pOut_result, const vec3* pPoints, u32 numPoints, f32 t)
{
	const f32 index1F = t * (f32)(numPoints-1);
	const u32 index1 = (u32)index1F;
	
	const f32 remainder = index1F - (f32)index1;
	
	const u32 index0 = MaxS32(0,index1 - 1);
	const u32 index2 = index1 + 1;
	const u32 index3 = MinS32(numPoints-1,index1 + 2);
	
	const vec3* p0 = &pPoints[index0];
	const vec3* p1 = &pPoints[index1];
	const vec3* p2 = &pPoints[index2];
	const vec3* p3 = &pPoints[index3];
	
	CatmullRom_Evaluate(pOut_result, p0, p1, p2, p3, remainder);
}



//Creates the specified number of points along a Catmull-Rom spline (non-uniform)
void CatmullRom_CreatePoints(vec3* pOut_pointArray, u32 numPointsDesired, const vec3* pCurvePoints, u32 numCurvePoints)
{	
	const f32 numPointsDesiredMin1 = (f32)(numPointsDesired-1);
	
	for(u32 i=0; i<numPointsDesired; ++i)
	{
		const f32 t = (f32)i/numPointsDesiredMin1;
		CatmullRom_EvaluateCurve(&pOut_pointArray[i],pCurvePoints,numCurvePoints,t);
	}
}


//Creates the specified number of points along a Catmull-Rom spline (uniformly spaced)
void CatmullRom_CreatePoints_Uniform(vec3* pOut_pointArray, u32 numPointsDesired, const vec3* pCurvePoints, u32 numCurvePoints, DistanceTableEntry* pDistTable, u32 numDistTableEntries)
{
	const f32 numPointsDesiredMin1 = (f32)(numPointsDesired-1);
	const f32 totalLength = pDistTable[numDistTableEntries-1].distance;
	
	for(u32 i=0; i<numPointsDesired; ++i)
	{
		const f32 distT = (f32)i/(f32)numPointsDesiredMin1;
		const f32 distance = distT * totalLength;
		
		const f32 t = Curve_TValueFromDist(distance,pDistTable,numDistTableEntries);
		CatmullRom_EvaluateCurve(&pOut_pointArray[i],pCurvePoints,numCurvePoints,t);
	}
}


//Creates a distance table used by Curve_TValueFromDist below
void Curve_CreateDistanceTable(DistanceTableEntry* pOut_result, const vec3* pPoints, u32 numPoints)
{
	const u32 numPointsMin1 = numPoints-1;
	
	f32 distSoFar = 0.0f;
	
	pOut_result[0].distance = 0.0f;
	pOut_result[0].t = 0.0f;
	
	for(u32 i=1; i<numPoints; ++i)
	{
		const f32 dist = DistVec3(&pPoints[i-1], &pPoints[i]);
		
		distSoFar += dist;
		
		DistanceTableEntry* pEntry = &pOut_result[i];
		pEntry->distance = distSoFar;
		pEntry->t = (f32)i/(f32)numPointsMin1;
	}
}


//When evaluating a spline, pass in a dist (0 to max) to get a T value to pass in
f32 Curve_TValueFromDist(f32 dist, DistanceTableEntry* pDistTable, u32 numDistTableEntries)
{
	for(s32 i=numDistTableEntries-2; i != -1; --i)
	{
		DistanceTableEntry* pEntry = &pDistTable[i];
		if(dist >= pEntry->distance)
		{
			if(i == numDistTableEntries-1)
			{
				return 1.0f;
			}
			else
			{
				DistanceTableEntry* pNextEntry = &pDistTable[i+1];
				const f32 lerpT = (dist-pEntry->distance)/(pNextEntry->distance-pEntry->distance);
				const f32 t = Lerp(pEntry->t, pNextEntry->t, lerpT);
				
				return t;
			}
		}
	}
	
	return 0.0f;
}

