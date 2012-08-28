//
//  CoreObject_Manager.h
//  CoreEngine3D
//
//  Created by JodyMcAdams on 1/3/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#ifndef CoreEngine3D_CoreObjectFactory_h
#define CoreEngine3D_CoreObjectFactory_h

#include "CoreObjects/CoreObject.h"
#include "Math/MathTypes.h"
#include "Math/MathUtil.h"
#include "Util/CoreDebug.h"
#include "Util/ArrayUtil.h"
#include "Util/Hash.h"

#include "stddef.h" //for NULL -_-
#include <cassert>
#include <typeinfo>

#define DECLAREFACTORY(TYPE) extern CoreObjectFactory<TYPE> g_Factory_##TYPE
#define DEFINEFACTORY(TYPE) CoreObjectFactory<TYPE> g_Factory_##TYPE(#TYPE)	//Constructor is called which hashes the #TYPE string and saves it!

#define DECLAREFACTORY_WITHNAME(TYPE,NAME) extern CoreObjectFactory<TYPE> g_Factory_##NAME
#define DEFINEFACTORY_WITHNAME(TYPE,NAME) CoreObjectFactory<TYPE> g_Factory_##NAME

#define FACTORY_MAX_CREATED_TYPES 8

class CoreObjectManager;
extern CoreObjectManager* COREOBJECTMANAGER;

class CoreObjectFactoryBase
{
public:
	virtual bool UpdateObjectList(f32 timeElapsed) = 0;
	virtual void Clear() = 0;
	virtual u32 GetDataSize() = 0;
	virtual void Init(u32 maxObjects, void* pMemLocation = NULL) = 0;
	virtual CoreObject* CreateObjectGeneric(u32 type) = 0;
	virtual void LoadResourcesForType() = 0;
	u32 m_numObjects;
	u32 m_maxObjects;
	u32 m_dataSize;
	u32 m_typeHash;
	bool m_objectsCanUpdate;
	bool m_usedPlacementNew;
	bool m_objectHasBeenDeleted;
	bool m_canAutoSpawn;
};

template <class T>
class CoreObjectFactory : public CoreObjectFactoryBase
{
	friend class CoreObjectManager;
	
public:
	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	CoreObjectFactory(const char* name)
	{
		InitVariables();
		
		m_typeHash = Hash(name);
	}
	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	CoreObjectFactory()
	{
		InitVariables();
	}
	
	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	~CoreObjectFactory()
	{
		if(m_usedPlacementNew == false
		   && m_pObjectList != NULL)
		{
			delete[] m_pObjectList;
		}
	}
	
	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual u32 GetDataSize()
	{
		return m_dataSize;
	}
	
	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual void LoadResourcesForType()
	{
		T::LoadResourcesForType(0);	//TODO: deprecate this type parameter
	}
	
	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Sort(bool (*compareFunc)(const T& lhs, const T& rhs))
	{
		Array_InsertionSort(m_pObjectList, m_numObjects, compareFunc);
		
		for(u32 i=0; i<m_numObjects; ++i)
		{
			m_pObjectList[i].UpdatePointers();
		}
	}
	
	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Sort(u32 startIndex, u32 count, bool (*compareFunc)(const T& lhs, const T& rhs))
	{
		Array_InsertionSort(&m_pObjectList[startIndex], count, compareFunc);
		
		for(u32 i=startIndex; i<startIndex+count; ++i)
		{
			m_pObjectList[i].UpdatePointers();
		}
	}
	
	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Clear()
	{
		for(u32 i=0; i<m_numObjects; ++i)
		{
			m_pObjectList[i].Uninit();
		}
		
		for(u32 i=0; i<m_maxObjects; ++i)
		{
			m_pObjectList[i].m_markedForDeletion = false;
			m_pObjectList[i].InvalidateHandle();
		}
		
		m_numObjects = 0;
	};

	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	CoreObject* CreateObjectGeneric(u32 type)
	{
		return CreateObject(type);
	}
	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	T* CreateObject(u32 type)
	{
		if(m_numObjects == m_maxObjects)
		{
			COREDEBUG_PrintDebugMessage("INSANE ERROR: You can't make any more objects!");

			return NULL;
		}

		T* pObject = &m_pObjectList[m_numObjects];

		if(pObject->Init(type))
		{
			++m_numObjects;

			return pObject;
		}
		
		COREDEBUG_PrintDebugMessage("INSANE ERROR! CoreObjectFactory: Failed to create object!  Could not initialize object.\n");
		
		return NULL;
	}
    
	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
    void UpdateObjectSubList(u32 listStart, u32 num, f32 timeElapsed)
    {
        //Update remaining objects
        for(u32 i=listStart; i<num; ++i)
        {
            T* pCurrObject = &m_pObjectList[i];
            pCurrObject->Update(timeElapsed);
        }
    }
	
	//----------------------------------------------------------------------------
	//Returns true if an object was deleted in case you need
	//to respond to that sort of thing
	//----------------------------------------------------------------------------
	virtual bool UpdateObjectList(f32 timeElapsed)
	{
		//So far nothing has been deleted
		m_objectHasBeenDeleted = false;
		
		//If there's no objects, exit early
		if(m_numObjects == 0)
		{
			return false;
		}

		//Loop through, deleting dead objects
		for(u32 i=0; i<m_numObjects;)
		{
			T* pCurrObject = &m_pObjectList[i];
			
			if(pCurrObject->m_markedForDeletion)
			{
				m_objectHasBeenDeleted = true;

				//Uninitialize object
				//The object's Uninit function should delete everything it spawned.
				pCurrObject->Uninit();

				//This is the last object in the list
				T* pLastObject = &m_pObjectList[m_numObjects-1];

				//If the object is not the last object, overwrite it
				if(pCurrObject != pLastObject)
				{
					//overwrite current enemy with last enemy
					*pCurrObject = *pLastObject;	

					//Memory location of the object has changed.
					//Update pointers to memory in this object (mostly for renderables)
					pCurrObject->UpdatePointers();
				}

				//Decrease the number of objects
				--m_numObjects;
			}
			else
			{
				//If the object was deleted, the next object
				//will already have been copied into it's place.
				//Thus we only increment if nothing was deleted.
				++i;
			}
		}

		if(m_objectsCanUpdate)
		{ 
            //Update remaining objects
            for(u32 i=0; i<m_numObjects; ++i)
            {
                T* pCurrObject = &m_pObjectList[i];
                pCurrObject->Update(timeElapsed);
            }
            
		}

		return m_objectHasBeenDeleted;
	}

	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	virtual void Init(u32 maxObjects, void* pMemLocation = NULL)
	{
		if(pMemLocation == NULL)
		{
			m_pObjectList = new T[maxObjects];
			//m_usedPlacementNew is already set to false by default
		}
		else
		{
			m_pObjectList = new(pMemLocation) T[maxObjects];
			m_usedPlacementNew = true;
		}
		
		m_maxObjects = maxObjects;
		
		Clear();
		
		T::InitClass();
		
		m_dataSize = sizeof(T)*m_maxObjects;
	}
	
	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void SetObjectsCanUpdate(bool objectsCanUpdate)
	{
		m_objectsCanUpdate = objectsCanUpdate;
	}
    
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void InitVariables()
	{
		m_pObjectList = NULL;
		m_numObjects = 0;
		m_maxObjects = 0;
		m_dataSize = 0;
		m_objectsCanUpdate = true;
		m_usedPlacementNew = false;
		m_objectHasBeenDeleted = false;
		m_canAutoSpawn = false;
	}
	
	T* m_pObjectList;
};


#endif
