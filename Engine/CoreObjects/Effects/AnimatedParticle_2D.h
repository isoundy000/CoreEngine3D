//
//  AnimatedParticle_2D.h
//  CoreEngine3D(OSX)
//
//  Created by Jody McAdams on 2/26/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#ifndef CoreEngine3D_OSX__AnimatedParticle_2D_h
#define CoreEngine3D_OSX__AnimatedParticle_2D_h

#include "../CoreObject.h"
#include "../RenderLayer.h"
#include "../GraphicsTypes.h"
#include "../AnimationController.h"
#include "../SpriteSheetAnimator.h"


class AnimatedParticle_2D: public CoreObject
{
public:
	struct ParticleSettings
	{
		AnimationSet* pAnimSet;	//required
		SpriteSheetAnimator* pSpriteAnimator; //required
		RenderMaterial renderMaterial;
		u32 renderFlags;
		BlendMode blendMode;
		f32 lifetimeMin;
		f32 lifetimeMax;
		f32 fadeTimeStart;
		f32 fadeTimeEnd;
		f32 gravity;
		f32 moveSpeedMin;
		f32 moveSpeedMax;
		f32 spinSpeedMin;
		f32 spinSpeedMax;
		bool randomlyFlipSpin;
		f32 radiusMin;
		f32 radiusMax;
		f32 radiusScale_start;
		f32 radiusScale_end;
		vec4 diffuseColor;
		TextureAsset* pItemArt;
		RenderLayer renderLayer;
		u32 categoryFlags;
	};
	
	//static void InitClass(){};
	
	virtual void UpdateHandle();	//Call when the memory location changes
	
	//virtual bool Init(u32 type);
	//virtual bool SpawnInit(void* pSpawnStruct){return true;}
	//virtual bool PostSpawnInit(void* pSpawnStruct){return true;}
	void InitParticle(ParticleSettings *pSettings, const vec3* pPosition, const vec3* pDirection, f32 startAngle, s32 animID, u32 frameOffset, f32 playSpeed);
	u32 GetCategoryFlags();
	virtual void Uninit();
	virtual void Update(f32 timeElapsed);
	virtual const vec3* GetPosition() const;
	void AddVelocity(const vec3* pVelAdd);
	//virtual void ProcessMessage(u32 message){};	//Pass in a hash value
private:
	ParticleSettings* m_pSettings;
	f32 m_spinSpeed;
	f32 m_lifeTimer;
	f32 m_totalLifeTime;
	f32 m_fadeTimeStart;
	f32 m_fadeTimeEnd;
	f32 m_radiusStart;
	f32 m_radiusEnd;
	vec3 m_velocity;
	vec3 m_position;
	vec4 m_diffuseColorStart;
	vec4 m_diffuseColor;
	vec2 m_texcoordOffset;
	f32 m_currSpinAngle;
	CoreObjectHandle m_hRenderable;
	AnimationPlayer m_animPlayer;
};

#endif
