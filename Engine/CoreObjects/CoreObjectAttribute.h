//
//  CoreObjectProperties.h
//  CoreEngine3D(OSX)
//
//  Created by Jody McAdams on 7/18/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#ifndef CoreEngine3D_OSX__CoreObjectProperties_h
#define CoreEngine3D_OSX__CoreObjectProperties_h

#include <string>
#include <sstream>

#include "../MathTypes.h"
#include "../Hash.h"
#include "../CoreDebug.h"

enum CoreUI_Origin
{
	CoreUI_Origin_Center,
	CoreUI_Origin_Left,
	CoreUI_Origin_Right,
	CoreUI_Origin_Top,
	CoreUI_Origin_Bottom,
	CoreUI_Origin_TopLeft,
	CoreUI_Origin_BottomLeft,
	CoreUI_Origin_TopRight,
	CoreUI_Origin_BottomRight,
	CoreUI_Origin_Num,
};

enum CoreObjectAttributeType
{
	CoreObjectAttributeType_U32,
	CoreObjectAttributeType_S32,
	CoreObjectAttributeType_F32,
	CoreObjectAttributeType_CoreUI_Origin,
};

//Attribute class
class CoreObjectAttribute
{
public:
	void Init(const char* name, CoreObjectAttributeType type)
	{
		//this->name = name;
		nameHash = Hash(name);
		this->type = type;
		m_valueSize = 0;
	}
	
	u32 GetSizeBytes() const
	{
		return m_valueSize;
	}
	
	virtual void SetValueFromCString(const char* cStr) = 0;
	
	//Quick and dirty way to get the value
	virtual void* GetValuePointer() = 0;
	
	//std::string name;
	u32 nameHash;
	
	CoreObjectAttributeType type;
protected:
	u32 m_valueSize;
private:
	
};



//Specializations for each attribute type

//F32
class CoreObjectAttribute_F32 : public CoreObjectAttribute
{
public:
	CoreObjectAttribute_F32(const char* name)
	{
		Init(name,CoreObjectAttributeType_F32);
		m_valueSize = sizeof(CoreObjectAttribute_F32);
		value = 0.0f;
	}
	CoreObjectAttribute_F32(const char* name, f32 defaultValue)
	{
		Init(name,CoreObjectAttributeType_F32);
		m_valueSize = sizeof(CoreObjectAttribute_F32);
		value = defaultValue;
	}
	
	void SetValueFromCString(const char* cStr)
	{
		value = atof(cStr);
	}
	
	virtual void* GetValuePointer()
	{
		return &value;
	}
	
	f32 value;
};

//S32
class CoreObjectAttribute_S32 : public CoreObjectAttribute
{
public:
	CoreObjectAttribute_S32(const char* name)
	{
		Init(name,CoreObjectAttributeType_S32);
		m_valueSize = sizeof(CoreObjectAttribute_S32);
		value = 0;
	}
	
	CoreObjectAttribute_S32(const char* name, s32 defaultValue)
	{
		Init(name,CoreObjectAttributeType_S32);
		m_valueSize = sizeof(CoreObjectAttribute_S32);
		value = defaultValue;
	}
	
	void SetValueFromCString(const char* cStr)
	{
		value = atoi(cStr);
	}
	
	virtual void* GetValuePointer()
	{
		return &value;
	}
	
	s32 value;
};

//U32
class CoreObjectAttribute_U32 : public CoreObjectAttribute
{
public:
	CoreObjectAttribute_U32(const char* name)
	{
		Init(name,CoreObjectAttributeType_U32);
		m_valueSize = sizeof(CoreObjectAttribute_U32);
		value = 0;
	}
	
	CoreObjectAttribute_U32(const char* name, u32 defaultValue)
	{
		Init(name,CoreObjectAttributeType_U32);
		m_valueSize = sizeof(CoreObjectAttribute_U32);
		value = defaultValue;
	}
	
	void SetValueFromCString(const char* cStr)
	{
		value = atoi(cStr);
	}
	
	virtual void* GetValuePointer()
	{
		return &value;
	}
	
	u32 value;
};

//CoreUI_Origin
class CoreObjectAttribute_CoreUI_Origin : public CoreObjectAttribute
{
public:
	CoreObjectAttribute_CoreUI_Origin(const char* name)
	{
		Init(name,CoreObjectAttributeType_CoreUI_Origin);
		m_valueSize = sizeof(CoreObjectAttribute_CoreUI_Origin);
		value = CoreUI_Origin_Center;
	}
	
	CoreObjectAttribute_CoreUI_Origin(const char* name, CoreUI_Origin defaultValue)
	{
		Init(name,CoreObjectAttributeType_CoreUI_Origin);
		m_valueSize = sizeof(CoreObjectAttribute_CoreUI_Origin);
		value = defaultValue;
	}
	
