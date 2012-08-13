//
//  CoreObject_Manager.h
//  CoreEngine3D
//
//  Created by JodyMcAdams on 1/3/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#ifndef CoreEngine3D_CoreObjectFactoryManager_h
#define CoreEngine3D_CoreObjectFactoryManager_h

#include "CoreObjects/CoreObject.h"
#include "CoreObjects/CoreObject_Factory.h"
#include "MathTypes.h"
#include "CoreDebug.h"
#include "stddef.h" //for NULL -_-
#include "ArrayUtil.h"
#include <cassert>
#include "Math/MathTypes.h"

class CoreObjectFactoryManager
{
public:
	struct CoreObjectFactoryEntry
	{
		u32 nextEntryByteIndex;	//Set to 0 to terminate list
		CoreObjectFactoryBase* pFactory;
		//u32 pad[2];
	};
	
	CoreObjectFactoryManager();
	~CoreObjectFactoryManager();
	void Init(u32 memorySizeBytes);
	void Clear(); //Clears the factories (like between levels, etc.)
	void Update(f32 timeElapsed);
	void Add(CoreObjectFactoryBase& factory, u32 maxObjects);
	
private:
	u8* m_pMemory;
	u32 m_memorySizeBytes;
	u32 m_currByteIndex;
};


#endif
