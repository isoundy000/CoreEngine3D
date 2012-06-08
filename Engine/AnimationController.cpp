//
//  AnimationController.cpp
//  Crow-Regime(iOS)
//
//  Created by Jody McAdams on 12/17/11.
//  Copyright (c) 2011 Jody McAdams. All rights reserved.
//

#include "AnimationController.h"
#include "stddef.h" //for NULL -_-
#include "Hash.h"
#include <cassert>

AnimationSet::AnimationSet()
{
	m_currAnimIDX = 0;
}

//ANIMATION SET
void AnimationSet::AddAnimation(s32* pOut_animID, u32 startFrame, u32 endFrame, f32 FPS, bool isLooping, s32 animID_NextAnim)
{
	if(m_numAnimations == ANIMATION_MAX_ANIMATIONS)
	{
		return;
	}
	
	Animation* pAnim = &m_animations[m_numAnimations];
	
	pAnim->startFrame = startFrame;
	pAnim->endFrame = endFrame;
	pAnim->FPS = FPS;
	pAnim->isLooping = isLooping;
	pAnim->animID_playWhenFinished = animID_NextAnim;
	pAnim->animID = m_currAnimIDX;
	
	*pOut_animID = m_currAnimIDX;
	++m_currAnimIDX;

	++m_numAnimations;
}


Animation* AnimationSet::GetAnimationByID(s32 animID)
{
	for(u32 i=0; i<m_numAnimations; ++i)
	{
		Animation* pCurrAnim = &m_animations[i];
		if(pCurrAnim->animID == animID)
		{
			return pCurrAnim;
		}
	}
	
	return NULL;
}


//ANIMATION PLAYER

void AnimationPlayer::PlayAnimation(s32 animID, u32 frameOffset, f32 playSpeed)
{
	if(m_pAnimSet == NULL)
	{
		return;
	}
	
	m_pCurrAnim = m_pAnimSet->GetAnimationByID(animID);
	
	if(!m_pCurrAnim)
	{
		return;
	}
	
	m_playSpeed = playSpeed;
	
	m_animIsDone = false;
	
	const u32 numFrames = m_pCurrAnim->endFrame-m_pCurrAnim->startFrame;
	
	m_currFrame = static_cast<f32>(m_pCurrAnim->startFrame+frameOffset%numFrames);
}

void AnimationPlayer::SetSpeed(f32 speed)
{
	m_playSpeed = speed;
}


bool AnimationPlayer::GetAnimIsDone()
{
	return m_animIsDone;
}

s32 AnimationPlayer::GetCurrentAnimationID()
{
	return m_pCurrAnim->animID;
}

f32 AnimationPlayer::GetCurrentFrame()
{
	return m_currFrame;
}


void AnimationPlayer::Update(f32 timeElapsed)
{
	if(m_animIsDone)
	{
		return;
	}
	
	if(m_pCurrAnim == NULL)
	{
		return;
	}
	
	m_currFrame += timeElapsed*(f32)m_pCurrAnim->FPS*m_playSpeed;
	
	if(m_currFrame >= (m_pCurrAnim->endFrame+1))
	{
		if(m_pCurrAnim->isLooping)
		{
			m_currFrame -= (m_pCurrAnim->endFrame-m_pCurrAnim->startFrame+1);
		}
		else if(m_pCurrAnim->animID_playWhenFinished != -1)
		{
			PlayAnimation(m_pCurrAnim->animID_playWhenFinished,0,m_playSpeed);
		}
		else
		{
			m_animIsDone = true;
		}
	}
}


void AnimationPlayer::Init(AnimationSet* pAnimSet)
{
	m_pAnimSet = pAnimSet;
	m_pCurrAnim = NULL;
	m_currFrame = 0.0f;
	m_animIsDone = false;
	m_playSpeed = 1.0f;
}


