//
//  Game.cpp
//  InfiniSurv(OSX)
//
//  Created by Jody McAdams on 11/27/11.
//  Copyright (c) 2011 Jody McAdams. All rights reserved.
//

#include "Game.h"
#include "MathUtil.h"
#include "matrix.h"

#include "ThirdParty/zlib/zlib.h"
#include "base64.h"
#include "Hash.h"
#include "Box2DDebugDraw.h"
#include "Box2DUtil.h"
#include "GameUtil.h"

#include "CoreObjects/CoreObjectFactories.h"
#include "CoreObjects/CoreObjectTypes.h"

#if defined (PLATFORM_WIN)
#include <direct.h>	//For directory commands
#include <stdlib.h>
#endif

#include <algorithm>

#if defined (PLATFORM_IOS) || defined (PLATFORM_ANDROID)
#include "CoreInput_DeviceInputState.h"
#endif

#define ENGINE_PATH "Engine/" 

Game* GAME = NULL;

static bool SortLinePointListByX(const LinePointList& lhs, const LinePointList& rhs);

bool SortLinePointListByX(const LinePointList& lhs, const LinePointList& rhs)
{
	return lhs.pPoints[0].x < rhs.pPoints[0].x;
};

static void DrawFunc_DrawTile_Init();
static void DrawFunc_DrawTile_Uninit();
static void DrawFunc_DrawTile(void* pData);

static void DrawFunc_DrawTileLayer_Init();
static void DrawFunc_DrawTileLayer_Uninit();
static void DrawFunc_DrawTileLayer(void* pData);

static const u32 g_Tile_NumAttributes = 2;
static const AttributeData g_Tile_AttribData[g_Tile_NumAttributes] = 
{
	{ATTRIB_VERTEX,GL_FLOAT,0,3},
	{ATTRIB_TEXCOORD,GL_FLOAT,12,2},
};


static DrawFunctionStruct g_drawStruct_RenderTileLayer = 
{
    DrawFunc_DrawTileLayer_Init,
    DrawFunc_DrawTileLayer,
    DrawFunc_DrawTileLayer_Uninit,
};


void Game::ResetCamera()
{
	m_cameraMode = CameraMode_Anchor;
	
	CopyVec3(&m_startCamPos,&vec3_zero);
	CopyVec3(&m_camPos,&vec3_zero);
	CopyVec3(&m_desiredCamPos,&vec3_zero);
	CopyVec3(&m_followCamPos,&vec3_zero);
	
	m_camLerpTimer = -1.0f;
	m_camLerpTotalTime = 0.0f;
}


bool Game::Init()
{	
	m_paused = false;
	
	m_pUIEditor = NULL;
	m_pTiledLevelEditor = NULL;
	m_pLevelEditor3D = NULL;
	
	m_GUIEditModeOn = false;
	m_TiledLevelEditorOn = false;
	m_LevelEditor3DOn = false;
	
	m_Box2D_NumVelocitySteps = 5;
	m_Box2D_NumPositionSteps = 5;
	
	m_globalSongVolume = 1.0f;
	m_songVolume = 0.0f;
	
	m_numHUDTextures = 0;
	
	m_cullingRange = 30;
	
	m_Box2D_PhysicsIsLocked = false;
	
	m_numTilesToDelete = 0;
	
	m_parallaxScale = 0.0f;

	m_levelHasCamRestraints = false;
	
	m_cameraMode = CameraMode_Anchor;
	CopyVec3(&m_camPos,&vec3_zero);
	CopyVec3(&m_parallaxBasePos,&vec3_zero);

	m_camLerpTimer = -1.0f;

	CopyVec3(&m_followCamPos,&vec3_zero);
	
	m_Box2D_pWorld = NULL;
	m_Box2D_pContactListener = NULL;
	m_Box2D_pDebugDraw = NULL;
	m_Box2D_defaultCollisionFriction = 1.0f;
	
	for(int i=0; i<NumLevelLayers; ++i)
	{
		m_layers[i].tiles = NULL;
		CopyVec3(&m_layers[i].position,&vec3_zero);
		m_layers[i].pLevelData = NULL;
	}
    
    for(u32 i=0; i<GAME_MAX_TILESET_DESCRIPTIONS; ++i)
	{
		TileSetDescription* pDesc = &m_tileSetDescriptions[i];
		pDesc->loadedTextureID = 0;
		pDesc->textureFileName = NULL;
		pDesc->name = NULL;
	}
	
	m_pTileVerts = NULL;
	
	//Register the common models people will use
	GLRENDERER->BufferModel(&g_Square1x1_modelData);
	GLRENDERER->BufferModel(&g_Square_Tiled_2_modelData);
	GLRENDERER->BufferModel(&g_Square_Tiled_4_modelData);
	GLRENDERER->BufferModel(&g_Square_Tiled_8_modelData);
	GLRENDERER->BufferModel(&g_Square_Tiled_16_modelData);
	GLRENDERER->BufferModel(&g_Square_Tiled_32_modelData);

#if defined (PLATFORM_WIN)
	char currentPath[_MAX_PATH];
	GetModuleFileName(0,currentPath,_MAX_PATH);
	std::string pathString(currentPath);

	s32 lastSlashIndex = 0;
	for(u32 i=0; i<pathString.size(); ++i)
	{
		if(pathString[i] == '\\')
		{
			lastSlashIndex = i;
		}
	}

	m_path = pathString.substr(0,lastSlashIndex+1);
	//m_path = "../Crow-Regime/";
	m_enginePath = "../CoreEngine3D/";

	OutputDebugString(m_path.c_str());
#endif

	m_currSongSource = 0;
	m_currSongBuffer = 0;

	m_numLoadedArtDescriptions = 0;
	m_numArtDescriptionsToLoadTexturesFor = 0;
	m_numLoadedSoundDescriptions = 0;
	m_numSoundDescriptionsToLoadWavsFor = 0;

	m_numSongsInPlaylist = 0;
	m_currSongID = -1;
	
#if defined (PLATFORM_IOS)
	m_pTouchInput = [[TouchInputIOS alloc]init:&m_deviceInputState];
	//HACK:
	[m_pTouchInput SetAccelerometerIsUsed:YES];
#endif
	
	m_pCoreAudioOpenAL = new CoreAudioOpenAL;
	m_pCoreAudioOpenAL->Init();
	
#if defined (PLATFORM_OSX) || defined (PLATFORM_IOS)
	m_pAudioPlayer = nil;
#endif

#if defined (PLATFORM_IOS)
	AVAudioSession *session = [AVAudioSession sharedInstance];
	
	NSError* error;
	[session setCategory:AVAudioSessionCategorySoloAmbient error:&error];
#endif
	
	m_coreObjectManager = new CoreObjectManager;
	
	GAME = this;

	return true;
}


void Game::CleanUp()
{
	if(m_Box2D_pWorld != NULL)
	{
		delete m_Box2D_pWorld;
	}
	
	if(m_Box2D_pContactListener != NULL)
	{
		delete m_Box2D_pContactListener;
	}
	
	for(int i=0; i<NumLevelLayers; ++i)
	{
		if(m_layers[i].tiles != NULL)
		{
			delete[] m_layers[i].tiles;
		}
	}
    
    for(u32 i=0; i<m_LinePointLists.size(); ++i)
    {
		if(m_LinePointLists[i].pPoints != NULL)
		{
			delete[] m_LinePointLists[i].pPoints;
		}
    }
	
	for(u32 i=0; i<GAME_MAX_TILESET_DESCRIPTIONS; ++i)
	{
		TileSetDescription* pDesc = &m_tileSetDescriptions[i];
		
		if(pDesc->textureFileName != NULL)
		{
			delete[] pDesc->textureFileName;
		}
		
		if(pDesc->name != NULL)
		{
			delete[] pDesc->name;
		}
		
		GLRENDERER->DeleteTexture(&pDesc->loadedTextureID);
	}
	
	for(u32 i=0; i<m_numHUDTextures; ++i)
	{
		GLRENDERER->DeleteTexture(&m_HUDTextures[i].textureHandle);
	}
	
	delete m_pCoreAudioOpenAL;
	delete m_coreObjectManager;
}


void Game::ToggleUIEditor()
{
	m_GUIEditModeOn = !m_GUIEditModeOn;
	if(m_GUIEditModeOn == true)
	{
		COREDEBUG_PrintDebugMessage("GUI Edit Mode: On");
		m_pUIEditor->SetVisible(true);
	}
	else
	{
		COREDEBUG_PrintDebugMessage("GUI Edit Mode: Off");
		m_pUIEditor->SetVisible(false);
	}
}

void Game::ToggleTiledGameEditor()
{
	
}


