//
//  MuzzleFlareParticle.h
//  CoreEngine3D(OSX)
//
//  Created by Jody McAdams on 2/26/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#ifndef CoreEngine3D_OSX__MuzzleFlareParticle_h
#define CoreEngine3D_OSX__MuzzleFlareParticle_h

#include "../CoreObject.h"
#include "../RenderLayer.h"
#include "../GraphicsTypes.h"

class MuzzleFlareParticle: public CoreObject
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
		vec4 diffuseColor;
		ItemArtDescription* pItemArt;
		RenderLayer renderLayer;
	};
	
	virtual void UpdateHandle();	//Call when the memory location changes
	
	void InitParticle(MuzzleFlareParticle::ParticleSettings *pSettings, const vec3* pPosition, const vec3* pDirection, u32 texIndex);
	virtual void Uninit();
	virtual void Update(f32 timeElapsed);

private:
	ParticleSettings* m_pSettings;
	f32 m_lifeTimer;
	f32 m_timeToLive;
	f32 m_radius;
	vec3 m_position;
	vec3 m_dir;
	vec4 m_diffuseColorStart;
	vec4 m_diffuseColor;
	vec2 m_texcoordOffset;
	CoreObjectHandle m_hRenderable;
};

#endif
