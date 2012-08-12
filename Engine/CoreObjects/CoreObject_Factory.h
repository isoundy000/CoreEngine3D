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
#include "MathTypes.h"
#include "CoreDebug.h"
#include "stddef.h" //for NULL -_-
#include "ArrayUtil.h"
#include <cassert>
#include "MathUtil.h"

class CoreObjectManager;
extern CoreObjectManager* COREOBJECTMANAGER;

class CoreObjectFactoryBase
{
public:
	virtual bool UpdateObjectList(f32 timeElapsed) = 0;
	virtual void Clear() = 0;
	virtual u32 GetDataSize() = 0;
	virtual void Init(u32 maxObjects, void* pMemLocation = NULL) = 0;
};

template <class T>
class CoreObjectFactory : public CoreObjectFactoryBase
{
	friend class CoreObjectManager;
	
public:
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	CoreObjectFactory()
	{
		m_pObjectList = NULL;
		m_numObjects = 0;
		m_maxObjects = 0;
		m_dataSize = 0;
		m_objectsCanUpdate = true;
		m_usedPlacementNew = false;
		m_objectHasBeenDeleted = false;
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
	void Sort(bool (*compareFunc)(const T& lhs, const T& rhs))
	{
		Array_InsertionSort(m_pObjectList, m_numObjects, compareFunc);
		
		for(u32 i=0; i<m_numObjects; ++i)
		{
			m_pObjectList[i].UpdateHandle();
		}
	}
	
	
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	void Sort(u32 startIndex, u32 count, bool (*compareFunc)(const T& lhs, const T& rhs))
	{
		Array_InsertionSort(&m_pObjectList[startIndex], count, compareFunc);
		
		for(u32 i=startIndex; i<startIndex+count; ++i)
		{
			m_pObjectList[i].UpdateHandle();
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
		m_objectHasBeenDeleted = false;
		
		if(m_numObjects == 0)
		{
			return false;
		}
		
		//Delete dead objects
		bool deletedSomething = false;
		for(u32 i=0; i<m_numObjects;)
		{
			T* pCurrObject = &m_pObjectList[i];
			if(pCurrObject->m_markedForDeletion)
			{
				deletedSomething = true;

				pCurrObject->Uninit();

				T* pLastObject = &m_pObjectList[m_numObjects-1];

				if(pCurrObject != pLastObject)
				{
					//overwrite current enemy with last enemy
					*pCurrObject = *pLastObject;	

					//Memory location of the object has moved so update the handle
					//to point to the new memory location
					pCurrObject->UpdateHandle();
				}
				
				//Last object should now be an invalid object
				pLastObject->m_markedForDeletion = false;
				pLastObject->InvalidateHandle();

				--m_numObjects;
			}
			else
			{
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
		
		m_objectHasBeenDeleted = deletedSomething;
		
		return deletedSomething;
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
    
	//private:
	T* m_pObjectList;
	u32 m_numObjects;
	u32 m_maxObjects;
	u32 m_dataSize;
	bool m_objectsCanUpdate;
	bool m_usedPlacementNew;
	bool m_objectHasBeenDeleted;
};


#endif
