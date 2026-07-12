// Copyright 2025, Dakota Dawe, All rights reserved


#include "Subsystems/NightVisionEngineSubsystem.h"
#include "ShaderPasses/NightVisionSceneViewExtension.h"

#include "SceneViewExtension.h"

void UNightVisionEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	NightVisionSceneViewExtension = FSceneViewExtensions::NewExtension<FNightVisionSceneViewExtension>();
}

void UNightVisionEngineSubsystem::Deinitialize()
{
	Super::Deinitialize();

	NightVisionSceneViewExtension.Reset();
	NightVisionSceneViewExtension = nullptr;
}

void UNightVisionEngineSubsystem::RegisterNightVisionDeviceAsActiveForBurnIn(USKGNightVisionComponent* NightVisionComponent)
{
	GetNightVisionSceneViewExtension()->RegisterNightVisionComponent(NightVisionComponent);
}

void UNightVisionEngineSubsystem::UnregisterNightVisionDeviceAsActiveForBurnIn(USKGNightVisionComponent* NightVisionComponent)
{
	GetNightVisionSceneViewExtension()->UnregisterNightVisionComponent(NightVisionComponent);
}

FNightVisionSceneViewExtension* UNightVisionEngineSubsystem::GetNightVisionSceneViewExtension() const
{
	return NightVisionSceneViewExtension.Get();
}
