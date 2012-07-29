//
//  CoreObjectAttribute.h
//  CoreEngine3D(OSX)
//
//  Created by Jody McAdams on 7/18/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#ifndef CoreEngine3D_OSX__CoreObjectAttribute_h
#define CoreEngine3D_OSX__CoreObjectAttribute_h

#include <string>
#include <sstream>
#include <cassert>

#include "Math/MathTypes.h"
#include "Math/MathUtil.h"
#include "Util/Hash.h"
#include "Util/CoreDebug.h"

static const u32 CoreObjectAttribute_MaxAttributes = 16;

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
	CoreObjectAttributeType_Char32,
};

//Attribute class
class CoreObjectAttribute
{
public:
	void Init(const char* attribName, CoreObjectAttributeType type)
	{
		nameHash = Hash(attribName);
		this->type = type;
		m_classSizeBytes = 0;
		
		const u32 numChars = MinU32(15,(u32)strlen(attribName));	//anti-buffer overrun
		memcpy(name, attribName, numChars);
		name[numChars] = 0;	//Sanity check
	}
	
	u32 GetSizeBytes() const
	{
		return m_classSizeBytes;
	}
	
	virtual void SetValueFromCString(const char* cStr) = 0;
	
	u32 nameHash;
	u8 name[16];
	
	CoreObjectAttributeType type;
protected:
	u32 m_classSizeBytes;
private:
	
};



//Specializations for each attribute type

//ShortText
class CoreObjectAttribute_Char32 : public CoreObjectAttribute
{
public:
	CoreObjectAttribute_Char32(const char* name)
	{
		Init(name,CoreObjectAttributeType_Char32);
		m_classSizeBytes = sizeof(CoreObjectAttribute_Char32);
		memset(value, 0, 32);
		
		hashedValue = 0;
	}
	
	CoreObjectAttribute_Char32(const char* name, const char* defaultValue)
	{
		Init(name,CoreObjectAttributeType_Char32);
		m_classSizeBytes = sizeof(CoreObjectAttribute_Char32);
		
		const u32 numChars = MinU32(31,(u32)strlen(defaultValue));	//anti-buffer overrun
		memcpy(value, defaultValue, numChars);
		value[numChars] = 0;	//Sanity check
		
		//COREDEBUG_PrintDebugMessage("Hello: %s",value);
		
		hashedValue = Hash((const char*)value);
	}
	
	void SetValueFromCString(const char* cStr)
	{
		const u32 numChars = MinU32(31,(u32)strlen(cStr));	//anti-buffer overrun
		memcpy(value, cStr, numChars);
		value[numChars] = 0;	//Sanity check
		
		//COREDEBUG_PrintDebugMessage("Hi: %s",value);
		
		hashedValue = Hash((const char*)value);
	}
	
	u8 value[256];
	u32 hashedValue;
};

//F32
class CoreObjectAttribute_F32 : public CoreObjectAttribute
{
public:
	CoreObjectAttribute_F32(const char* name)
	{
		Init(name,CoreObjectAttributeType_F32);
		m_classSizeBytes = sizeof(CoreObjectAttribute_F32);
		value = 0.0f;
	}
	CoreObjectAttribute_F32(const char* name, f32 defaultValue, f32 minValue = 0.0f, f32 maxValue = 1.0f)
	{
		Init(name,CoreObjectAttributeType_F32);
		m_classSizeBytes = sizeof(CoreObjectAttribute_F32);
		value = defaultValue;
		this->minValue = minValue;
		this->maxValue = maxValue;
	}
	
	void SetValueFromCString(const char* cStr)
	{
		value = atof(cStr);
	}
	
	f32 value;
	f32 minValue;
	f32 maxValue;
};

//S32
class CoreObjectAttribute_S32 : public CoreObjectAttribute
{
public:
	CoreObjectAttribute_S32(const char* name)
	{
		Init(name,CoreObjectAttributeType_S32);
		m_classSizeBytes = sizeof(CoreObjectAttribute_S32);
		value = 0;
	}
	
	CoreObjectAttribute_S32(const char* name, s32 defaultValue, s32 minValue = 0, s32 maxValue = 1024)
	{
		Init(name,CoreObjectAttributeType_S32);
		m_classSizeBytes = sizeof(CoreObjectAttribute_S32);
		value = defaultValue;
		this->minValue = minValue;
		this->maxValue = maxValue;
	}
	
