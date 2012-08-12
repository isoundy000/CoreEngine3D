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


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CoreObjectFactoryManager::CoreObjectFactoryManager()
{
	m_pMemory = NULL;
	m_memorySizeBytes = 0;
	m_currByteIndex = 0;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CoreObjectFactoryManager::~CoreObjectFactoryManager()
{
	if(m_pMemory != NULL)
	{
		delete[] m_pMemory;
	}
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CoreObjectFactoryManager::Init(u32 memorySizeBytes)
{
	m_pMemory = new u8[memorySizeBytes];
	assert(m_pMemory != NULL);
	m_memorySizeBytes = memorySizeBytes;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CoreObjectFactoryManager::Clear()
{
	CoreObjectFactoryEntry* pEntry = (CoreObjectFactoryEntry*)&m_pMemory[0];
	
	while (pEntry->nextEntryByteIndex != 0)
	{
		//Clear the factory
		pEntry->pFactory->Clear();
		
		//Get the next entry
		pEntry = (CoreObjectFactoryEntry*)&m_pMemory[pEntry->nextEntryByteIndex];
	}
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CoreObjectFactoryManager::Update(f32 timeElapsed)
{
	CoreObjectFactoryEntry* pEntry = (CoreObjectFactoryEntry*)&m_pMemory[0];
	
	while (pEntry->nextEntryByteIndex != 0)
	{
		//Update the factory
		pEntry->pFactory->UpdateObjectList(timeElapsed);
		
		//Get the next entry
		pEntry = (CoreObjectFactoryEntry*)&m_pMemory[pEntry->nextEntryByteIndex];
	}
}