void Game::Update(f32 timeElapsed)
{
#if defined (PLATFORM_OSX) || defined(PLATFORM_WIN)
	
	//35 is the 'P' key
	if(m_keyboardState.buttonState[112] == CoreInput_ButtonState_Began)
	{
		m_paused = !m_paused;
		
		//TODO: change how this works if you want to do
		//fancy UI effects that won't draw any more because
		//you paused the graphics
		GLRENDERER->paused = m_paused;
		
		HandlePauseStatus();
	}
	
#if defined(_DEBUG_PC)
	//Make sure if you're editingthe UI that debug lines
	//will always draw
	if(m_GUIEditModeOn == true)
	{
		GLRENDERER->paused = false;
	}
#endif
	
	//Update controls
	
	for(u32 i=0; i<MOUSESTATE_MAX_MOUSEBUTTONS; ++i)
	{
		const CoreInput_ButtonState mouseState = m_mouseState.buttonState[i];

		if(mouseState == CoreInput_ButtonState_Began)
		{
			m_mouseState.buttonState[i] = CoreInput_ButtonState_Held;
		}
		else if(mouseState == CoreInput_ButtonState_Ended)
		{
			m_mouseState.buttonState[i] = CoreInput_ButtonState_None;
		}
	}
	
	//Save last position for sleep detection
	CopyIntVec2(&m_mouseState.lastPosition,&m_mouseState.position);
	
	for(u32 i=0; i<256; ++i)
	{
		const CoreInput_ButtonState keyState = m_keyboardState.buttonState[i];
		
		if(keyState == CoreInput_ButtonState_Began)
		{
			if(m_keyboardState.skippedBegan[i])
			{
				m_keyboardState.skippedBegan[i] = 0;
				m_keyboardState.buttonState[i] = CoreInput_ButtonState_Ended;
			}
			else
			{
				m_keyboardState.buttonState[i] = CoreInput_ButtonState_Held;
			}
		}
		else if(keyState == CoreInput_ButtonState_Ended)
		{
			m_keyboardState.buttonState[i] = CoreInput_ButtonState_None;
		}
	}
#endif
	
	//If the game is paused, we can bust out at this point
	if(m_paused)
	{
		return;
	}

	if(m_camLerpTimer > 0.0f)
	{
		//If this is follow cam, slowly lerp to the follow cam pos
		if(m_cameraMode == CameraMode_FollowCam)
		{
			CopyVec3(&m_desiredCamPos,&m_followCamPos);
		}
		
		m_camLerpTimer -= timeElapsed;
		if(m_camLerpTimer < 0.0f)
		{
			m_camLerpTimer = 0.0f;
		}
		
		LerpVec3(&m_camPos,&m_desiredCamPos,&m_startCamPos,m_camLerpTimer/m_camLerpTotalTime);
	}
	//If we're not lerping and it's follow cam, straight up copy the position
	else if(m_cameraMode == CameraMode_FollowCam)
	{
		CopyVec3(&m_camPos,&m_followCamPos);
	}
	
	if(m_levelHasCamRestraints == true)
	{
		ConstrainCameraToTiledLevel();
	}
	
	if(m_Box2D_pWorld != NULL)
	{
		if(m_Box2D_PhysicsIsLocked)
		{
			m_Box2D_pWorld->Step(1.0f/60.0f, m_Box2D_NumVelocitySteps, m_Box2D_NumPositionSteps);
		}
		else
		{
			m_Box2D_pWorld->Step(timeElapsed, m_Box2D_NumVelocitySteps, m_Box2D_NumPositionSteps);
		}
		
#ifdef _DEBUG
		m_Box2D_pWorld->DrawDebugData();
#endif
	}
	
	Layer* pMainLayer = &m_layers[LevelLayer_Main1];
	Layer* pCollLayer = &m_layers[LevelLayer_Collision];
	
	for(u32 i=0; i<m_numTilesToDelete; ++i)
	{
		TileDestructionData* pCurrTile = &m_tilesToDelete[i];

		const s32 indexX = pCurrTile->tilePos.x;
		const s32 indexY = pCurrTile->tilePos.y;
		
		Tile* pMainTile = &ARRAY2D(pMainLayer->tiles, indexX, indexY, pMainLayer->numTilesX);
		ToggleTileVisibility(pMainTile,false);
		
		//Only destroy visible tiles
		//This will also help with possible repeated tiles showing
		//up in this list

		Tile* pCollTile = &ARRAY2D(pCollLayer->tiles, indexX, indexY, pCollLayer->numTilesX);
		if(pCollTile->pBody != NULL)
		{
			m_Box2D_pWorld->DestroyBody(pCollTile->pBody);
			pCollTile->pBody = NULL;
		}
		
		//Do special stuff according to your specific game
		TileDestructionCallback(indexX, indexY, &pCurrTile->hitVel);
	}
	
	m_numTilesToDelete = 0;

	
}


s32 Game::AddSongToPlaylist(const char* songFilenameMP3)
{
	const u32 songID = m_numSongsInPlaylist;
	
	m_songPlaylist[songID] = new char[strlen(songFilenameMP3)+1];
	strcpy(m_songPlaylist[songID], songFilenameMP3);
	
	//printf("songname: %s\n",m_songPlaylist[songID]);
	
	++m_numSongsInPlaylist;
	
	return songID;
}


std::string Game::GetPathToFile(const char* filename, bool fromEngine)
{
#if defined (PLATFORM_OSX) || defined (PLATFORM_IOS)
@autoreleasepool{
	NSString* fileString = [NSString stringWithCString:filename encoding:NSUTF8StringEncoding];
	NSString *fullPath = [[NSBundle mainBundle] pathForResource:[fileString lastPathComponent] ofType:nil inDirectory:[fileString stringByDeletingLastPathComponent]];
	
	//File does not exist or if you're running XCode and just added new art,
	//do a Product->Clean from the menu
	assert(fullPath != nil);
	
	std::string pathString;
	if(fullPath)
	{
		pathString = [fullPath UTF8String];
	}
	
	return pathString;
}
#endif

#if defined (PLATFORM_WIN)
	return (fromEngine) ? m_enginePath + ENGINE_PATH + std::string(filename) : m_path + std::string(filename);
#endif
}


void Game::StopSong()
{
	[m_pAudioPlayer stop];
	
	m_pAudioPlayer = nil;
	
	m_currSongID = -1;
}

void Game::SetGlobalSongVolume(f32 volume)
{
	m_globalSongVolume = volume;
	
#if defined (PLATFORM_OSX) || defined (PLATFORM_IOS)
	[m_pAudioPlayer setVolume:m_songVolume*m_globalSongVolume];
#endif
}

void Game::PlaySongByID(s32 songID, f32 volume, bool isLooping)
{
	if(songID >= (s32)m_numSongsInPlaylist)
	{
		return;
	}
	
	if(m_currSongID == songID)
	{
		return;
	}
	
#if defined (PLATFORM_OSX) || defined (PLATFORM_IOS)
	[m_pAudioPlayer stop];
	
	NSString* fileString = [NSString stringWithCString:m_songPlaylist[songID] encoding:NSUTF8StringEncoding];
	NSString *fullPath = [[NSBundle mainBundle] pathForResource:[fileString lastPathComponent] ofType:nil inDirectory:[fileString stringByDeletingLastPathComponent]];
	assert(fullPath != nil);
	
	NSURL *soundURL = [NSURL fileURLWithPath:fullPath];
	
	//m_pAudioPlayer = CreateAudioPlayer(fullPath,@"",YES,1.0f);
	NSError* error;
	
	m_songVolume = 1.0f;
	
	m_pAudioPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:soundURL error:&error];
	m_pAudioPlayer.volume = m_songVolume*m_globalSongVolume;
	m_pAudioPlayer.numberOfLoops = isLooping?-1:0;
	
	[m_pAudioPlayer play];
#endif

#if defined (PLATFORM_WIN)
	/*if(m_currSongSource != 0)
	{
		OPENALAUDIO->DeleteSoundSource(&m_currSongSource);
	}

	if(m_currSongBuffer != 0)
	{
		OPENALAUDIO->DeleteSoundBuffer(&m_currSongBuffer);
	}

	std::string filePath = GetPathToFile(m_songPlaylist[songID]);
	OPENALAUDIO->CreateSoundBufferFromFile(filePath.c_str(),&m_currSongBuffer);
	m_currSongSource = OPENALAUDIO->CreateSoundSourceFromBuffer(m_currSongBuffer);
	OPENALAUDIO->PlaySoundSource(m_currSongSource,1.0f,1.0f,true);*/
#endif
	
	m_currSongID = songID;
}




/*void Game::UpdateButtons(TouchState touchState, vec2 *pTouchPosBegin, vec2* pTouchPosCurr)
{
	for(u32 i=0; i<m_ui_numButtons; ++i)
	{
		if(touchState != TouchState_None)
		{
			m_ui_buttons[i].PressButton(touchState, pTouchPosBegin, pTouchPosCurr);
		}
	}
}*/


//Checks if the art will be loaded next time LoadItemArt gets called
bool Game::WillArtDescriptionBeLoaded(TextureAsset* pArtDesc)
{
    for(u32 i=0; i<m_numArtDescriptionsToLoadTexturesFor; ++i)
    {
        TextureAsset* pCurrArtDesc = m_pArtDescriptionsToLoadTexturesFor[i];
		if(pCurrArtDesc == NULL)
		{
			return false;
		}
        else if(pArtDesc == pCurrArtDesc)
        {
            return true;
        }
    }
    
    return false;
}


//Call many times to prepare art to be loaded later
void Game::AddTextureResource(TextureAsset* pArtDescription)
{
    //Make sure this description is not already in the list
    for(u32 i=0; i<m_numArtDescriptionsToLoadTexturesFor; ++i)
    { 
        if(m_pArtDescriptionsToLoadTexturesFor[i] == pArtDescription)
        {
            return;
        }
    }
    
    //It wasn't in the list, so add it
    m_pArtDescriptionsToLoadTexturesFor[m_numArtDescriptionsToLoadTexturesFor] = pArtDescription;
    ++m_numArtDescriptionsToLoadTexturesFor;
}


//Call after all your art has been added to the list of things to load gunna die
//You can call this multiple times if you want and nothing bad will happen
void Game::LoadItemArt()
{
    //Dump old textures that don't need to be loaded
    for(u32 i=0; i<m_numLoadedArtDescriptions; ++i)
    {
        TextureAsset* pCurrArtDesc = m_pLoadedArtDescriptions[i];
		if(pCurrArtDesc == NULL)
		{
			//This seems evil
			continue;
		}
		
        if(m_numArtDescriptionsToLoadTexturesFor == 0
		   || WillArtDescriptionBeLoaded(pCurrArtDesc) == false)
        {
            GLRENDERER->DeleteTexture(&pCurrArtDesc->textureHandle);
        }
    }
	
    //Reset the loaded description count to 0
    m_numLoadedArtDescriptions = 0;
	
    //Load unloaded level item textures
    //This list has only unique entries
    for(u32 i=0; i<m_numArtDescriptionsToLoadTexturesFor; ++i)
    {
		TextureAsset* pCurrArtDesc = m_pArtDescriptionsToLoadTexturesFor[i];
		if(pCurrArtDesc == NULL)
		{
			//Why is someone adding NULL art descriptions?
			continue;
		}
		
		//This will load the texture but if it's already loaded, it will do nothing
		const bool loadedATexture = GLRENDERER->LoadTexture(pCurrArtDesc->textureFileName, pCurrArtDesc->imageType, &pCurrArtDesc->textureHandle, pCurrArtDesc->textureFilterMode, pCurrArtDesc->wrapModeU, pCurrArtDesc->wrapModeV,pCurrArtDesc->flipY);
		
		//If we loaded a texture, we should force a resort for optimum performance
		if(loadedATexture == true)
		{
			GLRENDERER->ForceRenderablesNeedSorting(RenderableObjectType_All);
		}
        
        //Store the descriptions we've loaded
		//Some of them were already loaded but now we can keep track!
        m_pLoadedArtDescriptions[m_numLoadedArtDescriptions] = pCurrArtDesc;
        ++m_numLoadedArtDescriptions;
    }
}


//Call this at init, then call AddArtDescriptionToLoad many times.
//The art you're not using anymore will get deleted when you call LoadItemArt
void Game::ClearItemArt()
{
	m_numArtDescriptionsToLoadTexturesFor = 0;
}


