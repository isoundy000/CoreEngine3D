//Author: Jody McAdams
//Website: http://www.jodymcadams.com
//Company Website: http://www.gamejellyonline.com

#ifndef __MATHUTIL_H
#define __MATHUTIL_H

#include "MathTypes.h"

#define PI 3.141592653589f

#define SQUARE(X) ((X) * (X))

//1D
s32 PowerOf2(s32 num);
f32 DegreesToRadians(f32 degrees);
f32 Lerp(f32 f0, f32 f1, f32 t);
s32	  ClampS32(s32 n, s32 clampMin, s32 clampMax);
u32	  ClampU32(u32 n, u32 clampMin, u32 clampMax);
f32 ClampF(f32 n, f32 clampMin, f32 clampMax);
f32 MaxF(f32 l, f32 r);
s32 MaxS32(int l, int r);
s32 MinS32(int l, int r);
u32 MaxU32(unsigned int l, unsigned int r);
u32 MinU32(unsigned int l, unsigned int r);
f32 MinF(f32 l, f32 r); 
void SwapS32(s32* l, s32* r);

//2D
void  SetVec2(vec2* out_resultVec,f32 x, f32 y);
f32 DotVec2(const vec2* vec0, const vec2* vec1);
void  LerpVec2(vec2* out_lerpedPos, vec2* p0, vec2* p1, f32 t);
f32  NormalizeVec2_Self(vec2* vec);
f32  NormalizeVec2(vec2* out_resultVec, const vec2* vec);
f32  TryNormalizeVec2_Self(vec2* vec);
f32 MagnitudeVec2(const vec2* vec);
f32 MagnitudeSqVec2(const vec2* vec);
f32 DistSqVec2(const vec2* vec0, const vec2* vec1);
f32 DistVec2(const vec2* vec0, const vec2* vec1);
void  SubVec2(vec2* out_resultVec, const vec2* p0, const vec2* p1);
void  SubVec2_Self(vec2* out_resultVec, const vec2* p);
void  ScaleVec2_Self(vec2* vec, f32 scale);
void  RotateVec2(vec2* out_resultVec, const vec2* vec, f32 sinTheta, f32 cosTheta);
void  RotateVec2_Self(vec2* vec, f32 sinTheta, f32 cosTheta);
void  ScaleVec2(vec2* out_resultVec, const vec2* vec, f32 scale);
void  AddVec2_Self(vec2* out_resultVec, const vec2* vec);
void  AddVec2(vec2* out_resultVec, const vec2* vec0, const vec2* vec1);
void  AddScaledVec2_Self(vec2* out_resultVec, const vec2* vec, f32 scale);
void  AddScaledVec2(vec2* out_resultVec, const vec2* vec0, const vec2* vec1, f32 scale);
void  SubScaledVec2_Self(vec2* out_resultVec, vec2* vec, f32 scale);
void  SubScaledVec2(vec2* out_resultVec, vec2* vec0, vec2* vec1, f32 scale);
void  CopyVec2(vec2* out_result, const vec2* point);
void  CreatePlaneFromPointsVec2(struct PlaneVec2* out_resultPlane, vec2* p0, vec2* p1);
bool  PointInsidePlaneVec2(vec2* point,struct PlaneVec2* plane);
bool  PointInsidePlaneVec2(vec2* point, struct PlaneVec2* plane);
f32 AngleBetweenVec2(const vec2* vec0, const vec2* vec1);
f32 AngleOfVec2(const vec2* vec0);
void  MakeRotatedVec2(vec2* out_resultVec, f32 angleRadians);
void  MakeRotatedVec2_SinCos(vec2* out_resultVec, f32 sinTheta, f32 cosTheta);
//Projects vec0 onto normalized vec1
void ProjectVec2_Norm(vec2* out_resultVec, const vec2* vec0, const vec2* vec1);

//3D
bool ApproxEqualsZeroVec3(vec3* pTestvec);
void  SetVec3(vec3* out_resultVec,f32 x, f32 y, f32 z);
void  SetVec4(vec4* out_resultVec,f32 x, f32 y, f32 z, f32 w);
void  CrossVec3(vec3* out_resultVec, const vec3* vec0, const vec3* vec1);
f32 DotVec3(const vec3* vec0, const vec3* vec1);
void  LerpVec3(vec3* out_lerpedPos, const vec3* p0, const vec3* p1, f32 t);
void  LerpVec4(vec4* out_lerpedPos, const vec4* p0, const vec4* p1, f32 t);
f32  NormalizeVec3_Self(vec3* vec);
f32  TryNormalizeVec3(vec3* out_resultVec, const vec3* vec);
f32  TryNormalizeVec3_Self(vec3* vec);
f32  NormalizeVec3(vec3* out_resultVec, const vec3* vec);
f32 MagnitudeVec3(const vec3* vec);

