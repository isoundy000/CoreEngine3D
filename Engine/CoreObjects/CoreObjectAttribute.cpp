//
//  CoreObjectAttribute.h
//  CoreEngine3D
//
//  Created by Jody McAdams on 7/18/12.
//  Copyright (c) 2012 Jody McAdams. All rights reserved.
//

#include "CoreObjectAttribute.h"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CoreObjectAttribute::Init(const char* attribName, CoreObjectAttributeType type)
{
	nameHash = Hash(attribName);
	this->type = type;
	m_classSizeBytes = 0;
	
	const s32 numChars = MinS32(15,(s32)strlen(attribName));	//anti-buffer overrun
	memcpy(name, attribName, numChars);
	name[numChars] = 0;	//Sanity check
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
u32 CoreObjectAttribute::GetSizeBytes() const
{
	return m_classSizeBytes;
}


//Specializations for each attribute type


//CHAR32

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CoreObjectAttribute_Char32::CoreObjectAttribute_Char32(const char* name)
{
	Init(name,CoreObjectAttributeType_Char32);
	m_classSizeBytes = sizeof(CoreObjectAttribute_Char32);
	memset(value, 0, 32);
	
	hashedValue = 0;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CoreObjectAttribute_Char32::CoreObjectAttribute_Char32(const char* name, const char* defaultValue)
{
	Init(name,CoreObjectAttributeType_Char32);
	m_classSizeBytes = sizeof(CoreObjectAttribute_Char32);
	
	const s32 numChars = MinS32(31,(s32)strlen(defaultValue));	//anti-buffer overrun
	memcpy(value, defaultValue, numChars);
	value[numChars] = 0;	//Sanity check
	
	//COREDEBUG_PrintDebugMessage("Hello: %s",value);
	
	hashedValue = Hash((const char*)value);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CoreObjectAttribute_Char32::SetValueFromCString(const char* cStr)
{
	const s32 numChars = MinS32(31,(s32)strlen(cStr));	//anti-buffer overrun
	memcpy(value, cStr, numChars);
	value[numChars] = 0;	//Sanity check
	
	//COREDEBUG_PrintDebugMessage("Hi: %s",value);
	
	hashedValue = Hash((const char*)value);
}

//F32

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CoreObjectAttribute_F32::CoreObjectAttribute_F32(const char* name)
{
	Init(name,CoreObjectAttributeType_F32);
	m_classSizeBytes = sizeof(CoreObjectAttribute_F32);
	value = 0.0f;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CoreObjectAttribute_F32::CoreObjectAttribute_F32(const char* name, f32 defaultValue, f32 minValue, f32 maxValue)
{
	Init(name,CoreObjectAttributeType_F32);
	m_classSizeBytes = sizeof(CoreObjectAttribute_F32);
	value = defaultValue;
	this->minValue = minValue;
	this->maxValue = maxValue;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CoreObjectAttribute_F32::SetValueFromCString(const char* cStr)
{
	value = atof(cStr);
}

//S32

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CoreObjectAttribute_S32::CoreObjectAttribute_S32(const char* name)
{
	Init(name,CoreObjectAttributeType_S32);
	m_classSizeBytes = sizeof(CoreObjectAttribute_S32);
	value = 0;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CoreObjectAttribute_S32::CoreObjectAttribute_S32(const char* name, s32 defaultValue, s32 minValue, s32 maxValue)
{
	Init(name,CoreObjectAttributeType_S32);
	m_classSizeBytes = sizeof(CoreObjectAttribute_S32);
	value = defaultValue;
	this->minValue = minValue;
	this->maxValue = maxValue;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CoreObjectAttribute_S32::SetValueFromCString(const char* cStr)
{
	value = atoi(cStr);
}


//U32

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CoreObjectAttribute_U32::CoreObjectAttribute_U32(const char* name)
{
	Init(name,CoreObjectAttributeType_U32);
	m_classSizeBytes = sizeof(CoreObjectAttribute_U32);
	value = 0;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CoreObjectAttribute_U32::CoreObjectAttribute_U32(const char* name, u32 defaultValue, u32 minValue, u32 maxValue)
{
	Init(name,CoreObjectAttributeType_U32);
	m_classSizeBytes = sizeof(CoreObjectAttribute_U32);
	value = defaultValue;
	this->minValue = minValue;
	this->maxValue = maxValue;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CoreObjectAttribute_U32::SetValueFromCString(const char* cStr)
{
	value = atoi(cStr);
}

//CoreUI_Origin

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CoreObjectAttribute_CoreUI_Origin::CoreObjectAttribute_CoreUI_Origin(const char* name)
{
	Init(name,CoreObjectAttributeType_CoreUI_Origin);
	m_classSizeBytes = sizeof(CoreObjectAttribute_CoreUI_Origin);
	value = CoreUI_Origin_Center;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CoreObjectAttribute_CoreUI_Origin::CoreObjectAttribute_CoreUI_Origin(const char* name, CoreUI_Origin defaultValue)
{
	Init(name,CoreObjectAttributeType_CoreUI_Origin);
	m_classSizeBytes = sizeof(CoreObjectAttribute_CoreUI_Origin);
	value = defaultValue;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CoreObjectAttribute_CoreUI_Origin::SetValueFromCString(const char* cStr)
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


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CoreObjectAttributeList::Init(u8* pDataBuffer, u32 numBytes)
{
	//Allocate some memory for the attributes
	m_data = pDataBuffer;
	m_currDataSizeBytes = 0;
	m_maxDataSizeBytes = numBytes;
	numAttributes = 0;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CoreObjectAttributeList::Uninit()
{
	m_data = NULL;
	numAttributes = 0;
	m_currDataSizeBytes = 0;
	m_maxDataSizeBytes = 0;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const CoreObjectAttribute* CoreObjectAttributeList::operator[](s32 index) const
{
	return GetAttributeByByteIndex(m_byteIndices[index]);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CoreObjectAttribute* CoreObjectAttributeList::operator[](s32 index)
{
	return GetAttributeByByteIndex(m_byteIndices[index]);
}


//----------------------------------------------------------------------------
//Returns the byte index for quick access to the attribute
//----------------------------------------------------------------------------
s32 CoreObjectAttributeList::Add(const CoreObjectAttribute& attrib)
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


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const CoreObjectAttribute* CoreObjectAttributeList::GetAttributeByByteIndex(u32 byteIndex) const
{
	CoreObjectAttribute* pAttrib = (CoreObjectAttribute*)&m_data[byteIndex];
	return pAttrib;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CoreObjectAttribute* CoreObjectAttributeList::GetAttributeByByteIndex(u32 byteIndex)
{
	CoreObjectAttribute* pAttrib = (CoreObjectAttribute*)&m_data[byteIndex];
	return pAttrib;
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
const CoreObjectAttribute* CoreObjectAttributeList::GetAttributeByName(const char* name) const
{
	const u32 nameHash = Hash(name);
	
	return (const CoreObjectAttribute*)GetAttributeByNameHash(nameHash);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CoreObjectAttribute* CoreObjectAttributeList::GetAttributeByNameHash(u32 nameHash) const
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


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void CoreObjectAttributeList::SetValueForAttribByCString(const char* attribName, const char* valueString)
{
	const u32 nameHash = Hash(attribName);
	
	CoreObjectAttribute* pAttrib = GetAttributeByNameHash(nameHash);
	if(pAttrib != NULL)
	{
		pAttrib->SetValueFromCString(valueString);
	}
}