//Just deletes all the loaded art PERIOD
//You might never have to call this
void Game::DeleteAllItemArt()
{
	//Dump old textures that don't need to be loaded
    for(u32 i=0; i<m_numLoadedArtDescriptions; ++i)
    {
        TextureAsset* pCurrArtDesc = m_pLoadedArtDescriptions[i];
		GLRENDERER->DeleteTexture(&pCurrArtDesc->textureHandle);
    }
	
	m_numLoadedArtDescriptions = 0;
	m_numArtDescriptionsToLoadTexturesFor = 0;
}


//SOUND

//Checks if the art will be loaded next time LoadItemArt gets called
bool Game::WillSoundDescriptionBeLoaded(SoundAsset* pSoundDesc)
{
    for(u32 i=0; i<m_numSoundDescriptionsToLoadWavsFor; ++i)
    {
        SoundAsset* pCurrSoundDesc = m_pSoundDescriptionsToLoadWavsFor[i];
        if(pSoundDesc == pCurrSoundDesc)
        {
            return true;
        }
    }
    
    return false;
}


//Call many times to prepare sound to be loaded later
void Game::AddSoundResource(SoundAsset* pSoundDescription)
{
    //Make sure this description is not already in the list
    for(u32 i=0; i<m_numSoundDescriptionsToLoadWavsFor; ++i)
    { 
        if(m_pSoundDescriptionsToLoadWavsFor[i] == pSoundDescription)
        {
            return;
        }
    }
    
    //It wasn't in the list, so add it
    m_pSoundDescriptionsToLoadWavsFor[m_numSoundDescriptionsToLoadWavsFor] = pSoundDescription;
    ++m_numSoundDescriptionsToLoadWavsFor;
}


//Call after all your sound has been added to the list of things to load gunna die
//You can call this multiple times if you want and nothing bad will happen
void Game::LoadItemSounds()
{
    //Dump old sounds that don't need to be loaded
    for(u32 i=0; i<m_numLoadedSoundDescriptions; ++i)
    {
        SoundAsset* pCurrSoundDesc = m_pLoadedSoundDescriptions[i];
        if(m_numSoundDescriptionsToLoadWavsFor == 0
		   || WillSoundDescriptionBeLoaded(pCurrSoundDesc) == false)
        {
			OPENALAUDIO->DeleteSoundBuffer(&pCurrSoundDesc->soundBufferID);
        }
    }
	
    //Reset the loaded description count to 0
    m_numLoadedSoundDescriptions = 0;
	
    //Load unloaded level item textures
    //This list has only unique entries
    for(u32 i=0; i<m_numSoundDescriptionsToLoadWavsFor; ++i)
    {
		SoundAsset* pCurrSoundDesc = m_pSoundDescriptionsToLoadWavsFor[i];
	
		OPENALAUDIO->CreateSoundBufferFromFile(pCurrSoundDesc->soundFileName, &pCurrSoundDesc->soundBufferID);
		
        //Store the descriptions we've loaded
		//Some of them were already loaded but now we can keep track!
        m_pLoadedSoundDescriptions[m_numLoadedSoundDescriptions] = pCurrSoundDesc;
        ++m_numLoadedSoundDescriptions;
    }
}


//Call this at init, then call AddArtDescriptionToLoad many times.
//The art you're not using anymore will get deleted when you call LoadItemArt
void Game::ClearItemSounds()
{
	m_numSoundDescriptionsToLoadWavsFor = 0;
}


//Just deletes all the loaded art PERIOD
//You might never have to call this
void Game::DeleteAllItemSounds()
{
	//Dump old sounds that don't need to be loaded
    for(u32 i=0; i<m_numLoadedSoundDescriptions; ++i)
    {
        SoundAsset* pCurrSoundDesc = m_pLoadedSoundDescriptions[i];
		OPENALAUDIO->DeleteSoundBuffer(&pCurrSoundDesc->soundBufferID);
    }
	
	m_numLoadedSoundDescriptions = 0;
	m_numSoundDescriptionsToLoadWavsFor = 0;
}

static void DrawFunc_DrawTile_Init()
{
    GLRENDERER->DisableVertexBufferObjects();
    
    glEnableVertexAttribArray(ATTRIB_VERTEX);
	glEnableVertexAttribArray(ATTRIB_TEXCOORD);
}

static void DrawFunc_DrawTile_Uninit()
{
    glDisableVertexAttribArray(ATTRIB_VERTEX);
	glDisableVertexAttribArray(ATTRIB_TEXCOORD);
}

static void DrawFunc_DrawTile(void* pData)
{
	Tile* pTile = (Tile*)pData;
	
	const TileSetDescription* pDesc = pTile->pDesc;
	
	static f32 tileVerts[20] = {
		/*v:*/-0.5f, -0.5f, 0.0f, /*t:*/0.0f,0.0f, 
		/*v:*/-0.5f,  0.5f, 0.0f, /*t:*/0.0f,0.5f,
		/*v:*/ 0.5f, -0.5f, 0.0f, /*t:*/0.5f,0.0f,  
		/*v:*/ 0.5f,  0.5f, 0.0f, /*t:*/0.5f,0.5f,
	};
	
	const PrimitiveData* pVertData = &pDesc->pModelData->primitiveArray[0];
	memcpy(tileVerts, pVertData->vertexData, pVertData->sizeOfVertexData);
	
	vec3* pos0 = (vec3*)&tileVerts[0];
	vec3* pos1 = (vec3*)&tileVerts[5];
	vec3* pos2 = (vec3*)&tileVerts[10];
	vec3* pos3 = (vec3*)&tileVerts[15];
	
	const f32 scaleX = pTile->scale.x;
	const f32 scaleY = pTile->scale.y;
	
	pos0->x *= scaleX;
	pos1->x *= scaleX;
	pos2->x *= scaleX;
	pos3->x *= scaleX;
	
	pos0->y *= scaleY;
	pos1->y *= scaleY;
	pos2->y *= scaleY;
	pos3->y *= scaleY;
	
	AddVec2_Self((vec2*)pos0, &pTile->position);
	AddVec2_Self((vec2*)pos1, &pTile->position);
	AddVec2_Self((vec2*)pos2, &pTile->position);
	AddVec2_Self((vec2*)pos3, &pTile->position);
	
	const f32 screenWidthDiv2 = (f32)GLRENDERER->screenWidth_points/2.0f;
	const f32 screenHeightDiv2 = (f32)GLRENDERER->screenHeight_points/2.0f;
	
	pos0->x = pos0->x/screenWidthDiv2-1.0f;
	pos1->x = pos1->x/screenWidthDiv2-1.0f;
	pos2->x = pos2->x/screenWidthDiv2-1.0f;
	pos3->x = pos3->x/screenWidthDiv2-1.0f;
	
	pos0->y = -pos0->y/screenHeightDiv2+1.0f;
	pos1->y = -pos1->y/screenHeightDiv2+1.0f;
	pos2->y = -pos2->y/screenHeightDiv2+1.0f;
	pos3->y = -pos3->y/screenHeightDiv2+1.0f;
	
	vec2* uv0 = (vec2*)&tileVerts[3];
	vec2* uv1 = (vec2*)&tileVerts[8];
	vec2* uv2 = (vec2*)&tileVerts[13];
	vec2* uv3 = (vec2*)&tileVerts[18];
	
	AddVec2_Self(uv0, &pTile->texCoordOffset);
	AddVec2_Self(uv1, &pTile->texCoordOffset);
	AddVec2_Self(uv2, &pTile->texCoordOffset);
	AddVec2_Self(uv3, &pTile->texCoordOffset);
	
	glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, 0, 20, pos0);
    
	glVertexAttribPointer(ATTRIB_TEXCOORD, 2, GL_FLOAT, 0, 20, uv0);
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

static void DrawFunc_DrawTileLayer_Init()
{
	//TODO: check this
    GLRENDERER->DisableVertexBufferObjects();
}

static void DrawFunc_DrawTileLayer_Uninit()
{
	const bool supportsVAOs = GLRENDERER->GetSupportsVAOs();
	
	if(supportsVAOs == false)
	{
		glDisableVertexAttribArray(ATTRIB_VERTEX);
		glDisableVertexAttribArray(ATTRIB_TEXCOORD);
	}
}

