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

#if defined (PLATFORM_WIN)
#include <direct.h>
#include <stdlib.h>
#endif

Game* GAME = NULL;

const MaterialSettings g_Game_BlobShadowSettings =
{
	GL_LINEAR,//GLuint			textureFilterMode;
	GL_CLAMP_TO_EDGE,//GLuint			wrapModeU;
	GL_CLAMP_TO_EDGE,//GLuint			wrapModeV;
    MT_TextureOnlySimple,//RenderMaterial	renderMaterial;
    RenderFlagDefaults_CardWithAlpha,//u32				renderFlags;
	true,
};

ItemArtDescription g_Game_BlobShadowDesc =
{
	"ArtResources/Textures/shadowblob.png",//const char*		textureFileName;
	ImageType_PNG,//ImageType		imageType;
	0,//GLuint			textureHandle;
	&g_Game_BlobShadowSettings,//MaterialSettings	materialSettings;
	&g_Square1x1_modelData//ModelData*		pModelData;
};

void Game::Init()
{
#if defined (PLATFORM_WIN)
	char currentPath[_MAX_PATH];
	//GetCurrentDirectory(_MAX_PATH,currentPath);
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

	OutputDebugString(m_path.c_str());
#endif

	m_currSongSource = 0;
	m_currSongBuffer = 0;

	m_numLoadedArtDescriptions = 0;
	m_numArtDescriptionsToLoadTexturesFor = 0;
	m_numLoadedSoundDescriptions = 0;
	m_numSoundDescriptionsToLoadWavsFor = 0;
	m_ui_numButtons = 0;
	m_numBreakables = 0;
	
	m_numSongsInPlaylist = 0;
	m_currSongID = -1;
	
#if defined (PLATFORM_IOS)
	m_pTouchInput = [[[TouchInputIOS alloc]init:&m_deviceInputState]retain];
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
}

void Game::CleanUp()
{
	delete m_pCoreAudioOpenAL;
	delete m_coreObjectManager;
}

void Game::Update(f32 timeElapsed)
{
	UpdateBreakables(timeElapsed);

#if defined (PLATFORM_OSX) || defined(PLATFORM_WIN)
	
	for(u32 i=0; i<MOUSESTATE_MAX_MOUSEBUTTONS; ++i)
	{
		const MouseButtonState moustState = m_mouseState.buttonState[i];

		if(moustState == MouseButtonState_Began)
		{
			m_mouseState.buttonState[i] = MouseButtonState_Held;
		}
		else if(moustState == MouseButtonState_Ended)
		{
			m_mouseState.buttonState[i] = MouseButtonState_None;
		}
	}
#endif
}

s32 Game::AddSongToPlaylist(const char* songFilenameMP3)
{
	const u32 songID = m_numSongsInPlaylist;
	
	m_songPlaylist[songID] = new char[strlen(songFilenameMP3+1)];
	strcpy(m_songPlaylist[songID], songFilenameMP3);
	
	++m_numSongsInPlaylist;
	
	return songID;
}


std::string Game::GetPathToFile(const char* filename)
{
#if defined (PLATFORM_OSX) || defined (PLATFORM_IOS)
	NSString* fileString = [NSString stringWithCString:filename encoding:NSUTF8StringEncoding];
	NSString *fullPath = [[NSBundle mainBundle] pathForResource:[fileString lastPathComponent] ofType:nil inDirectory:[fileString stringByDeletingLastPathComponent]];
	
	return [fullPath UTF8String];
#endif

#if defined (PLATFORM_WIN)
	return m_path + std::string(filename);
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
	[m_pAudioPlayer release];
	
	NSString* fileString = [NSString stringWithCString:m_songPlaylist[songID] encoding:NSUTF8StringEncoding];
	NSString *fullPath = [[NSBundle mainBundle] pathForResource:[fileString lastPathComponent] ofType:nil inDirectory:[fileString stringByDeletingLastPathComponent]];
	NSURL *soundURL = [NSURL fileURLWithPath:fullPath];
	
	//m_pAudioPlayer = CreateAudioPlayer(fullPath,@"",YES,1.0f);
	NSError* error;
	m_pAudioPlayer = [[AVAudioPlayer alloc] initWithContentsOfURL:soundURL error:&error];
	m_pAudioPlayer.volume = volume;
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


CoreUI_Button* Game::AddUIButton(u32 width, u32 height, CoreUI_AttachSide attachSide, s32 offsetX, s32 offsetY, u32* textureHandle, s32 value, void (*callback)(s32))
{
	if(m_ui_numButtons == GAME_MAX_BUTTONS)
	{
		return NULL;
	}
	
	CoreUI_Button* pButton = &m_ui_buttons[m_ui_numButtons];
	pButton->Init(width, height, attachSide, offsetX, offsetY, textureHandle, value, callback);
	
	++m_ui_numButtons;
	
	return pButton;
}

void Game::ClearAllButtons()
{
	m_ui_numButtons = 0;
}


void Game::UpdateButtons(TouchState touchState, vec2 *pTouchPosBegin, vec2* pTouchPosCurr)
{
	for(u32 i=0; i<m_ui_numButtons; ++i)
	{
		if(touchState != TouchState_None)
		{
			m_ui_buttons[i].PressButton(touchState, pTouchPosBegin, pTouchPosCurr);
		}
	}
}


//Checks if the art will be loaded next time LoadItemArt gets called
bool Game::WillArtDescriptionBeLoaded(ItemArtDescription* pArtDesc)
{
    for(u32 i=0; i<m_numArtDescriptionsToLoadTexturesFor; ++i)
    {
        ItemArtDescription* pCurrArtDesc = m_pArtDescriptionsToLoadTexturesFor[i];
        if(pArtDesc == pCurrArtDesc)
        {
            return true;
        }
    }
    
    return false;
}


//Call many times to prepare art to be loaded later
void Game::AddItemArt(ItemArtDescription* pArtDescription)
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
        ItemArtDescription* pCurrArtDesc = m_pLoadedArtDescriptions[i];
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
		ItemArtDescription* pCurrArtDesc = m_pArtDescriptionsToLoadTexturesFor[i];
		const MaterialSettings* pMaterialSettings = pCurrArtDesc->materialSettings;
		
		//This will load the texture but if it's already loaded, it will do nothing
		GLRENDERER->LoadTexture(pCurrArtDesc->textureFileName, pCurrArtDesc->imageType, &pCurrArtDesc->textureHandle, pMaterialSettings->textureFilterMode, pMaterialSettings->wrapModeU, pMaterialSettings->wrapModeV,pMaterialSettings->flipY);
        
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
        ItemArtDescription* pCurrArtDesc = m_pLoadedArtDescriptions[i];
		GLRENDERER->DeleteTexture(&pCurrArtDesc->textureHandle);
    }
	
	m_numLoadedArtDescriptions = 0;
	m_numArtDescriptionsToLoadTexturesFor = 0;
}


