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
		u32 pad[2];
	};
	
	CoreObjectFactoryManager();
	~CoreObjectFactoryManager();
	void Init(u32 memorySizeBytes);
	void Clear(); //Clears the factories (like between levels, etc.)
	void Update(f32 timeElapsed);
	
	template <class T>
	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Add(CoreObjectFactory<T>& factory, u32 maxObjects)
	{
		//To start, the current byte index is at the beginning of the current entry
		
		//Create new entry at the current location
		CoreObjectFactoryEntry* pNewEntry = (CoreObjectFactoryEntry*)&m_pMemory[m_currByteIndex];
		
		//Save a pointer to the factory
		pNewEntry->pFactory = &factory;
		
		//Now the byte index becomes the location of the memory to hold all the factory objects
		m_currByteIndex += sizeof(CoreObjectFactoryEntry);
		
		//Init factory using memory at the current location
		u8* pMemLoc = &m_pMemory[m_currByteIndex];
		factory.Init(maxObjects,pMemLoc);
		
		//Now the byte index will be at the beginning of the next entry
		const u32 dataSize = factory.GetDataSize();
		m_currByteIndex += dataSize;
		
		//Save the location
		pNewEntry->nextEntryByteIndex = m_currByteIndex;
		
		//Initialize the last entry as a terminator
		CoreObjectFactoryEntry* pLastEntry = (CoreObjectFactoryEntry*)&m_pMemory[m_currByteIndex];
		pLastEntry->nextEntryByteIndex = 0;
		
		const u32 memUsage = pLastEntry->nextEntryByteIndex+sizeof(CoreObjectFactoryEntry);
		
		COREDEBUG_PrintDebugMessage("FactoryManager->Add: Current mem usage is: %d bytes, %.2fMB",memUsage, ((f32)memUsage) / SQUARE(1024.0f) );
	}
	
private:
	u8* m_pMemory;
	u32 m_memorySizeBytes;
	u32 m_currByteIndex;
};


#endif