static void DrawFunc_DrawTileLayer(void* pData)
{
	mat4f identityMat;
	mat4f_LoadIdentity(identityMat);
	
	const bool supportsVAOs = GLRENDERER->GetSupportsVAOs();
	
	Layer* pLayer = (Layer*)pData;

	TileVert* pTileVerts = GAME->GetTiledVerts();
	
	u32 tileVertIndex = 0;
	
	const s32 numTilesX = pLayer->numTilesX;
	const s32 numTilesY = pLayer->numTilesY;
	
	
	const s32 tilePosX = (s32)(-pLayer->position.x/GAME->GetTileSize());
	
	const s32 numScreenTilesX = (f32)GLRENDERER->screenWidth_points/GAME->GetTileSize()+0.5f;
	
	int xStart = ClampS32(tilePosX, 0, numTilesX);
	int xEnd = ClampS32(tilePosX+numScreenTilesX+1, 0, numTilesX);
	
	assert(xEnd < numTilesX+1);
	
	const s32 tilePosY = (s32)(-pLayer->position.y/GAME->GetTileSize());
	
	const s32 numScreenTilesY = (f32)GLRENDERER->screenHeight_points/GAME->GetTileSize()+0.5f;
	
	const s32 yStart = ClampS32(tilePosY, 0, numTilesY);
	const s32 yEnd = ClampS32(tilePosY+numScreenTilesY+1, 0, numTilesY);
	
	u32 textureID = 0;
	
	//Calculate position
	const s32 halfTileSize = GAME->GetHalfTileSize();
	const s32 tileSize = GAME->GetTileSize();
	const s32 baseX = halfTileSize+((s32)pLayer->position.x);
	const s32 baseY = halfTileSize+((s32)pLayer->position.y);
	
	for(u32 y=yStart; y<yEnd; ++y)
	{
		const s32 tileBasePosY = y*tileSize+baseY;
		
		for(u32 x=xStart; x<xEnd; ++x)
		{
			Tile* pTile = &ARRAY2D(pLayer->tiles, x, y, numTilesX);
			
			if(pTile->tileID == -1)
			{
				continue;
			}
			
			const s32 tileBasePosX = x*tileSize+baseX;
			vec2 position = {(f32)tileBasePosX,(f32)tileBasePosY};
			
			//Create vert!
			const TileSetDescription* pDesc = pTile->pDesc;
			
			//TODO: replace this fucked code
			textureID = pDesc->loadedTextureID;
			
			static TileVert tileVerts[4];
			
			const PrimitiveData* pVertData = &pDesc->pModelData->primitiveArray[0];
			memcpy(tileVerts, pVertData->vertexData, pVertData->sizeOfVertexData);
			
			vec3* pos0 = &tileVerts[0].position;
			vec3* pos1 = &tileVerts[1].position;
			vec3* pos2 = &tileVerts[2].position;
			vec3* pos3 = &tileVerts[3].position;
			
			ScaleVec3_Self(pos0, tileSize);
			ScaleVec3_Self(pos1, tileSize);
			ScaleVec3_Self(pos2, tileSize);
			ScaleVec3_Self(pos3, tileSize);
			
			AddVec2_Self((vec2*)pos0, &position);
			AddVec2_Self((vec2*)pos1, &position);
			AddVec2_Self((vec2*)pos2, &position);
			AddVec2_Self((vec2*)pos3, &position);
			
			const f32 screenWidthDiv2 = (f32)GLRENDERER->screenWidth_points/2.0f;
			const f32 screenHeightDiv2 = (f32)GLRENDERER->screenHeight_points/2.0f;
			
			pos0->x = pos0->x/screenWidthDiv2-1.0f;
			pos1->x = pos1->x/screenWidthDiv2-1.0f;
			pos2->x = pos2->x/screenWidthDiv2-1.0f;
			pos3->x = pos3->x/screenWidthDiv2-1.0f;
			
			pos0->y = -pos0->y/screenHeightDiv2+1.0f;
			pos1->y = -pos1->y/screenHeightDiv2+1.0f;
			pos2->y = -pos2->y/screenHeightDiv2+1.0f;
			pos3->y = -pos3->y/screenHeightDiv2+1.0f;
			
			vec2* uv0 = &tileVerts[0].texcoord;
			vec2* uv1 = &tileVerts[1].texcoord;
			vec2* uv2 = &tileVerts[2].texcoord;
			vec2* uv3 = &tileVerts[3].texcoord;
			
			const s32 tileID_X = pTile->tileID%pDesc->numTextureTilesX;
			const s32 tileID_Y = pTile->tileID/pDesc->numTextureTilesX;
			
			vec2 texcoordOffset;
			texcoordOffset.x = (f32)tileID_X/(f32)pDesc->numTextureTilesX;
			texcoordOffset.y = (f32)tileID_Y/(f32)pDesc->numTextureTilesY;
			
			AddVec2_Self(uv0, &texcoordOffset);
			AddVec2_Self(uv1, &texcoordOffset);
			AddVec2_Self(uv2, &texcoordOffset);
			AddVec2_Self(uv3, &texcoordOffset);
			
			//Triangle 1
			pTileVerts[tileVertIndex]   = tileVerts[0];
			pTileVerts[tileVertIndex+1] = tileVerts[1];
			pTileVerts[tileVertIndex+2] = tileVerts[2];
			
			//Triangle 2
			pTileVerts[tileVertIndex+3] = tileVerts[1];
			pTileVerts[tileVertIndex+4] = tileVerts[2];
			pTileVerts[tileVertIndex+5] = tileVerts[3];
			
			//Increment vert index
			tileVertIndex += 6; //6 verts in a tile
		}
	}
	
	GAME->UpdateTileVBO();
	
	if (supportsVAOs)
	{
		const u32 vao = GAME->GetTileVAO();
		GLRENDERER->BindVertexArrayObject(vao);
	}
	else
	{
		const u32 vbo = GAME->GetTileVBO();
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		
		for(u32 i=0; i<g_Tile_NumAttributes; ++i)
		{
			GLRENDERER->EnableAttribute(&g_Tile_AttribData[i], sizeof(TileVert));
		}
	}
	
	GLRENDERER->SetTexture(&textureID, 0);
	
	glDrawArrays(GL_TRIANGLES, 0, tileVertIndex);
}


void Game::UpdateTileVBO()
{
	GLRENDERER->UpdateVBO(m_tileVBOHandle, m_pTileVerts, m_tileVertDataSize, GL_DYNAMIC_DRAW);
}


TileVert* Game::GetTiledVerts()
{
	return m_pTileVerts;
}


u32 Game::GetTileVBO()
{
	return m_tileVBOHandle;
}


u32 Game::GetTileVAO()
{
	return m_tileVAOHandle;
}


void Game::CreateRenderableTile(Tile* pTile, RenderableGeometry3D** pGeom, RenderLayer renderLayer, RenderMaterial material, vec2* pOut_texCoordOffset, bool usesViewMatrix)
{
	CoreObjectHandle hRenderable = GLRENDERER->CreateRenderableGeometry3D(RenderableObjectType_Normal,pGeom);
	if(hRenderable.IsValid() == false)
	{
		return;
	}
	
	TileSetDescription* pDesc = pTile->pDesc;
	
	vec3 scaleVec = {(f32)m_tiledLevelDescription.tileDisplaySizeX,(f32)m_tiledLevelDescription.tileDisplaySizeX,(f32)m_tiledLevelDescription.tileDisplaySizeX};
	
	if(pTile->flags & TILE_FLIPPED_HORIZONTALLY_FLAG)
	{
		scaleVec.x *= -1.0f;
	}
	
	if(pTile->flags & TILE_FLIPPED_VERTICALLY_FLAG)
	{
		scaleVec.y *= -1.0f;
	}
	
	if(pTile->flags & TILE_FLIPPED_DIAGONALLY_FLAG)
	{
		scaleVec.x *= -1.0f;
		scaleVec.y *= -1.0f;
	}
	
	f32 tileMat[16];
	mat4f_LoadScaleFromVec(tileMat, &scaleVec);

	const u32 baseFlag = usesViewMatrix ? RenderFlagDefaults_2DTexture_AlphaBlended_UseView:RenderFlagDefaults_2DTexture_AlphaBlended;
	
	GLRENDERER->InitRenderableGeometry3D(*pGeom, pDesc->pModelData, material, &pDesc->loadedTextureID, tileMat, renderLayer, BlendMode_Normal, baseFlag|RenderFlag_Visible);
	
	const s32 tileID_X = pTile->tileID%pDesc->numTextureTilesX;
	const s32 tileID_Y = pTile->tileID/pDesc->numTextureTilesX;
	
	if(pOut_texCoordOffset != NULL)
	{
		pOut_texCoordOffset->x = (f32)tileID_X/(f32)pDesc->numTextureTilesX;
		pOut_texCoordOffset->y = (f32)tileID_Y/(f32)pDesc->numTextureTilesY;
	}
	
	pTile->hRenderable = hRenderable;
}


void Game::CullTile(Layer* layer, s32 x, s32 y)
{
	Tile* pTile = &ARRAY2D(layer->tiles, x, y, layer->numTilesX);
	if(pTile->tileID == -1)
	{
		return;
	}
	
	//If there is a valid renderable, delete it
	if(pTile->hRenderable.IsValid() == true)
	{
		RenderableGeometry3D* pCurrRenderable = (RenderableGeometry3D*)COREOBJECTMANAGER->GetObjectByHandle(pTile->hRenderable);
		
		if(pCurrRenderable != NULL)
		{
			pCurrRenderable->DeleteObject();	
		}
		
		pTile->hRenderable = CoreObjectHandle();
	}
}


void Game::SetTileCullingRange(s32 cullingRange)
{
	m_cullingRange = cullingRange;
}


void Game::UpdateTiledLevelPosition(vec3* pPosition)
{
	vec3 position;
	ScaleVec3(&position,pPosition,-1.0f);
	
	for(s32 i=0; i<NumLevelLayers; ++i)
	{
		const LevelLayer currLayer = (LevelLayer)i;
		
#ifndef _DEBUG
		if(currLayer == LevelLayer_Collision)
		{
			continue;
		}
#endif
		if(currLayer == LevelLayer_CameraExtents)
		{
			continue;
		}
		
		Layer* pCurrLayer = &m_layers[i];
		if(pCurrLayer->pLevelData == NULL)
		{
			continue;
		}
		
		//If this is the collision layer, it should move at the same rate as the main layer
		const s32 adjustedIndex = (currLayer==LevelLayer_Main0 || currLayer==LevelLayer_Collision || currLayer==LevelLayer_TileObjectArt)?(s32)LevelLayer_Main1:i;
		
		CopyVec3(&pCurrLayer->position,&position);
		
		if(m_parallaxScale != 0.0f)
		{
			vec3 parallaxDiffVec;
			
			SubVec3(&parallaxDiffVec,&m_parallaxBasePos,&position);
			SubScaledVec3_Self(&pCurrLayer->position,&parallaxDiffVec,(f32)(adjustedIndex-LevelLayer_Main1)*m_parallaxScale);
		}
	}
}


const vec3* Game::GetCameraPosition()
{
	return &m_camPos;
}


void Game::SetParallaxPosition(const vec3* pParallaxPos)
{
	CopyVec3(&m_parallaxBasePos,pParallaxPos);
}

void Game::SetParallaxScale(f32 parallaxScale)
{
	m_parallaxScale = parallaxScale;
}


//use with caution
void Game::SetCameraPosition(const vec3* pCamPos, f32 lerpTime)
{
	if(lerpTime == 0.0f)
	{
		CopyVec3(&m_startCamPos,pCamPos);
		CopyVec3(&m_camPos,pCamPos);
	}
	else
	{
		m_camLerpTimer = lerpTime;
		m_camLerpTotalTime = lerpTime;
		
		CopyVec3(&m_desiredCamPos,pCamPos);
		CopyVec3(&m_startCamPos,&m_camPos);
	}
}


#if defined (PLATFORM_IOS) || defined (PLATFORM_ANDROID)
DeviceInputState* Game::GetDeviceInputState()
{
	return &m_deviceInputState;
}
#endif


void Game::GetTileIndicesFromScreenPosition(const vec2* pPosition, u32* pOut_X, u32* pOut_Y)
{
	*pOut_X = static_cast<u32>((pPosition->x+m_camPos.x)/m_tiledLevelDescription.tileDisplaySizeX);
	*pOut_Y = static_cast<u32>((pPosition->y+m_camPos.y)/m_tiledLevelDescription.tileDisplaySizeY);
}

void Game::GetTileIndicesFromPosition(const vec2* pPosition, u32* pOut_X, u32* pOut_Y)
{
	*pOut_X = static_cast<u32>((pPosition->x)/m_tiledLevelDescription.tileDisplaySizeX);
	*pOut_Y = static_cast<u32>((pPosition->y)/m_tiledLevelDescription.tileDisplaySizeY);
}


void Game::GetPositionFromTileIndices(s32 index_X, s32 index_Y, vec3* pOut_position)
{
	pOut_position->x = static_cast<f32>(index_X * m_tiledLevelDescription.tileDisplaySizeX + m_tiledLevelDescription.halfTileDisplaySizeX);
	pOut_position->y = static_cast<f32>(index_Y * m_tiledLevelDescription.tileDisplaySizeY + m_tiledLevelDescription.halfTileDisplaySizeY);
	pOut_position->z = 0.0f;
}