//SOUND

//Checks if the art will be loaded next time LoadItemArt gets called
bool Game::WillSoundDescriptionBeLoaded(ItemSoundDescription* pSoundDesc)
{
    for(u32 i=0; i<m_numSoundDescriptionsToLoadWavsFor; ++i)
    {
        ItemSoundDescription* pCurrSoundDesc = m_pSoundDescriptionsToLoadWavsFor[i];
        if(pSoundDesc == pCurrSoundDesc)
        {
            return true;
        }
    }
    
    return false;
}


//Call many times to prepare sound to be loaded later
void Game::AddItemSound(ItemSoundDescription* pSoundDescription)
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
        ItemSoundDescription* pCurrSoundDesc = m_pLoadedSoundDescriptions[i];
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
		ItemSoundDescription* pCurrSoundDesc = m_pSoundDescriptionsToLoadWavsFor[i];
	
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
        ItemSoundDescription* pCurrSoundDesc = m_pLoadedSoundDescriptions[i];
		OPENALAUDIO->DeleteSoundBuffer(&pCurrSoundDesc->soundBufferID);
    }
	
	m_numLoadedSoundDescriptions = 0;
	m_numSoundDescriptionsToLoadWavsFor = 0;
}


void Game::UpdateBreakables(f32 timeElapsed)
{
	//TODO: this is probably BAD
	
	//Delete old breakables
	for(u32 i=0; i<m_numBreakables; )
    {
        Breakable* pCurrBreakable = &m_updatingBreakables[i];
        
        //Kill off old breakables
        if(pCurrBreakable->lifeTimer < 0.0f)
        {
			Breakable* pLastBreakable = &m_updatingBreakables[m_numBreakables-1];
			
			//Get handles to both renderables
			RenderableGeometry3D* pCurrGeom = (RenderableGeometry3D*)COREOBJECTMANAGER->GetObjectByHandle(pCurrBreakable->handleRenderable);
			//Delete the current geom because it's getting overwritten
			pCurrGeom->Uninit();

			if(m_numBreakables > 1)
			{
				RenderableGeometry3D* pLastGeom = (RenderableGeometry3D*)COREOBJECTMANAGER->GetObjectByHandle(pLastBreakable->handleRenderable);

				//Overwrite the breakable
				*pCurrBreakable = *pLastBreakable;
			}
			
			--m_numBreakables;
        }
		else
		{
			++i;
		}
	}
	
	//Update breakables
    for(u32 i=0; i<m_numBreakables; ++i)
    {
        Breakable* pCurrBreakable = &m_updatingBreakables[i];
        
		RenderableGeometry3D* pCurrGeom = (RenderableGeometry3D*)COREOBJECTMANAGER->GetObjectByHandle(pCurrBreakable->handleRenderable);
		
		pCurrBreakable->lifeTimer -= timeElapsed;
		
        const f32 breakableAlpha = ClampF(pCurrBreakable->lifeTimer/0.15f,0.0f,1.0f);
        ScaleVec4(&pCurrBreakable->diffuseColor,&pCurrBreakable->diffuseColorStart,breakableAlpha);
        
        BreakableData* pData = pCurrBreakable->pBreakableData;
        
        pCurrBreakable->currSpinAngle += pCurrBreakable->spinSpeed*timeElapsed;
        
        
        vec3* pBreakablePos = mat4f_GetPos(pCurrGeom->worldMat);
        
        pCurrBreakable->velocity.y -= pData->pSettings->gravity*timeElapsed;
        AddScaledVec3_Self(pBreakablePos,&pCurrBreakable->velocity,timeElapsed);
        
        vec3 velNorm;
        TryNormalizeVec3(&velNorm,&pCurrBreakable->velocity);
        
        const f32 maxZForScale = 40.0f;
        const f32 maxZScale = 3.5f;
        
        f32 radius = pData->radius;
        if(pData->scaleWithZ)
        {
            const f32 zScaleT = MinF(pBreakablePos->z/maxZForScale,1.0f);
            radius = Lerp(pData->radius,maxZScale,zScaleT);
        }
        
        mat4f_LoadScaledZRotation_IgnoreTranslation(pCurrGeom->worldMat, pCurrBreakable->currSpinAngle, radius);
        
										
		//Have to relink up the uniform values because they're basicaly gone
		//TODO: make this better
		pCurrGeom->material.uniqueUniformValues[0] = (u8*)&pCurrBreakable->texcoordOffset;
		pCurrGeom->material.uniqueUniformValues[1] = (u8*)&pCurrBreakable->diffuseColor;

		//Bouncing disabled for now
		
        /*if(pData->pSettings->doesBounce && pBreakablePos->y <= 0.0f)
        {
            pBreakablePos->y = 0.0f;
            
            const f32 dotProd = -DotVec3(&g_GameBox_normal_Floor,&velNorm);
            //printf("DotProd: %f\n",dotProd);
            
            const f32 finalDamp = Lerp(1.0f,pDesc->pSettings->bounceDamping,dotProd);
            //printf("Damping mult: %f\n",finalDamp);
            ScaleVec3_Self(&pCurrBreakable->velocity,finalDamp);
            
            pCurrBreakable->velocity.y *= -1.0f;
        }*/
    }
}


