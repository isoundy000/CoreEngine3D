//
//  TileAffector.h
//  Game(iOS)
//
//  Created by Jody McAdams on 12/27/11.
//  Copyright (c) 2011 Jody McAdams. All rights reserved.
//

#ifndef __TileAffector_h
#define __TileAffector_h

#include "Math/MathTypes.h"
#include "Graphics/GraphicsTypes.h"

#include "CoreObjects/CoreObject.h"
#include "CoreObjects/CoreObject_Factory.h"
#include "Game/Game.h"

class TileAffector: public CoreObject
{
public:
	virtual void Uninit();
	virtual void ProcessMessage(u32 message, u32 parameter);	//Pass in a hash value

	virtual bool SpawnInit(void* pSpawnStruct);
	
private:	
	u32 m_tileIndex_X;
	u32 m_tileIndex_Y;
};

void TileAffector_Init();

DECLAREFACTORY(TileAffector);

#endif