s32 Game::GetCollisionFromTileIndices(s32 index_X, s32 index_Y)
{
	Layer* pLayer = &m_layers[LevelLayer_Collision];

	if(index_X < 0 || index_X > static_cast<s32>(pLayer->numTilesX)-1
	   || index_Y < 0 || index_Y > static_cast<s32>(pLayer->numTilesY)-1)
	{
		return -1;
	}
	
	Tile* pTile = &ARRAY2D(pLayer->tiles, index_X, index_Y, pLayer->numTilesX);
	
	return pTile->tileID;
}


f32 Game::GetTileUnitConversionScale()
{
	return m_unitConversionScale;
}


f32 Game::GetTileSize()
{
	return static_cast<f32>(m_tiledLevelDescription.tileDisplaySizeX);
}


f32 Game::GetHalfTileSize()
{
	return static_cast<f32>(m_tiledLevelDescription.halfTileDisplaySizeX);
}


f32 Game::GetPixelsPerMeter()
{
	return static_cast<f32>(m_pixelsPerMeter);
}


void Game::ConstrainCameraToTiledLevel()
{
	Layer* pCamExtents = &m_layers[LevelLayer_CameraExtents];
	if(pCamExtents->pLevelData == NULL)
	{
		return;
	}

	const f32 minCameraX = m_camExtentTL_X;
	const f32 maxCameraX = m_camExtentBR_X-GLRENDERER->screenWidth_points;
	const f32 minCameraY = m_camExtentTL_Y;
	const f32 maxCameraY = m_camExtentBR_Y-GLRENDERER->screenHeight_points;
	
	if(m_camPos.x < minCameraX)
	{
		m_camPos.x = minCameraX;
	}
	else if(m_camPos.x > maxCameraX)
	{
		m_camPos.x = maxCameraX;
	}

	if(m_camPos.y < minCameraY)
	{
		m_camPos.y = minCameraY;
	}
	else if(m_camPos.y > maxCameraY)
	{
		m_camPos.y = maxCameraY;
	}
}


void Game::ConvertTileID(s32* p_InOut_tileID, TileSetDescription** pOut_tileDesc)
{
	s32 tileID = *p_InOut_tileID;
	tileID &= 0x00FFFFFF;	//Remove the flags
	//Now tileID is a tile index
	
	//Now find the tileset it belongs to
	u32 maxTilesetIndex = 1;
	TileSetDescription* pFoundDesc = &m_tileSetDescriptions[0];
	
	for(u32 tilesetIDX=0; tilesetIDX<m_numTileSetDescriptions; ++tilesetIDX)
	{
		TileSetDescription* pDesc = &m_tileSetDescriptions[tilesetIDX];
		
		if(static_cast<u32>(tileID) >= pDesc->firstTileIndex)
		{
			if(pDesc->firstTileIndex >= maxTilesetIndex)
			{
				pFoundDesc = pDesc;
				maxTilesetIndex = pDesc->firstTileIndex;
			}
		}
	}
	
	//Save the tileset description in the tile.
	//It makes it easier to load the tile later.
	*pOut_tileDesc = pFoundDesc;
	
	//Now subtract the highest tileset index from the tileID
	tileID -= maxTilesetIndex;
	
	*p_InOut_tileID = tileID;
}


Layer* Game::GetLayer(LevelLayer layer)
{
	return &m_layers[layer];
}


SpawnableEntity* Game::GetSpawnableEntityByTiledUniqueID(u32 tiledUniqueID)
{
	for(u32 i=0; i<m_numSpawnableEntities; ++i)
	{
		SpawnableEntity* pEnt = &m_spawnableEntities[i];
		if(pEnt->tiledUniqueID == tiledUniqueID)
		{
			return pEnt;
		}
	}
	
	return NULL;
}


void Game::ToggleTileVisibility(Tile* pTile, bool isVisible)
{
	if(pTile != NULL && pTile->tileID != -1)
	{
		RenderableGeometry3D* pCurrRenderable = (RenderableGeometry3D*)COREOBJECTMANAGER->GetObjectByHandle(pTile->hRenderable);
		if(pCurrRenderable != NULL)
		{
			if(isVisible)
			{
				pCurrRenderable->material.flags |= RenderFlag_Visible;
			}
			else
			{
				pCurrRenderable->material.flags &= ~RenderFlag_Visible;
			}
		}
	}
}

								 
void Game::ToggleTileVisibility(LevelLayer levelLayer,u32 tileIndex_X,u32 tileIndex_Y,bool isVisible)
{
	Tile* pTile = &ARRAY2D(m_layers[levelLayer].tiles, tileIndex_X, tileIndex_Y, m_layers[levelLayer].numTilesX);
	ToggleTileVisibility(pTile,isVisible);
}


void Game::SetFollowCamTarget(const vec3* pFollowCamPos)
{
	CopyVec3(&m_followCamPos,pFollowCamPos);
}


void Game::SetCameraMode(CameraMode mode)
{
	m_cameraMode = mode;
	
	if(mode == CameraMode_FollowCam)
	{
		m_camLerpTimer = 1.0f;
		m_camLerpTotalTime = 1.0f;
		
		CopyVec3(&m_desiredCamPos,&m_followCamPos);
		CopyVec3(&m_startCamPos,&m_camPos);
	}
}


void Game::DestroyTile(s32 index_x, s32 index_Y, const vec2* pVel)
{
	if(m_numTilesToDelete != GAME_MAX_STORED_DELETABLE_TILES)
	{
		TileDestructionData* pData = &m_tilesToDelete[m_numTilesToDelete];
		
		pData->tilePos.x = index_x;
		pData->tilePos.y = index_Y;
		CopyVec2(&pData->hitVel,pVel);
	}
	
	++m_numTilesToDelete;
}


void Game::Box2D_SetNumIterations(u32 velIterations, u32 posIterations)
{
	m_Box2D_NumVelocitySteps = velIterations;
	m_Box2D_NumPositionSteps = posIterations;
}


void Game::Box2D_SetPhysicsIsLocked(bool isLocked)
{
	m_Box2D_PhysicsIsLocked = isLocked;
}


void Game::Box2D_TogglePhysicsDebug(bool allowPhysicsDebugDraw)
{
#if defined(DEBUG) || defined(_DEBUG)
	if(allowPhysicsDebugDraw == true)
	{
		m_Box2D_pWorld->SetDebugDraw(m_Box2D_pDebugDraw);
	}
	else
	{
		m_Box2D_pWorld->SetDebugDraw(NULL);
	}
#endif
}


void Game::Box2D_ResetWorld()
{
	if(m_Box2D_pWorld != NULL)
	{
		delete m_Box2D_pWorld;
		m_Box2D_pWorld = NULL;
	}
    
    if(m_Box2D_pContactListener != NULL)
	{
		delete m_Box2D_pContactListener;
	}
	
	Box2D_Init(m_Box2D_ContinuousPhysicsEnabled,m_Box2D_allowObjectsToSleep);	
}


void Game::Box2D_Init(bool continuousPhysicsEnabled, bool allowObjectsToSleep)
{
	m_Box2D_ContinuousPhysicsEnabled = continuousPhysicsEnabled;
	m_Box2D_allowObjectsToSleep = allowObjectsToSleep;
	
	b2Vec2 gravity;
	gravity.Set(0.0f, 10.0f);
	
	m_Box2D_pWorld = new b2World(gravity);
	
#if defined(DEBUG) || defined(_DEBUG)
	if(m_Box2D_pDebugDraw == NULL)
	{
		m_Box2D_pDebugDraw = new Box2DDebugDraw;
		m_Box2D_pDebugDraw->SetFlags(0xFFFFFFFF);
		m_Box2D_pWorld->SetDebugDraw(m_Box2D_pDebugDraw);
	}
#endif
	
	m_Box2D_pWorld->SetContinuousPhysics(continuousPhysicsEnabled);
	m_Box2D_pWorld->SetAllowSleeping(false);
	b2BodyDef bodyDef;
	m_Box2D_pGroundBody = m_Box2D_pWorld->CreateBody(&bodyDef);
	
	m_Box2D_pContactListener = new Box2DContactListener;
	
	m_Box2D_pWorld->SetContactListener(m_Box2D_pContactListener);
}


b2World* Game::Box2D_GetWorld()
{
	return m_Box2D_pWorld;
}


void Game::Box2D_SetContactListener(b2ContactListener* pContactListener)
{
	m_Box2D_pWorld->SetContactListener(pContactListener);
}


b2Body* Game::Box2D_GetGroundBody()
{
	return m_Box2D_pGroundBody;
}


void Game::Box2D_SetGravity(f32 x, f32 y)
{
	m_Box2D_pWorld->SetGravity(b2Vec2(x,y));
}


void Game::Box2D_SetDefaultCollisionFriction(f32 friction)
{
	m_Box2D_defaultCollisionFriction = friction;
}


b2Body* Game::Box2D_CreateBodyForTileIndex(s32 tileIndex, s32 posX, s32 posY)
{
	const f32 halfTileSize = GetHalfTileSizeMeters();
	
	vec3 pos;
	GetPositionFromTileIndices(posX, posY, &pos);
	
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	
	b2FixtureDef fixtureDef;
	fixtureDef.density = 1;
	fixtureDef.friction = m_Box2D_defaultCollisionFriction;
	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(halfTileSize,halfTileSize);
	fixtureDef.shape = &polygonShape;
	fixtureDef.filter.categoryBits = Box2D_GetCollisionFlagsForTileIndex(tileIndex);
	fixtureDef.filter.maskBits = 0xFFFF;
	
	bodyDef.position.Set(pos.x/m_pixelsPerMeter, pos.y/m_pixelsPerMeter);
	
	b2Body* pBody = m_Box2D_pWorld->CreateBody(&bodyDef);
	pBody->CreateFixture(&fixtureDef);
	
	return pBody;
}


void Game::TiledLevel_DeleteObjectIfOffscreen_X(CoreObject* pObject, vec3* pPos, f32 scale, f32 distToCheck)
{
    Layer* pLayerDesc = &m_layers[LevelLayer_Main1];

    const f32 posX = pPos->x;
    
    const f32 deleteXMin = -pLayerDesc->position.x - scale - distToCheck;
    const f32 deleteXMax = -pLayerDesc->position.x + GLRENDERER->screenWidth_points + scale + distToCheck;
    
    if(posX < deleteXMin || posX > deleteXMax)
    {
        pObject->DeleteObject();
    }
}