	void SetValueFromCString(const char* cStr)
	{
		//Set a default for sanity
		value = CoreUI_Origin_Center;
		
		//Do tons of terrible string compares
		if(strcmp(cStr, "center") == 0)
		{
			value = CoreUI_Origin_Center;
		}
		else if(strcmp(cStr, "left") == 0)
		{
			value = CoreUI_Origin_Left;
		}
		else if(strcmp(cStr, "right") == 0)
		{
			value = CoreUI_Origin_Right;
		}
		else if(strcmp(cStr, "top") == 0)
		{
			value = CoreUI_Origin_Top;
		}
		else if(strcmp(cStr, "bottom") == 0)
		{
			value = CoreUI_Origin_Bottom;
		}
		else if(strcmp(cStr, "topLeft") == 0)
		{
			value = CoreUI_Origin_TopLeft;
		}
		else if(strcmp(cStr, "bottomLeft") == 0)
		{
			value = CoreUI_Origin_BottomLeft;
		}
		else if(strcmp(cStr, "topRight") == 0)
		{
			value = CoreUI_Origin_TopRight;
		}
		else if(strcmp(cStr, "bottomRight") == 0)
		{
			value = CoreUI_Origin_BottomRight;
		}
	}
	
	virtual void* GetValuePointer()
	{
		return &value;
	}
	
	CoreUI_Origin value;
};


class CoreObjectAttributeList
{
public:
	void Init(u8* pDataBuffer, u32 numBytes)
	{
		//Allocate some memory for the attributes
		m_data = pDataBuffer;
		m_currDataSizeBytes = 0;
		m_maxDataSizeBytes = numBytes;
		m_numAttributes = 0;
	}

	void Uninit()
	{
		m_data = NULL;
		m_numAttributes = 0;
		m_currDataSizeBytes = 0;
		m_maxDataSizeBytes = 0;
	}
	
	const CoreObjectAttribute* operator[](s32 byteIndex) const
	{
		return GetAttributeByByteIndex(byteIndex);
	}

	
	//Returns the byte index for quick access to the attribute
	s32 Add(const CoreObjectAttribute& attrib)
	{
		//Make sure we don't add beyond the memory we have
		const u32 sizeBytes = attrib.GetSizeBytes();
		if(sizeBytes + m_currDataSizeBytes >= m_maxDataSizeBytes)
		{
			COREDEBUG_PrintDebugMessage("ERROR: AttributeList->Max attributes reached.  Please increase the memory size.");
			return -1;
		}
		
		//Get the spot in memory and write the attrib to it
		memcpy(&m_data[m_currDataSizeBytes], (u8*)&attrib, sizeBytes);
		
		const u32 byteIndexOut = m_currDataSizeBytes;
	
		//Keep track of the memory we are using
		m_currDataSizeBytes += sizeBytes;
		
		//Keep track of how many attributes we're adding
		++m_numAttributes;
		
		return byteIndexOut;
	}
	
	
	const CoreObjectAttribute* GetAttributeByByteIndex(u32 byteIndex) const
	{
		CoreObjectAttribute* pAttrib = (CoreObjectAttribute*)&m_data[byteIndex];
		return pAttrib;
	}
	
	
	const CoreObjectAttribute* GetAttributeByName(const char* name) const
	{
		const u32 nameHash = Hash(name);
		
		return (const CoreObjectAttribute*)GetAttributeByNameHash(nameHash);
	}
	

	CoreObjectAttribute* GetAttributeByNameHash(u32 nameHash) const
	{
		u32 byteIndex = 0;
		while(byteIndex < m_currDataSizeBytes)
		{
			CoreObjectAttribute* pAttrib = (CoreObjectAttribute*)&m_data[byteIndex];
			if(pAttrib->nameHash == nameHash)
			{
				return pAttrib;
			}
			
			byteIndex += pAttrib->GetSizeBytes();
		}
		
		COREDEBUG_PrintDebugMessage("Warning: AttributeList->The attribute could not be found.  This is probably fine.");
		
		return NULL;
	}
	
	
	void SetValueForAttribByCString(const char* attribName, const char* valueString)
	{
		const u32 nameHash = Hash(attribName);
		
		CoreObjectAttribute* pAttrib = GetAttributeByNameHash(nameHash);
		if(pAttrib != NULL)
		{
			pAttrib->SetValueFromCString(valueString);
		}
	}
	
	
private:
	u8* m_data;
	u32 m_maxDataSizeBytes;
	u32 m_currDataSizeBytes;
	u32 m_numAttributes;
};


#endif
