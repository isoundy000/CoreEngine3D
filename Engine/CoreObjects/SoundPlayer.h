//
//  SoundPlayer.h
//  Game(iOS)
//
//  Created by Jody McAdams on 12/27/11.
//  Copyright (c) 2011 Jody McAdams. All rights reserved.
//

#ifndef __SoundPlayer_h
#define __SoundPlayer_h

#include "Math/MathTypes.h"
#include "Graphics/GraphicsTypes.h"

#include "CoreObjects/CoreObject.h"
#include "Game/Game.h"
#include "CoreObjects/CoreObject_Factory.h"

class SoundPlayer: public CoreGameObject
{
public:
	virtual bool Init(u32 type);	//Init variables
	virtual void Uninit();
	virtual void ProcessMessage(u32 message, u32 parameter);	//Pass in a hash value

	virtual bool SpawnInit(void* pSpawnStruct);
	void SpawnInit(const vec3* pPos, u32 soundBufferID, f32 volume, f32 pitch, bool isLooping, CoreObjectHandle hParent);
	virtual void Update(f32 timeElapsed);
private:	

	u32 m_soundSource;
	CoreObjectHandle m_hParent;	//optional: parent to update position from
	bool m_isAttachedToParent;
};

void SoundPlayer_Init();

DECLAREFACTORY(SoundPlayer);

#endif
