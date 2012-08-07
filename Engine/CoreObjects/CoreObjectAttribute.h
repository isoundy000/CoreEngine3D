//
//  CoreObjectAttribute.h
//  CoreEngine3D
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
	void Init(const char* attribName, CoreObjectAttributeType type);
	
	u32 GetSizeBytes() const;
	
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
	CoreObjectAttribute_Char32(const char* name);
	CoreObjectAttribute_Char32(const char* name, const char* defaultValue);
	void SetValueFromCString(const char* cStr);
	
	u8 value[256];
	u32 hashedValue;
};

//F32
class CoreObjectAttribute_F32 : public CoreObjectAttribute
{
public:
	CoreObjectAttribute_F32(const char* name);
	CoreObjectAttribute_F32(const char* name, f32 defaultValue, f32 minValue = 0.0f, f32 maxValue = 1.0f);
	void SetValueFromCString(const char* cStr);
	
	f32 value;
	f32 minValue;
	f32 maxValue;
};

//S32
class CoreObjectAttribute_S32 : public CoreObjectAttribute
{
public:
	CoreObjectAttribute_S32(const char* name);
	CoreObjectAttribute_S32(const char* name, s32 defaultValue, s32 minValue = 0, s32 maxValue = 1024);
	void SetValueFromCString(const char* cStr);
	
	s32 value;
	s32 minValue;
	s32 maxValue;
};

//U32
class CoreObjectAttribute_U32 : public CoreObjectAttribute
{
public:
	CoreObjectAttribute_U32(const char* name);
	CoreObjectAttribute_U32(const char* name, u32 defaultValue, u32 minValue = 0, u32 maxValue = 1024);
	void SetValueFromCString(const char* cStr);
	
	u32 value;
	u32 minValue;
	u32 maxValue;
};

//CoreUI_Origin
class CoreObjectAttribute_CoreUI_Origin : public CoreObjectAttribute
{
public:
	CoreObjectAttribute_CoreUI_Origin(const char* name);
	
	CoreObjectAttribute_CoreUI_Origin(const char* name, CoreUI_Origin defaultValue);
	
	void SetValueFromCString(const char* cStr);
	
	CoreUI_Origin value;
};


class CoreObjectAttributeList
{
public:
	void Init(u8* pDataBuffer, u32 numBytes);
	void Uninit();
	
	const CoreObjectAttribute* operator[](s32 index) const;
	CoreObjectAttribute* operator[](s32 index);
	
	//Returns the byte index for quick access to the attribute
	s32 Add(const CoreObjectAttribute& attrib);
	
	const CoreObjectAttribute* GetAttributeByByteIndex(u32 byteIndex) const;
	CoreObjectAttribute* GetAttributeByByteIndex(u32 byteIndex);
	const CoreObjectAttribute* GetAttributeByName(const char* name) const;
	CoreObjectAttribute* GetAttributeByNameHash(u32 nameHash) const;
	
	void SetValueForAttribByCString(const char* attribName, const char* valueString);
	
	u32 numAttributes;
	
private:
	u8* m_data;
	u32 m_maxDataSizeBytes;
	u32 m_currDataSizeBytes;
	u32 m_byteIndices[CoreObjectAttribute_MaxAttributes];
};


#endif
