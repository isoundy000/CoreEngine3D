//
//  MuzzleFlareParticle_2D.h
//  CoreEngine3D
//
//  Created by Jody McAdams on 2/26/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#ifndef CoreEngine3D_OSX__MuzzleFlareParticle_2D_h
#define CoreEngine3D_OSX__MuzzleFlareParticle_2D_h

#include "CoreObjects/CoreObject.h"
#include "Graphics/RenderLayer.h"
#include "Graphics/GraphicsTypes.h"
#include "CoreObjects/CoreObject_Factory.h"

class MuzzleFlareParticle_2D: public CoreObject
{
public:
	struct ParticleSettings
	{
		RenderMaterial renderMaterial;
		u32 renderFlags;
		BlendMode blendMode;
		f32 lifetimeMin;
		f32 lifetimeMax;
		f32 radiusMin;
		f32 radiusMax;
		f32 radiusScaleStart;
		f32 radiusScaleIdle;
		f32 radiusScaleEnd;
		f32 heightScaleStart;
		f32 heightScaleIdle;
		f32 heightScaleEnd;
		f32 widthScaleStart;
		f32 widthScaleIdle;
		f32 widthScaleEnd;
		vec4 diffuseColor;
		f32 lerpT_Idle;
		f32 lerpT_End;
		TextureAsset* pItemArt;
		RenderLayer renderLayer;
	};
	
	virtual void UpdatePointers();	//Call when the memory location changes
	
	void InitParticle(MuzzleFlareParticle_2D::ParticleSettings *pSettings, CoreObjectHandle hParentRenderable, mat4f localMat, u32 texIndex);
	virtual void Uninit();
	virtual void Update(f32 timeElapsed);

private:
	void UpdateAttachment();
	
	ParticleSettings* m_pSettings;
	f32 m_lifeTimer;
	f32 m_timeToLive;
	f32 m_radius;
	f32 m_computedRadius;
	vec3 m_position;
	vec3 m_dir;
	vec4 m_diffuseColorStart;
	vec4 m_diffuseColor;
	CoreObjectHandle m_hRenderable;
	f32 m_heightScale;
	f32 m_widthScale;
	mat4f m_localMat;
	
	CoreObjectHandle m_hParentRenderable;
};

DECLAREFACTORY(MuzzleFlareParticle_2D);

#endif
