// Copyright 2025, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SKGPDANightVisionSettings.generated.h"

class UMaterialInterface;
class UMaterialInstanceDynamic;

USTRUCT(BlueprintType)
struct FNightVisionMaterialPair
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision")
	TObjectPtr<UMaterialInstanceDynamic> NightVisionMaterialInstance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision")
	TObjectPtr<UMaterialInstanceDynamic> NightVisionScintillationMaterialInstance;
};


UCLASS()
class SKGNIGHTVISION_API USKGPDANightVisionTubeSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Material")
	TObjectPtr<UMaterialInterface> NightVisionMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Material")
	TObjectPtr<UMaterialInterface> NightVisionScintillationMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|NightVision")
	bool bTubesOnByDefault {true};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|NightVision")
	FLinearColor Color {FLinearColor(0.4f, 0.88f, 1.0f, 1.0f)};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|NightVision")
	float Gain {100.0f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|NightVision")
	float Contrast {1.0f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|NightVision")
	float MaxGain {1000.0f};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Grain")
	float NoiseFactor {0.992};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Grain")
	float GrainMultiplier {0.5};
	
	// Leave value at 0 if you don't want a render target created for burn in. Size of 1024-2048 is recommended
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|BurnIn")
	int32 RenderTargetSize {0};
	// Luminance in the scene beyond this threshold will start to cause streaks and eventually burn ins on the tubes
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|BurnIn")
	float MinimumLuminanceForBurnIn {8.0f};
	// A burn in equal or beyond this threshold (solid black) will leave permanent damage that will not recover. Set to > 1 for no permanent damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|BurnIn")
	float BurnInPermanentThreshold {1.0f};
	// BurnInRateValue above BurnInPermanentThreshold = A(POW(B, X) + C
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|BurnIn")
	float BurnInRateValueA {1.0f};
	// BurnInRateValue above BurnInPermanentThreshold = A(POW(B, X) + C
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|BurnIn")
	float BurnInRateValueB {1.5f};
	// BurnInRateValue above BurnInPermanentThreshold = A(POW(B, X) + C
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|BurnIn")
	float BurnInRateValueC {1.0f};
	// The lower the value the slower the burn in for luminance beyond the threshold
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|BurnIn")
	float BurnInScale {0.0001f};
	// The higher the value the faster streaking will be healed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|BurnIn")
	float BurnInHealingRate {0.0001f};
};

UCLASS()
class SKGNIGHTVISION_API USKGPDANightVisionHousingSettings : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Housing")
	bool bManualGain {true};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Housing", meta = (EditCondition = "bManualGain"))
	bool bSmoothAdjustGain {false};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Housing", meta = (EditCondition = "bManualGain && bSmoothAdjustGain"))
	float SmoothAdjustSpeed {1.0f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Housing", meta = (EditCondition = "bManualGain"))
	float GainAdjustmentStepAmount {0.1f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Housing", meta = (EditCondition = "bManualGain"))
	float MinimumGain {0.1f};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Lens")
	float DistortionRadius {0.88f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Lens")
	float DistortionHardness {4.0f};
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SKGNightVision|Lens")
	bool bUseRadialDistortion {true};
};