	void SetValueFromCString(const char* cStr)
	{
		value = atoi(cStr);
	}
	
	s32 value;
	s32 minValue;
	s32 maxValue;
};

//U32
class CoreObjectAttribute_U32 : public CoreObjectAttribute
{
public:
	CoreObjectAttribute_U32(const char* name)
	{
		Init(name,CoreObjectAttributeType_U32);
		m_classSizeBytes = sizeof(CoreObjectAttribute_U32);
		value = 0;
	}
	
	CoreObjectAttribute_U32(const char* name, u32 defaultValue, u32 minValue = 0, u32 maxValue = 1024)
	{
		Init(name,CoreObjectAttributeType_U32);
		m_classSizeBytes = sizeof(CoreObjectAttribute_U32);
		value = defaultValue;
		this->minValue = minValue;
		this->maxValue = maxValue;
	}
	
	void SetValueFromCString(const char* cStr)
	{
		value = atoi(cStr);
	}
	
	u32 value;
	u32 minValue;
	u32 maxValue;
};

//CoreUI_Origin
class CoreObjectAttribute_CoreUI_Origin : public CoreObjectAttribute
{
public:
	CoreObjectAttribute_CoreUI_Origin(const char* name)
	{
		Init(name,CoreObjectAttributeType_CoreUI_Origin);
		m_classSizeBytes = sizeof(CoreObjectAttribute_CoreUI_Origin);
		value = CoreUI_Origin_Center;
	}
	
	CoreObjectAttribute_CoreUI_Origin(const char* name, CoreUI_Origin defaultValue)
	{
		Init(name,CoreObjectAttributeType_CoreUI_Origin);
		m_classSizeBytes = sizeof(CoreObjectAttribute_CoreUI_Origin);
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
		numAttributes = 0;
	}

	void Uninit()
	{
		m_data = NULL;
		numAttributes = 0;
		m_currDataSizeBytes = 0;
		m_maxDataSizeBytes = 0;
	}
	
	const CoreObjectAttribute* operator[](s32 index) const
	{
		return GetAttributeByByteIndex(m_byteIndices[index]);
	}
	
	CoreObjectAttribute* operator[](s32 index)
	{
		return GetAttributeByByteIndex(m_byteIndices[index]);
	}
	
	//Returns the byte index for quick access to the attribute
	s32 Add(const CoreObjectAttribute& attrib)
	{
		if(numAttributes == CoreObjectAttribute_MaxAttributes)
		{
			COREDEBUG_PrintDebugMessage("ERROR: AttributeList->Max cannot create more than %d attributes",CoreObjectAttribute_MaxAttributes);
			
			assert(0);
			return -1;
		}
		
		//Make sure we don't add beyond the memory we have
		const u32 sizeBytes = attrib.GetSizeBytes();
		if(sizeBytes + m_currDataSizeBytes >= m_maxDataSizeBytes)
		{
			COREDEBUG_PrintDebugMessage("ERROR: AttributeList->Max attributes reached.  Please increase the memory size.");
			
			assert(0);
			return -1;
		}
		
		//Get the spot in memory and write the attrib to it
		memcpy(&m_data[m_currDataSizeBytes], (u8*)&attrib, sizeBytes);
		
		const u32 byteIndexOut = m_currDataSizeBytes;
		
		m_byteIndices[numAttributes] = byteIndexOut;
	
		//Keep track of the memory we are using
		m_currDataSizeBytes += sizeBytes;
		
		//Keep track of how many attributes we're adding
		++numAttributes;
		
		return byteIndexOut;
	}
	
	
	const CoreObjectAttribute* GetAttributeByByteIndex(u32 byteIndex) const
	{
		CoreObjectAttribute* pAttrib = (CoreObjectAttribute*)&m_data[byteIndex];
		return pAttrib;
	}
	
	CoreObjectAttribute* GetAttributeByByteIndex(u32 byteIndex)
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
	
	u32 numAttributes;
	
private:
	u8* m_data;
	u32 m_maxDataSizeBytes;
	u32 m_currDataSizeBytes;
	u32 m_byteIndices[CoreObjectAttribute_MaxAttributes];
};


#endif