const vec2* AsVec2(const vec3* pVec);
vec2* AsVec2(vec3* pVec);

bool  PosIsBehind(const vec3* frontPos, const vec3* frontAt, const vec3* behindPos);

f32 DistSqVec3(const vec3* vec0, const vec3* vec1);
f32 DistVec3_2D(const vec3* vec0, const vec3* vec1);
f32 DistVec3(const vec3* vec0, const vec3* vec1);
f32 MagnitudeSqVec3(const vec3* vec);
f32 Ease_InOut_Cubic(f32 t, f32 t5, f32 t4, f32 t3, f32 t2, f32 t1);
f32 Ease_InOut_Cubic_Simple(f32 t);

void CatmullRom_Evaluate_Vec3(vec3* pOut_result, const vec3* p0, const vec3* p1, const vec3* p2, const vec3* p3, f32 t);
f32 CatmullRom_Evaluate_F32(f32 p0, f32 p1, f32 p2, f32 p3, f32 t);

void CatmullRom_EvaluateCurve_Vec3(vec3* pOut_result, const vec3* pPoints, u32 numPoints, f32 t);
f32 CatmullRom_EvaluateCurve_F32(const f32* pPoints, u32 numPoints, f32 t);

void CatmullRom_CreatePoints(vec3* pOut_result, u32 numPointsDesired, const vec3* pCurvePoints, u32 numCurvePoints);
void CatmullRom_CreatePoints_Uniform(vec3* pOut_pointArray, u32 numPointsDesired, const vec3* pCurvePoints, u32 numCurvePoints, DistanceTableEntry* pDistTable, u32 numDistTableEntries);

void Curve_CreateDistanceTable(DistanceTableEntry* pOut_result, const vec3* pPoints, u32 numPoints);
f32 Curve_TValueFromDist(f32 dist, DistanceTableEntry* pDistTable, u32 numDistTableEntries);

//Projects vec0 onto normalized vec1
void ProjectVec3_Norm(vec3* out_resultVec, vec3* vec0, vec3* vec1);
void  AddVec3ThenScale_Self(vec3* out_resultVec, vec3* vec, f32 scale);
void  AddVec3ThenScale(vec3* out_resultVec, vec3* vec0, vec3* vec1, f32 scale);
void  AddVec3_Self(vec3* out_resultVec, const vec3* vec);
void  AddVec3(vec3* out_resultVec, const vec3* vec0, const vec3* vec1);
void  AddScaledVec3_Self(vec3* out_resultVec, const vec3* vec, f32 scale);
void  AddScaledVec3(vec3* out_resultVec, const vec3* vec0, const vec3* vec1, f32 scale);
void  SubScaledVec3_Self(vec3* out_resultVec, vec3* vec, f32 scale);
void  SubScaledVec3(vec3* out_resultVec, vec3* vec0, vec3* vec1, f32 scale);
void  SubVec3(vec3* out_resultVec, const vec3* p0, const vec3* p1);
void  SubVec3_Self(vec3* out_resultVec, const vec3* p0);
void  ScaleVec3_Self(vec3* vec, f32 scale);
void  ScaleVec3(vec3* out_resultVec, const vec3* vec, f32 scale);
void  ScaleVec4(vec4* out_resultVec, const vec4* vec, f32 scale);
void  ScaleVec4_Self(vec4* out_resultVec, f32 scale);
void  CopyVec3(vec3* out_result, const vec3* point);
void  CopyVec4(vec4* out_result, const vec4* point);
void  CopyVec3IntoVec4(vec4* out_result, const vec3* point);
void  ReflectVec2_Self(vec2* vec, const vec2* normal);
void  ReflectVec2(vec2* out_resultVec, vec2* vec, const vec2* normal);
void  MakeRandomVec3WithinBounds(vec3* pOut_resultVec, f32 x0, f32 x1, f32 y0, f32 y1, f32 z0, f32 z1);
f32 AngleBetweenVec3(vec3* vec0, vec3* vec1);
bool rand_Bool();
f32 rand_FloatRange(f32 a, f32 b);
s32 rand_IntRange(s32 a, s32 b);
void RotateVec3AboutAxis(vec3* out_resultVec,const vec3* vec0, const vec3* pAxis,f32 sinAngle, f32 cosAngle);
void RotateVec3AboutAxis_Self(vec3* out_resultVec,const vec3* pAxis,f32 sinAngle, f32 cosAngle);

void RGBAToVec4(vec4* pOut_vec,u8 r, u8 g, u8 b, u8 a);
#endif
