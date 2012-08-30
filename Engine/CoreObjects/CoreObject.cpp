//
//  CoreObject.cpp
//  CoreEngine3D
//
//  Created by JodyMcAdams on 1/3/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "CoreObject.h"
#include "CoreObject_Manager.h"

#include <cassert>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CoreObjectHandle CoreObject::GetHandle() const
{
	return handle;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CoreObject::DeleteObject()
{
	m_markedForDeletion = true;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CoreObject::InvalidateHandle()
{
	handle = 0;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
u32 CoreObject::GetEntityType()
{
	return m_entityTypeHash;
}


//----------------------------------------------------------------------------
//Called when the memory location changes
//----------------------------------------------------------------------------
void CoreObject::UpdatePointers()
{
	//assert(m_markedForDeletion == false);
	
	COREOBJECTMANAGER->UpdatePointers(this);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool CoreObject::Init(u32 type)
{
	m_markedForDeletion = false;
	m_entityTypeHash = type;
	m_canUpdate = true;

	return COREOBJECTMANAGER->AddObject(this);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CoreObject::Uninit()
{
	//assert(m_markedForDeletion == true);
	
	COREOBJECTMANAGER->RemoveObject(this);
}
