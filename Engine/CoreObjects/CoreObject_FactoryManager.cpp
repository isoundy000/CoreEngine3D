//
//  CoreObject_FactoryManager.cpp
//  CoreEngine3D
//
//  Created by JodyMcAdams on 1/3/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

// Thanks to http://gamesfromwithin.com/managing-data-relationships for the original code

#include "CoreObject_FactoryManager.h"
#include "stddef.h" //for NULL -_-

CoreObjectFactoryManager::CoreObjectFactoryManager()
{
	m_pMemory = NULL;
	m_memorySizeBytes = 0;
	m_currByteIndex = 0;
}


void CoreObjectFactoryManager::Init(u32 memorySizeBytes)
{
	m_pMemory = new u8[memorySizeBytes];
	assert(m_pMemory != NULL);
	m_memorySizeBytes = memorySizeBytes;
}


void CoreObjectFactoryManager::Clear()
{
	u32 currByteIndex = 0;
	
	while (1)
	{
		CoreObjectFactoryEntry* pEntry = (CoreObjectFactoryEntry*)&m_pMemory[currByteIndex];
		
		//Once the next byte index is 0, we are at the end
		if(pEntry->nextEntryByteIndex == 0)
		{
			break;
		}
		
		//Update the factory
		pEntry->pFactory->Clear();
		
		//Set the next memory location
		currByteIndex = pEntry->nextEntryByteIndex;
	}
}


void CoreObjectFactoryManager::Update(f32 timeElapsed)
{
	u32 currByteIndex = 0;
	
	while (1)
	{
		CoreObjectFactoryEntry* pEntry = (CoreObjectFactoryEntry*)&m_pMemory[currByteIndex];

		//Once the next byte index is 0, we are at the end
		if(pEntry->nextEntryByteIndex == 0)
		{
			break;
		}
		
		//Update the factory
		pEntry->pFactory->UpdateObjectList(timeElapsed);
		
		//Set the next memory location
		currByteIndex = pEntry->nextEntryByteIndex;
	}
}
