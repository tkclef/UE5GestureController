// Copyright 2025, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/SKGPDANightVisionSettings.h"
#include "Components/ActorComponent.h"
#include "SKGNightVisionComponent.generated.h"

static FName NoiseFactorParameterName {FName("NoiseFactor")};
static FName GrainMultiplierParameterName {FName("GrainMultiplier")};

static FName NightVisionColorParameterName {FName("NightVisionColor")};
static FName GainParameterName {FName("Gain")};
static FName ContrastParameterName {FName("Contrast")};
static FName NormalizedGainParameterName {FName("NormalizedGain")};

static FName DistortionRadiusParameterName {FName("DistortionRadius")};
static FName DistortionHardnessParameterName {FName("DistortionHardness")};
static FName UseRadialDistortionParameterName {FName("bUseRadialDistortion")};

static FName BurnInRenderTargetParameterName {FName("BurnInRenderTarget")};

class USKGNightVisionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTubesOnStateChanged, bool, bOn);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTubesStateChanged_Native, USKGNightVisionComponent*);

/**
 * This component is intended to be added to anything you want night vision control on such as
 * your pawn and/or optic.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKGNIGHTVISION_API USKGNightVisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USKGNightVisionComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	FOnTubesStateChanged_Native OnTubesOnStateChangedNative;
	UPROPERTY(BlueprintAssignable, Category = "SKGNightVision")
	FOnTubesOnStateChanged OnTubesOnStateChanged;
	
	UFUNCTION(BlueprintCallable, Category = "SKGNightVision")
	FNightVisionMaterialPair& CreateAndSetupDynamicNightVisionMaterialInstance(USKGPDANightVisionTubeSettings* Tube, USKGPDANightVisionHousingSettings* Housing);
	UFUNCTION(BlueprintCallable, Category = "SKGNightVision")
	const FNightVisionMaterialPair& GetNightVisionMaterialPair() const { return NightVisionMaterialPair; }

	// This just alters a boolean state for outside usage such as burn in system
	UFUNCTION(BlueprintCallable, Category = "SKGNightVision")
	void TurnTubesOn() { bTubesOn = true; OnTubesOnStateChangedNative.Broadcast(this); OnTubesOnStateChanged.Broadcast(bTubesOn); }
	// This just alters a boolean state for outside usage such as burn in system
	UFUNCTION(BlueprintCallable, Category = "SKGNightVision")
	void TurnTubesOff() { bTubesOn = false; OnTubesOnStateChangedNative.Broadcast(this); OnTubesOnStateChanged.Broadcast(bTubesOn); }
	UFUNCTION(BlueprintCallable, Category = "SKGNightVision")
	bool AreTubesOn() const { return bTubesOn; }
	
	// GainAlpha = 0-1, 0 = off, 1 = full gain
	UFUNCTION(BlueprintCallable, Category = "SKGNightVision")
	void SetGain(const float GainAlpha);
	// StepAmount is the amount to increase the gain by. Returns true if gain is max
	UFUNCTION(BlueprintCallable, Category = "SKGNightVision")
	void IncreaseGain();
	// StepAmount is the amount to increase the gain by. Returns true if gain is 0
	UFUNCTION(BlueprintCallable, Category = "SKGNightVision")
	void DecreaseGain();
	
	UFUNCTION(BlueprintPure, Category = "SKGNightVision")
	USKGPDANightVisionTubeSettings* GetTubeSettings() const { return TubeSettings; }
	UFUNCTION(BlueprintPure, Category = "SKGNightVision")
	USKGPDANightVisionHousingSettings* GetHousingSettings() const { return HousingSettings; }
	UFUNCTION(BlueprintPure, Category = "SKGNightVision")
	UTextureRenderTarget2D* GetBurnInRenderTarget() const { return BurnInRenderTarget; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision")
	TObjectPtr<USKGPDANightVisionTubeSettings> TubeSettings;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision")
	TObjectPtr<USKGPDANightVisionHousingSettings> HousingSettings;
	UPROPERTY(VisibleAnywhere, Category = "SKGNightVision")
	TObjectPtr<UTextureRenderTarget2D> BurnInRenderTarget;
	UPROPERTY()
	FNightVisionMaterialPair NightVisionMaterialPair;

	bool bTubesOn {false};

	float CurrentGainLerpValue {0.0f};
	float TargetGainLerpValue {0.0f};
	FTimerHandle TSmoothGainAdjustHandle;
	float PreviousSmoothGainAdjustTimestamp {0.0f};

	float GetGainValue() const;
	void SetGainValues(const float Gain) const;
};