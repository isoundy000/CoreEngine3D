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
void CoreObjectFactoryManager::Add(CoreObjectFactoryBase& factory, u32 maxObjects)
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
