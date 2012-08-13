//
//  CoreObject_Manager.h
//  CoreEngine3D
//
//  Created by JodyMcAdams on 1/3/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef CoreEngine3D_iOS__CoreObject_Manager_h
#define CoreEngine3D_iOS__CoreObject_Manager_h

#include "CoreObjects/CoreObject.h"
#include "MathTypes.h"
#include "CoreDebug.h"
#include "stddef.h" //for NULL -_-
#include "ArrayUtil.h"
#include <cassert>
#include "MathUtil.h"

class CoreObjectManager;
extern CoreObjectManager* COREOBJECTMANAGER;

#define COREOBJECT_MAX_OBJECTS 4096


class CoreObjectManager
{
	friend class CoreObject;
public:
	CoreObjectManager();
	void Clear();
	bool AddObject(CoreObject* pCoreObject);	//use outside this class is deprecated
	void PrintStatus();
	CoreObject* GetObjectByHandle(CoreObjectHandle handle);
	s32 GetNumEntries(){return m_activeEntryCount;}
private:
	void RemoveObject(CoreObject* pCoreObject);
	void UpdatePointers(CoreObject* pCoreObject);
	
	struct CoreObjectHandleEntry
	{
		CoreObjectHandleEntry();
		explicit CoreObjectHandleEntry(u32 nextFreeIndex);
		
		u32 m_nextFreeIndex : 12;
		u32 m_counter : 15;
		u32 m_active : 1;
		u32 m_endOfList : 1;
		CoreObject* m_pObject;
	};
	
	CoreObjectHandleEntry m_entries[COREOBJECT_MAX_OBJECTS];
	
	s32 m_activeEntryCount;
	u32 m_firstFreeEntry;
};


#endif
