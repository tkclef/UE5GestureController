// Copyright 2025, Dakota Dawe, All rights reserved


#include "Components/SKGNightVisionComponent.h"

#include "ImageUtils.h"
#include "DataAssets/SKGPDANightVisionSettings.h"
#include "Subsystems/NightVisionEngineSubsystem.h"
#include "ShaderPasses/NightVisionSceneViewExtension.h"

#include "TimerManager.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MaterialTypes.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"


USKGNightVisionComponent::USKGNightVisionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void USKGNightVisionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (TubeSettings && HousingSettings)
	{
		CreateAndSetupDynamicNightVisionMaterialInstance(TubeSettings, HousingSettings);
	}
}

void USKGNightVisionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (UNightVisionEngineSubsystem* NightVisionEngineSubsystem = GEngine->GetEngineSubsystem<UNightVisionEngineSubsystem>())
	{
		NightVisionEngineSubsystem->UnregisterNightVisionDeviceAsActiveForBurnIn(this);
	}
	BurnInRenderTarget = nullptr;
}

float USKGNightVisionComponent::GetGainValue() const
{
	FMaterialParameterInfo GainParameterInfo(GainParameterName);
	float Gain;
	NightVisionMaterialPair.NightVisionMaterialInstance->GetScalarParameterValue(GainParameterInfo, Gain);
	return Gain;
}

void USKGNightVisionComponent::SetGainValues(const float Gain) const
{
	NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(GainParameterName, Gain);
	NightVisionMaterialPair.NightVisionScintillationMaterialInstance->SetScalarParameterValue(GainParameterName, Gain);
}

FNightVisionMaterialPair& USKGNightVisionComponent::CreateAndSetupDynamicNightVisionMaterialInstance(USKGPDANightVisionTubeSettings* Tube, USKGPDANightVisionHousingSettings* Housing)
{
	TubeSettings = Tube;
	HousingSettings = Housing;
	bTubesOn = TubeSettings->bTubesOnByDefault;
	
	NightVisionMaterialPair.NightVisionMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), TubeSettings->NightVisionMaterial);
	NightVisionMaterialPair.NightVisionScintillationMaterialInstance = UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), TubeSettings->NightVisionScintillationMaterial);
	
	if (TubeSettings->RenderTargetSize > 0)
	{
		BurnInRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, TubeSettings->RenderTargetSize, TubeSettings->RenderTargetSize, ETextureRenderTargetFormat::RTF_R16f);
	}

	CurrentGainLerpValue = TubeSettings->Gain / TubeSettings->MaxGain;
	TargetGainLerpValue = CurrentGainLerpValue;
	
	NightVisionMaterialPair.NightVisionMaterialInstance->SetVectorParameterValue(NightVisionColorParameterName, TubeSettings->Color);
	NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(GainParameterName, TubeSettings->Gain);
	NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(NormalizedGainParameterName, CurrentGainLerpValue);
	NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(ContrastParameterName, TubeSettings->Contrast);
	
	NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(NoiseFactorParameterName, TubeSettings->NoiseFactor);
	NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(GrainMultiplierParameterName, TubeSettings->GrainMultiplier);
	
	NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(DistortionRadiusParameterName, HousingSettings->DistortionRadius);
	NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(DistortionHardnessParameterName, HousingSettings->DistortionHardness);
	NightVisionMaterialPair.NightVisionMaterialInstance->SetScalarParameterValue(UseRadialDistortionParameterName, HousingSettings->bUseRadialDistortion);

	NightVisionMaterialPair.NightVisionMaterialInstance->SetTextureParameterValue(BurnInRenderTargetParameterName, BurnInRenderTarget);

	NightVisionMaterialPair.NightVisionScintillationMaterialInstance->SetScalarParameterValue(GainParameterName, TubeSettings->Gain);
	NightVisionMaterialPair.NightVisionScintillationMaterialInstance->SetScalarParameterValue(NormalizedGainParameterName, CurrentGainLerpValue);
	return NightVisionMaterialPair;
}

void USKGNightVisionComponent::SetGain(const float Gain)
{
	if (HousingSettings->bManualGain)
	{
		TargetGainLerpValue = FMath::Clamp(Gain, HousingSettings->MinimumGain, 1.0f);
		if (HousingSettings->bSmoothAdjustGain)
		{
			PreviousSmoothGainAdjustTimestamp = GetWorld()->GetTimeSeconds();
			GetWorld()->GetTimerManager().SetTimer(TSmoothGainAdjustHandle, [this]()
			{
				CurrentGainLerpValue = FMath::FInterpConstantTo(CurrentGainLerpValue, TargetGainLerpValue, GetWorld()->GetTimeSeconds() - PreviousSmoothGainAdjustTimestamp, HousingSettings->SmoothAdjustSpeed);
				PreviousSmoothGainAdjustTimestamp = GetWorld()->GetTimeSeconds();
				const float NewGain = FMath::Lerp(HousingSettings->MinimumGain, TubeSettings->MaxGain, CurrentGainLerpValue);
				if (!FMath::IsNearlyEqual(NewGain, GetGainValue()))
				{
					SetGainValues(NewGain);
				}
				else
				{
					GetWorld()->GetTimerManager().ClearTimer(TSmoothGainAdjustHandle);
				}
			}, 1.0f / 60.0f, true);
		}
		else
		{
			CurrentGainLerpValue = TargetGainLerpValue;
			const float NewGain = FMath::Lerp(0.0f, TubeSettings->MaxGain, CurrentGainLerpValue);
			if (!FMath::IsNearlyEqual(NewGain, GetGainValue()))
			{
				SetGainValues(NewGain);
			}
		}

		OnTubesOnStateChangedNative.Broadcast(this);
	}
}

void USKGNightVisionComponent::IncreaseGain()
{
	SetGain(TargetGainLerpValue + HousingSettings->GainAdjustmentStepAmount);
}

void USKGNightVisionComponent::DecreaseGain()
{
	SetGain(TargetGainLerpValue - HousingSettings->GainAdjustmentStepAmount);
}
