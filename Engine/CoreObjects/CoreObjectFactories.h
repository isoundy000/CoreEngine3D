#ifndef __COREOBJECTFACTORIES_H
#define __COREOBJECTFACTORIES_H

#include "CoreObjects/CoreObject_Manager.h"
#include "GUI/CoreUIView.h"
#include "GUI/CoreUIImageView.h"
#include "GUI/CoreUIButton.h"
#include "Trigger_2D.h"
#include "ScriptObject.h"
#include "ObjectGroup.h"
#include "SoundPlayer.h"
#include "TileAffector.h"
#include "Effects/BasicParticle_2D.h"
#include "Effects/AnimatedParticle_2D.h"
#include "Effects/MuzzleFlareParticle_2D.h"
#include "CurvePoints_2D.h"
#include "Spawner.h"
#include "NullObject.h"

extern CoreObjectFactory<CoreUIView> g_Factory_CoreUIView;
extern CoreObjectFactory<CoreUIImageView> g_Factory_CoreUIImageView;
extern CoreObjectFactory<CoreUIButton> g_Factory_CoreUIButton;
extern CoreObjectFactory<Trigger_2D> g_Factory_Trigger_2D;
extern CoreObjectFactory<ScriptObject> g_Factory_ScriptObject;
extern CoreObjectFactory<ObjectGroup> g_Factory_ObjectGroup;
extern CoreObjectFactory<SoundPlayer> g_Factory_SoundPlayer;
extern CoreObjectFactory<TileAffector> g_Factory_TileAffector;
extern CoreObjectFactory<BasicParticle_2D> g_Factory_BasicParticle_2D;
extern CoreObjectFactory<AnimatedParticle_2D> g_Factory_AnimatedParticle_2D;
extern CoreObjectFactory<MuzzleFlareParticle_2D> g_Factory_MuzzleFlareParticle_2D;
extern CoreObjectFactory<CurvePoints_2D> g_Factory_CurvePoints_2D;
extern CoreObjectFactory<RenderableGeometry3D> g_Factory_Geometry_Normal;
extern CoreObjectFactory<RenderableGeometry3D> g_Factory_Geometry_UI;
extern CoreObjectFactory<RenderableGeometry3D> g_Factory_Geometry_Light;
extern CoreObjectFactory<RenderableSceneObject3D> g_Factory_RenderableSceneObject;
extern CoreObjectFactory<Spawner> g_Factory_Spawner;
extern CoreObjectFactory<NullObject> g_Factory_NullObject;
#endif
