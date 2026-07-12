// Copyright 2025, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "NightVisionEngineSubsystem.generated.h"

class FNightVisionSceneViewExtension;
class USKGNightVisionComponent;

UCLASS()
class SKGNIGHTVISION_API UNightVisionEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "SKGNightVision")
	void RegisterNightVisionDeviceAsActiveForBurnIn(USKGNightVisionComponent* NightVisionComponent);
	UFUNCTION(BlueprintCallable, Category = "SKGNightVision")
	void UnregisterNightVisionDeviceAsActiveForBurnIn(USKGNightVisionComponent* NightVisionComponent);
	
	FNightVisionSceneViewExtension* GetNightVisionSceneViewExtension() const;
	
protected:
	TSharedPtr<FNightVisionSceneViewExtension, ESPMode::ThreadSafe> NightVisionSceneViewExtension;
};
