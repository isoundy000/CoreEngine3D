//
//  SpriteSheetAnimator.h
//  CoreEngine3D
//
//  Created by Jody McAdams on 12/17/11.
//  Copyright (c) 2011 Jody McAdams. All rights reserved.
//

#ifndef CoreEngine3D__SpriteSheetAnimator_h
#define CoreEngine3D__SpriteSheetAnimator_h

#include "MathTypes.h"
#include "GraphicsTypes.h"

#define SPRITESHEET_MAX_SHEETS 3

struct SpriteFrameInfo
{
	vec2 textureOffset;
	u32* pTextureHandle;
	u32 cellsPerRow;
	ModelData* pModelData;
};

struct SpriteSheet
{
	u32 cellsPerRow;
	u32* pTextureHandle;
	u32 totalCells;
	f32 textureSize;
};

class SpriteSheetAnimator
{
public:
	SpriteSheetAnimator();
	void SetSpriteSheet(u32 spriteSheetIndex, u32* pTextureHandle, f32 textureSize, u32 cellsPerRow);
	bool GetSpriteInfoForFrame(SpriteFrameInfo* pOut_SpriteFrameInfo, u32 frame);
private:
	SpriteSheet m_spriteSheets[SPRITESHEET_MAX_SHEETS];
};

#endif