void Game::SpawnBreakable(BreakableData* pData, const vec3* pPosition, const vec3* pDirection, u32 breakableIndex, const vec4* diffuseColor, RenderLayer renderLayer)
{
	if(m_numBreakables == GAME_MAX_BREAKABLES)
	{
		return;
	}
	
	//printf("Spawned breakable!\n");
	
	Breakable* pCurrBreakable = &m_updatingBreakables[m_numBreakables];
	
	pCurrBreakable->pBreakableData = pData;
	ItemArtDescription* pArtDesc = &pData->itemArt;
	const MaterialSettings* pMaterial = pArtDesc->materialSettings;
	
	//[self PlaySoundByFilename:pCurrBreakable->pBreakableDescription->breakSoundName:pPosition:0.0f:FALSE];
	
	RenderableGeometry3D* pRenderable = NULL;
	pCurrBreakable->handleRenderable = GLRENDERER->CreateRenderableGeometry3D_Normal(&pRenderable);
	
	GLRENDERER->InitRenderableGeometry3D(pRenderable, pArtDesc->pModelData, pMaterial->renderMaterial, &pArtDesc->textureHandle, NULL, renderLayer, View_0, pMaterial->renderFlags|RenderFlag_Visible);
	pRenderable->material.uniqueUniformValues[0] = (u8*)&pCurrBreakable->texcoordOffset;
	pRenderable->material.uniqueUniformValues[1] = (u8*)&pCurrBreakable->diffuseColor;
	
	f32 radius = pData->radius;
	
	mat4f_LoadScale(pRenderable->worldMat, radius);
	
	vec3* pPos = mat4f_GetPos(pRenderable->worldMat);
	CopyVec3(pPos, pPosition);
	
	const f32 speed = rand_FloatRange(pData->pSettings->moveSpeedMin, pData->pSettings->moveSpeedMax);
	ScaleVec3(&pCurrBreakable->velocity,pDirection,speed);
	const f32 spinSpeed = rand_FloatRange(pData->pSettings->spinSpeedMin, pData->pSettings->spinSpeedMax);
	pCurrBreakable->spinSpeed = spinSpeed*((rand_FloatRange(0.0f, 1.0f) > 0.5f) ? -1.0f : 1.0f);
	pCurrBreakable->currSpinAngle = 0.0f;
	pCurrBreakable->lifeTimer = pData->pSettings->lifetime;
	
	CopyVec4(&pCurrBreakable->diffuseColor,diffuseColor);
	CopyVec4(&pCurrBreakable->diffuseColorStart,diffuseColor);
	
	switch (breakableIndex)
	{
		case 0:
		{
			SetVec2(&pCurrBreakable->texcoordOffset, 0.0f, 0.0f);
			break;
		}
		case 1:
		{
			SetVec2(&pCurrBreakable->texcoordOffset, 0.5f, 0.0f);
			break;
		}
		case 2:
		{
			SetVec2(&pCurrBreakable->texcoordOffset, 0.0f, 0.5f);
			break;
		}
		case 3:
		{
			SetVec2(&pCurrBreakable->texcoordOffset, 0.5f, 0.5f);
			break;
		}
		default:
		{
			break;
		}
	}
	
	++m_numBreakables;
}