void Game::TiledLevel_DeleteObjectIfOffscreen_Y(CoreObject* pObject, vec3* pPos, f32 scale, f32 distToCheck)
{
    Layer* pLayerDesc = &m_layers[LevelLayer_Main1];
    
    const f32 posY = pPos->y;
    
    const f32 deleteYMin = -pLayerDesc->position.y - scale - distToCheck;
    const f32 deleteYMax = -pLayerDesc->position.y + GLRENDERER->screenHeight_points + scale + distToCheck;
    
    if(posY < deleteYMin || posY > deleteYMax)
    {
        pObject->DeleteObject();
    }
}


bool Game::TiledLevel_CheckIsOnScreen_X(vec3* pPos, f32 scale)
{
	Layer* pLayerDesc = &m_layers[LevelLayer_Main1];
	
    const f32 posX = pPos->x;
    
    const f32 xMin = -pLayerDesc->position.x + scale;
    const f32 xMax = -pLayerDesc->position.x + GLRENDERER->screenWidth_points - scale;
    
    if(posX < xMin || posX > xMax)
    {
        return false;
    }
	else
	{
		return true;
	}
}


bool Game::TiledLevel_GetGroundPos(vec3* pOut_GroundPos, vec3* pOut_GroundNormal, const vec3* pPos)
{
    //TODO: make this run faster
    
    //NOTE: this assumes all your points are in left to right order
    
    //Loop through all the collision on the ground
    for(u32 i=0; i<m_LinePointLists.size()-1; ++i)
    {
        LinePointList* pSegment = &m_LinePointLists[i];
        
        //If we are farther than the entire segment, skip this segment
        vec2* pVecLast = &pSegment->pPoints[pSegment->numPoints-1];
        if(pPos->x > pVecLast->x)
        {
            continue;
        }
        
        for(u32 vertIDX=0; vertIDX<pSegment->numPoints; ++vertIDX)
        {
            vec2* pVec0 = &pSegment->pPoints[vertIDX];
            vec2* pVec1 = &pSegment->pPoints[vertIDX+1];
            
            if(pPos->x < pVec0->x || pPos->x > pVec1->x)
            {
                continue;
            }            
            
            vec2 distVec;
            SubVec2(&distVec, pVec1, pVec0);
            
            const f32 vertDistX = distVec.x;
            const f32 posDistX = pPos->x - pVec0->x;
            
            const f32 lerpT = posDistX/vertDistX;
            pOut_GroundPos->y = Lerp(pVec0->y,pVec1->y,lerpT);
            
            //If this position ends up being above
            //pPos then it is not valid.
            if(pOut_GroundPos->y < pPos->y)
            {
                //Return false because we're not
                //going to find anything better.
                return false;
            }
            
            TryNormalizeVec2_Self(&distVec);
            
            pOut_GroundNormal->x = distVec.y;
            pOut_GroundNormal->y = -distVec.x;
            pOut_GroundNormal->z = 0.0f;
            
            

            pOut_GroundPos->x = pPos->x;
            
            pOut_GroundPos->z = 0.0f;
            
            return true;
        }
    }
    
    //Failure...
    return false;
}


u32* Game::GetHUDTextureByNameSig(u32 nameSig)
{
	for(u32 i=0; i<m_numHUDTextures; ++i)
	{
		HUDTexture* pCurrHUDTexture = &m_HUDTextures[i];
		if(pCurrHUDTexture->nameSig == nameSig)
		{
			return &pCurrHUDTexture->textureHandle;
		}
	}
	
	return NULL;
}


void Game::Reset()
{
#if defined(_DEBUG_PC)
	if(m_pUIEditor != NULL)
	{
		m_pUIEditor->Clear();
	}
#endif
}


CoreUIView* Game::LoadCoreUIFromXML(std::string& path, std::string& filename)
{
	std::string filenameWithPath(path+filename);
	
	pugi::xml_document xmlDoc;
	pugi::xml_parse_result result = xmlDoc.load_file(GetPathToFile(filenameWithPath.c_str()).c_str());
	
	if(result.status != pugi::status_ok)
	{
		COREDEBUG_PrintDebugMessage("ERROR: LoadCoreUIViewFromXML->Failed to parse file: %s",filenameWithPath.c_str());
		return NULL;
	}
	
	//Load textures
	for (pugi::xml_node texture = xmlDoc.child("texture"); texture; texture = texture.next_sibling("texture"))
	{
		//We can't add unlimited HUD textures
		if(m_numHUDTextures == GAME_MAX_HUD_TEXTURES)
		{
			COREDEBUG_PrintDebugMessage("ERROR: LoadCoreUIViewFromXML->Ran out of texture slots");
			
			break;
		}
		
		//Get name signature
		u32 nameSig = 0;
		pugi::xml_attribute name_Attrib = texture.attribute("name");
		if(name_Attrib.empty() == false)
		{
			nameSig = Hash(name_Attrib.value());
		}
		
		bool nameFound = false;
		
		//Check if it exists
		for(u32 i=0; i<m_numHUDTextures; ++i)
		{
			if(m_HUDTextures[i].nameSig == nameSig)
			{
				nameFound = true;
				break;
			}
		}
		
		//If the texture was not found, add it
		if(nameFound == false)
		{
			pugi::xml_attribute file_Attrib = texture.attribute("file");
			if(file_Attrib.empty() == false)
			{
				HUDTexture* pCurrTexture = &m_HUDTextures[m_numHUDTextures];
				pCurrTexture->textureHandle = 0;
				
				pCurrTexture->nameSig = nameSig;
				
				std::string textureFileName = file_Attrib.value();
				std::string texFilenameWithPath(path+textureFileName);
				
				GLRENDERER->LoadTexture(texFilenameWithPath.c_str(), ImageType_PNG, &pCurrTexture->textureHandle, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE,true);
				
				COREDEBUG_PrintMessage("GUITexture: %s, id: %d",texFilenameWithPath.c_str(),pCurrTexture->textureHandle);
				
				assert(pCurrTexture->textureHandle);
				
				++m_numHUDTextures;
			}
		}
	}
	
	CoreUIView* pMainView = NULL;
	bool shouldPrintWarning = false;
	
	//Load views
	for (pugi::xml_node view = xmlDoc.child("view"); view; view = view.next_sibling("view"))
	{
		CoreUIView* pView = g_Factory_CoreUIView.CreateObject(0);
		if(pView != NULL)
		{
			//Spawn view using XML settings
			pView->SpawnInit(&view, NULL);
			
			if(pMainView == NULL)
			{
				pMainView = pView;
			}
			else
			{
				shouldPrintWarning = true;
			}
		}
	}
	
	if(pMainView != NULL)
	{
		if(shouldPrintWarning == true)
		{
			COREDEBUG_PrintDebugMessage("ERROR: LoadCoreUIFromXML-> You have more than one root view in the XML file!");
		}
		
#if defined(_DEBUG_PC)
		CoreUI_Container container;
		container.rootView = pMainView->GetHandle();
		container.filepath = filenameWithPath;
		
		if(m_pUIEditor == NULL)
		{
			m_pUIEditor = new CoreUIEditor();
		}
		
		m_pUIEditor->AddViewContainer(container);
#endif
		
		pMainView->LayoutView();

		//Return handle to view
		return pMainView;
	}
	else
	{
		//Return invalid handle
		return NULL;
	}
}


bool Game::LoadTiledLevelFromTMX(std::string& path, std::string& filename, u32 tileWidthPixels, f32 tileSizeMeters)
{
	const s32 tilesOnScreenX = GLRENDERER->screenWidth_points/tileWidthPixels;
	const s32 tilesOnScreenY = GLRENDERER->screenHeight_points/tileWidthPixels;
	
	const s32 maxTiles = (tilesOnScreenX+1)*(tilesOnScreenY+1);
	const u32 vertsPerTile = 6;
	const u32 numTileVerts = vertsPerTile*maxTiles;
	m_tileVertDataSize = sizeof(TileVert)*numTileVerts;
	
	for(u32 i=0; i<GAME_MAX_TILESET_DESCRIPTIONS; ++i)
	{
		TileSetDescription* pDesc = &m_tileSetDescriptions[i];

		if(pDesc->textureFileName != NULL)
		{
			delete[] pDesc->textureFileName;
			pDesc->textureFileName = NULL;
		}
		
		if(pDesc->name != NULL)
		{
			delete[] pDesc->name;
			pDesc->name = NULL;
		}
	}
	
	for(u32 i=0; i<m_LinePointLists.size(); ++i)
    {
		if(m_LinePointLists[i].pPoints != NULL)
		{
			delete[] m_LinePointLists[i].pPoints;
		}
    }
	
    m_LinePointLists.clear();
    
	for(LevelLayer i=(LevelLayer)0; i<NumLevelLayers; ++i)
	{
		Layer* pCurrLayer = &m_layers[i];
		
		if(pCurrLayer->pLevelData != NULL)
		{
			delete[] pCurrLayer->pLevelData;
			pCurrLayer->pLevelData = NULL;
		}
		
		
		if(pCurrLayer->tiles != NULL)
		{
			delete[] pCurrLayer->tiles;
			pCurrLayer->tiles = NULL;
		}
	}
	
	if(m_pTileVerts != NULL)
	{
		delete[] m_pTileVerts;
		m_pTileVerts = NULL;
	}
	
	m_pTileVerts = new TileVert[numTileVerts];
	
	COREDEBUG_PrintDebugMessage("Creating VBO for layer...");
	GLRENDERER->CreateVBO(&m_tileVAOHandle,&m_tileVBOHandle,(void*)m_pTileVerts,m_tileVertDataSize,GL_DYNAMIC_DRAW,&g_Tile_AttribData[0],g_Tile_NumAttributes,sizeof(TileVert));
	COREDEBUG_PrintDebugMessage("VBO created!");
	
	m_numTilesToDelete = 0;
	
	const u32 tileType = Hash("Tile");
	
	m_levelHasCamRestraints = false;
	
	m_tileSizeMeters = tileSizeMeters;
	
	m_pixelsPerMeter = (f32)tileWidthPixels/tileSizeMeters;

	m_halfTileSizeMeters = tileSizeMeters*0.5f;
	
	m_numSpawnableEntities = 0;

	std::string filenameWithPath(path+filename);
	
	pugi::xml_parse_result result = m_TMXDoc.load_file(GetPathToFile(filenameWithPath.c_str()).c_str());
	
	if(result)
	{
		COREDEBUG_PrintDebugMessage("Parsing map file was successful!\n");

		pugi::xml_node map = m_TMXDoc.child("map");
		
		const u32 mapTileSizeX = atoi(map.attribute("tilewidth").value());
		//const u32 mapTileSizeY = atoi(map.attribute("tileheight").value());
		
		m_tiledLevelDescription.tileDisplaySizeX = tileWidthPixels;
		m_tiledLevelDescription.tileDisplaySizeY = tileWidthPixels;
		m_tiledLevelDescription.halfTileDisplaySizeX = m_tiledLevelDescription.tileDisplaySizeX/2;
		m_tiledLevelDescription.halfTileDisplaySizeY = m_tiledLevelDescription.tileDisplaySizeY/2;
		
		m_unitConversionScale = (f32)tileWidthPixels/(f32)mapTileSizeX;
		
		m_numTileSetDescriptions = 0;
		for (pugi::xml_node tileset = map.child("tileset"); tileset; tileset = tileset.next_sibling("tileset"),++m_numTileSetDescriptions)
		{
			TileSetDescription* pDesc = &m_tileSetDescriptions[m_numTileSetDescriptions];

			const char* descName = tileset.attribute("name").value();
			const size_t descNameSize = strlen(descName);
			pDesc->name = new char[descNameSize+1];
			strcpy(pDesc->name, descName);
			
			pDesc->firstTileIndex = atoi(tileset.attribute("firstgid").value());
			pDesc->tileSizeX = atoi(tileset.attribute("tilewidth").value());
			pDesc->tileSizeY = atoi(tileset.attribute("tileheight").value());
			
			pugi::xml_node textureNode = tileset.child("image");
			
			const char* textureName = textureNode.attribute("source").value();
			const size_t textureNameSize = strlen(textureName);
			pDesc->textureFileName = new char[textureNameSize+1];
			strcpy(pDesc->textureFileName, textureName);
			
			pDesc->textureSizeX = atoi(textureNode.attribute("width").value());
			pDesc->textureSizeY = atoi(textureNode.attribute("height").value());
			
			//Load the tile texture into memory
			std::string textureFileName(pDesc->textureFileName);
			std::string texFilenameWithPath(path+textureFileName);
			
			GLRENDERER->LoadTexture(texFilenameWithPath.c_str(), ImageType_PNG, &pDesc->loadedTextureID, GL_NEAREST, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE,true);
			
			//Do some useful calculations
			
			pDesc->numTextureTilesX = pDesc->textureSizeX/pDesc->tileSizeX;
			pDesc->numTextureTilesY = pDesc->textureSizeY/pDesc->tileSizeY;
			
			pDesc->halfTexelOffset = 0.5f/pDesc->textureSizeX;

			switch(pDesc->numTextureTilesX)
			{
				case 2:
				{
					pDesc->pModelData = &g_Square_Tiled_2_modelData;
					
					break;
				}
				case 4:
				{
					pDesc->pModelData = &g_Square_Tiled_4_modelData;
					
					break;
				}
				case 8:
				{
					pDesc->pModelData = &g_Square_Tiled_8_modelData;
					
					break;
				}
				case 16:
				{
					pDesc->pModelData = &g_Square_Tiled_16_modelData;
					
					break;
				}
				case 32:
				{
					pDesc->pModelData = &g_Square_Tiled_32_modelData;
					
					break;
				}
			}
		}
		
		for (pugi::xml_node layer = map.child("layer"); layer; layer = layer.next_sibling("layer"))
		{
			const char* layerName = layer.attribute("name").value();

			s32* pData = NULL;

			const u32 width = atoi(layer.attribute("width").value());
			const u32 height = atoi(layer.attribute("height").value());

			COREDEBUG_PrintDebugMessage("\nLayer %s, width: %d, height, %d",layerName,width,height);
			
			LevelLayer currLayer = LevelLayer_Invalid;
			
			if(strcmp(layerName, "Dummy") == 0)
			{
				continue;
			}
			else if(strcmp(layerName, "Main0") == 0)
			{
				currLayer = LevelLayer_Main0;
			}
			else if(strcmp(layerName, "Main1") == 0)
			{
				currLayer = LevelLayer_Main1;
			}
			else if(strcmp(layerName, "Parallax0") == 0)
			{
				currLayer = LevelLayer_Parallax0;
			}
			else if(strcmp(layerName, "Parallax1") == 0)
			{
				currLayer = LevelLayer_Parallax1;
			}
			else if(strcmp(layerName, "Parallax2") == 0)
			{
				currLayer = LevelLayer_Parallax2;
			}
			else if(strcmp(layerName, "Parallax3") == 0)
			{
				currLayer = LevelLayer_Parallax3;
			}
			else if(strcmp(layerName, "Parallax4") == 0)
			{
				currLayer = LevelLayer_Parallax4;
			}
			else if(strcmp(layerName, "Collision") == 0)
			{
				currLayer = LevelLayer_Collision;
			}
			else if(strcmp(layerName, "TileObjectArt") == 0)
			{
				currLayer = LevelLayer_TileObjectArt;
			}
			else if(strcmp(layerName, "CameraExtents") == 0)
			{
				currLayer = LevelLayer_CameraExtents;
			}
			
			if(currLayer == LevelLayer_Invalid)
			{
				COREDEBUG_PrintDebugMessage("Invalid Layer: Skipping...");

				continue;
			}
			
			Layer* pCurrLayer = &m_layers[currLayer];
			
			const u32 numTiles = width*height;
			
			//const vec3* pClearColor = GLRENDERER->GetClearColor();
			
			switch(currLayer)
			{
				case LevelLayer_Parallax4:
				{
					pCurrLayer->material = MT_WorldSpace_TextureOnly;
					
					/*pCurrLayer->material = MT_TextureAndFogColorWithTexcoordOffset;
					
					CopyVec3((vec3*)&pCurrLayer->fogColor,pClearColor);
					pCurrLayer->fogColor.w = 0.95f;
					pCurrLayer->blurAmount = 0.95f;*/
					
					break;
				}
				case LevelLayer_Parallax3:
				{
					pCurrLayer->material = MT_WorldSpace_TextureOnly;
					
					/*pCurrLayer->material = MT_TextureAndFogColorWithTexcoordOffset;
					
					CopyVec3((vec3*)&pCurrLayer->fogColor,pClearColor);
					pCurrLayer->fogColor.w = 0.8f;
					pCurrLayer->blurAmount = 0.8f;*/
					
					break;
				}
				case LevelLayer_Parallax2:
				{
					pCurrLayer->material = MT_WorldSpace_TextureOnly;
					
					/*pCurrLayer->material = MT_TextureAndFogColorWithTexcoordOffset;
					
					CopyVec3((vec3*)&pCurrLayer->fogColor,pClearColor);
					pCurrLayer->fogColor.w = 0.6f;
					pCurrLayer->blurAmount = 0.6f;*/
					
					break;
				}
				case LevelLayer_Parallax1:
				{
					pCurrLayer->material = MT_WorldSpace_TextureOnly;
					
					/*pCurrLayer->material = MT_TextureAndFogColorWithTexcoordOffset;
					
					CopyVec3((vec3*)&pCurrLayer->fogColor,pClearColor);
					pCurrLayer->fogColor.w = 0.4f;
					pCurrLayer->blurAmount = 0.4f;*/

					break;
				}
				case LevelLayer_Parallax0:
				{
					pCurrLayer->material = MT_WorldSpace_TextureOnly;
					
					/*pCurrLayer->material = MT_TextureAndFogColorWithTexcoordOffset;
					
					CopyVec3((vec3*)&pCurrLayer->fogColor,pClearColor);
					pCurrLayer->fogColor.w = 0.2f;*/

					break;
				}
				case LevelLayer_TileObjectArt:
				case LevelLayer_Collision:
				case LevelLayer_Main0:
				case LevelLayer_Main1:
				{
					pCurrLayer->material = MT_WorldSpace_TextureOnly;
					
					break;
				}
				default:
				{
					break;
				}
			}
	
			switch(currLayer)
			{
				case LevelLayer_Parallax4:
				case LevelLayer_Parallax3:
				case LevelLayer_Parallax2:
				case LevelLayer_Parallax1:
				case LevelLayer_Parallax0:
				case LevelLayer_Main0:
				case LevelLayer_Main1:
				case LevelLayer_Collision:
				case LevelLayer_TileObjectArt:
				case LevelLayer_CameraExtents:
				{
					pCurrLayer->numTilesX = width;
					pCurrLayer->numTilesY = height;
					
					//TODO: seems like there's a bit of waste
					//if this camera extents layer is in here
					pData = new s32[numTiles];
					
					pCurrLayer->pLevelData = pData;
					
					CopyVec3(&pCurrLayer->position,&vec3_zero);
					
					RenderableGeometry3D* pGeom;
					pCurrLayer->hRenderable = GLRENDERER->CreateRenderableGeometry3D(RenderableObjectType_Normal, &pGeom); 

					if(pGeom != NULL)
					{
						const RenderLayer renderLayer = (RenderLayer)(RenderLayer_Background0+currLayer);
						
						GLRENDERER->InitRenderableGeometry3D(pGeom, &g_drawStruct_RenderTileLayer, pCurrLayer, MT_WorldSpace_TextureOnly, 0, NULL, renderLayer, BlendMode_Normal, RenderFlagDefaults_2DTexture_AlphaBlended_Fullscreen|RenderFlag_Visible);
					}
					
					break;
				}
				default:
				{
					break;
				}
			}
			
			if(pData == NULL)
			{
				//If we have no data to write into, we might as well skip
				COREDEBUG_PrintDebugMessage("This layer will not be saved. Skipping...");

				continue;
			}
			
			pugi::xml_node data = layer.child("data");
			//std::cout << "data: " << data.first_child().value() << '\n';
			
			char* dataToDecode = (char*)data.first_child().value();
			
			const int BUFFER_SIZE = 4096;
			
			unsigned char decodedData[BUFFER_SIZE];
			
			size_t outputLength = base64_decode(dataToDecode,decodedData, BUFFER_SIZE);
			assert(outputLength != -1);

			COREDEBUG_PrintDebugMessage("base64_decode...");
			
			const u32 dataSize = numTiles*sizeof(u32);
			unsigned long bufferSize = dataSize;
			
			switch(uncompress((Bytef*)pData, &bufferSize, (Bytef*)decodedData, outputLength))
			{
				case Z_OK:
				{
					COREDEBUG_PrintDebugMessage("ZLIB uncompress was successful!\n");
					
					break;
				}
				case Z_MEM_ERROR:
				{
					COREDEBUG_PrintDebugMessage("ZLIB ERROR: Uncompress failed due to MEMORY ERROR.  Exiting program...\n");
					return false;
				}
				case Z_BUF_ERROR:
				{
					COREDEBUG_PrintDebugMessage("ZLIB ERROR: Uncompress failed due to BUFFER ERROR.  Exiting program...\n");
					return false;
				}
				case Z_DATA_ERROR:
				{
					COREDEBUG_PrintDebugMessage("ZLIB ERROR: Uncompress failed due to DATA ERROR.  Exiting program...\n");
					return false;
				}
			}
			
			//Allocate an array of tiles
			pCurrLayer->tiles = new Tile[width*height];
			
			//Set all the tile indices up
			for(u32 y=0; y<height; ++y)
			{
				for(u32 x=0; x<width; ++x)
				{
					Tile* pTile = &ARRAY2D(pCurrLayer->tiles, x, y, width);
					pTile->hRenderable = CoreObjectHandle();

					pTile->tileID = ARRAY2D(pData, x, y, width);
					pTile->flags = (pTile->tileID >> 24) & 0x000000FF;
					ConvertTileID(&pTile->tileID, &pTile->pDesc);
					
					pTile->indexX = x;
					pTile->indexY = y;
					
					pTile->pBody = NULL;
				}
			}
			
			//Create collision if Box2D is enabled
			//Collision!
			if(m_Box2D_pWorld != NULL
			   && currLayer == LevelLayer_Collision)
			{
				for(u32 y=0; y<height; ++y)
				{
					for(u32 x=0; x<width; ++x)
					{
						Tile* pTile = &ARRAY2D(pCurrLayer->tiles, x, y, width);
						if(pTile->tileID == -1)
						{
							continue;
						}
						
						pTile->SetEntityType(tileType);
						
						pTile->pBody = Box2D_CreateBodyForTileIndex(pTile->tileID, x, y);
						
						pTile->pBody->SetUserData(pTile);
					}
				}
			}
			
			//Create the renderables for all the object art tiles
			if(currLayer == LevelLayer_TileObjectArt)
			{
				for(u32 y=0; y<height; ++y)
				{
					for(u32 x=0; x<width; ++x)
					{
						Tile* pTile = &ARRAY2D(pCurrLayer->tiles, x, y, width);
						if(pTile->tileID == -1)
						{
							continue;
						}
						
						RenderableGeometry3D* pGeom;
						CreateRenderableTile(pTile,&pGeom,RenderLayer_AlphaBlended2,MT_TextureOnlyWithTexcoordOffset,&pTile->texCoordOffset,false);
						//TODO: do something better than this if possible
						pGeom->material.uniqueUniformValues[0] = (u8*)&pTile->texCoordOffset;
					}
				}
			}
			
			if(currLayer == LevelLayer_CameraExtents)
			{
				m_levelHasCamRestraints = true;
				
				m_camExtentBR_X = 0;
				
				m_camExtentTL_X = 999999;
				
				m_camExtentBR_Y = 0;
				
				m_camExtentTL_Y = 999999;
				
				for(u32 y=0; y<height; ++y)
				{
					for(u32 x=0; x<width; ++x)
					{
						Tile* pTile = &ARRAY2D(pCurrLayer->tiles, x, y, width);
						if(pTile->tileID == -1)
						{
							continue;
						}
						
						vec3 extentPos;
						GetPositionFromTileIndices(x, y, &extentPos);
						
						if(extentPos.x < m_camExtentTL_X)
						{
							m_camExtentTL_X = extentPos.x;
						}
						if(extentPos.x > m_camExtentBR_X)
						{
							m_camExtentBR_X = extentPos.x;
						}
						
						if(extentPos.y < m_camExtentTL_Y)
						{
							m_camExtentTL_Y = extentPos.y;
						}
						if(extentPos.y > m_camExtentBR_Y)
						{
							m_camExtentBR_Y = extentPos.y;
						}
					}
				}
				
				const f32 halfTileSizePixels = tileWidthPixels*0.5f;
				m_camExtentTL_X -= halfTileSizePixels;
				m_camExtentBR_X += halfTileSizePixels;
				m_camExtentTL_Y -= halfTileSizePixels;
				m_camExtentBR_Y += halfTileSizePixels;
			}
		}
		
	
		for (pugi::xml_node layer = map.child("objectgroup"); layer; layer = layer.next_sibling("objectgroup"))
		{
			const char* layerName = layer.attribute("name").value();
			
			if(strcmp(layerName,"Dummy") == 0)
			{
				continue;
			}

			COREDEBUG_PrintDebugMessage("Layer: %s",layerName);
			
			//Create collision
			if(strcmp(layerName,"Collision") == 0)
			{
				for (pugi::xml_node object = layer.child("object"); object; object = object.next_sibling("object"))
				{
					LinePointList newSeg;
					GU_Create2DPathPointsFromXML(object,&newSeg,true);
					m_LinePointLists.push_back(newSeg);
				}
				
				//Sort the segments
				std::sort(m_LinePointLists.begin(),m_LinePointLists.end(),SortLinePointListByX);
			}
			//Create objects
			else
			{
				for (pugi::xml_node object = layer.child("object"); object; object = object.next_sibling("object"))
				{
					SpawnableEntity* pCurrEnt = &m_spawnableEntities[m_numSpawnableEntities];
					++m_numSpawnableEntities;
					
					pCurrEnt->pObject = NULL;
					
					pCurrEnt->tiledUniqueID = atoi(object.attribute("uniqueID").value());
					
					const f32 x = (f32)atoi(object.attribute("x").value())*m_unitConversionScale;
					const f32 y = (f32)atoi(object.attribute("y").value())*m_unitConversionScale;
					
					pCurrEnt->position.x = x;
					pCurrEnt->position.y = y;
					
					f32 width;
					f32 height;
					
					pugi::xml_attribute gidAttrib = object.attribute("gid");
					if(gidAttrib.empty() == false)
					{
						pCurrEnt->tileID = gidAttrib.as_int();
						ConvertTileID(&pCurrEnt->tileID, &pCurrEnt->pDesc);
						
						width = GAME->GetTileSize();
						height = GAME->GetTileSize();
						
						//TODO: this might be wrong.  Will need more testing
						pCurrEnt->position.x += width/2;
						pCurrEnt->position.y -= height/2;
						
						GetTileIndicesFromPosition((vec2*)&pCurrEnt->position, &pCurrEnt->tileIndexX, &pCurrEnt->tileIndexY);
					}
					else
					{
						pCurrEnt->tileID = -1;
						pCurrEnt->pDesc = NULL;
						
						width = (f32)atoi(object.attribute("width").value())*m_unitConversionScale;
						height = (f32)atoi(object.attribute("height").value())*m_unitConversionScale;
						
						pCurrEnt->position.x += width/2.0f;
						pCurrEnt->position.y += height/2.0f;
						
						pCurrEnt->tileIndexX = 0;
						pCurrEnt->tileIndexY = 0;
					}
					
					pCurrEnt->position.z = 0.0f;
					
					const char* typeString = object.attribute("type").value();
					pCurrEnt->type = Hash(typeString);
					
					pCurrEnt->scale.x = width;
					pCurrEnt->scale.y = height;
					
					//Find properties of the object
					//TODO: not do this horrible thing
					pCurrEnt->node = object;
					
					pCurrEnt->autospawn = true;
					
					pugi::xml_node propertiesNode = object.child("properties");
					
					for (pugi::xml_node property = propertiesNode.child("property"); property; property = property.next_sibling("property"))
					{
						const char* propNameString = property.attribute("name").value();
						const char* valueString = property.attribute("value").value();
						
						if(strcmp(propNameString, "AutoSpawn") == 0)
						{
							if(strcmp(valueString, "false") == 0)
							{
								pCurrEnt->autospawn = false;
							}
						}
					}
					
					
					if(pCurrEnt->autospawn == false)
					{
						continue;
					}

					switch(pCurrEnt->type)
					{
						case g_Type_ScriptObject:
						{
							pCurrEnt->pObject = g_Factory_ScriptObject.CreateObject(pCurrEnt->type);
							
							break;
						}
						case g_Type_CollisionBox:
						{
							pCurrEnt->pObject = g_Factory_Trigger_2D.CreateObject(pCurrEnt->type);
							
							break;
						}
						case g_Type_ObjectGroup:
						{
							pCurrEnt->pObject = g_Factory_ObjectGroup.CreateObject(pCurrEnt->type);
							
							break;
						}
						
						case g_Type_TileAffector:
						{
							pCurrEnt->pObject = g_Factory_TileAffector.CreateObject(pCurrEnt->type);
							
							break;
						}
						case g_Type_SoundPlayerType:
						{
							pCurrEnt->pObject = g_Factory_SoundPlayer.CreateObject(pCurrEnt->type);
							
							break;
						}
						case g_Type_Spawner:
						{
							pCurrEnt->pObject = g_Factory_Spawner.CreateObject(pCurrEnt->type);
							
							break;
						}
						case g_Type_CurvePoints_2D:
						{
							pCurrEnt->pObject = g_Factory_CurvePoints_2D.CreateObject(pCurrEnt->type);
							
							break;
						}
						case g_Type_NullObject:
						{
							pCurrEnt->pObject = g_Factory_NullObject.CreateObject(pCurrEnt->type);
							
							break;
						}
						default:
						{
							//If it's not a core game object then it's a custom one
							pCurrEnt->pObject = this->CreateObject(pCurrEnt->type);
							
							break;
						}
					}
				}
			}
		}
        
        //Load all the objects resources!
		for(u32 i=0; i<m_numSpawnableEntities; ++i)
		{
            SpawnableEntity* pEnt = &m_spawnableEntities[i];
            GAME->LoadResourcesForType(pEnt->type);
		}
        
        //Load shared resources
		LoadSharedResources();
		
        //Perform actual loading of textures and sounds
        LoadItemSounds();  
        LoadItemArt();
        
		//All the objects have been created, now initialize them!
        //This is a good place to start looping sounds, etc.
		for(u32 i=0; i<m_numSpawnableEntities; ++i)
		{
			SpawnableEntity* pEnt = &m_spawnableEntities[i];
			if(pEnt->pObject != NULL)
			{
				pEnt->pObject->SpawnInit(pEnt);
			}
		}
		
		//All the objects have been initialized, now do the post init!
        //This is a good place to link objects together
		for(u32 i=0; i<m_numSpawnableEntities; ++i)
		{
			SpawnableEntity* pEnt = &m_spawnableEntities[i];
			if(pEnt->pObject != NULL)
			{
				pEnt->pObject->PostSpawnInit(pEnt);
			}
		}
	}
	else
	{
		COREDEBUG_PrintDebugMessage("Failed to load level file.  FILE NOT FOUND!");
	}
	
	
	return true;
}
