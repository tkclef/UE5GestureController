// Copyright 2025, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "SceneViewExtension.h"

class USKGNightVisionComponent;
class UTextureRenderTarget2D;

class SKGNIGHTVISION_API FNightVisionSceneViewExtension : public FSceneViewExtensionBase
{
public:
	FNightVisionSceneViewExtension(const FAutoRegister& AutoRegister) : FSceneViewExtensionBase(AutoRegister) {}
	
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {}
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {}

	virtual void PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs) override;

	void RegisterNightVisionComponent(USKGNightVisionComponent* NightVisionComponent);
	void UnregisterNightVisionComponent(USKGNightVisionComponent* NightVisionComponent);
	void RefreshNVGRenderTarget() { bWantsUpdatedRenderTarget = true;}
	void ClearNVGRenderTarget() { bWantsToClearRenderTarget = true;}
	
protected:
	bool bNightVisionDeviceOn {false};
	bool bWantsUpdatedRenderTarget {false};
	bool bWantsToClearRenderTarget {false};
	float MinimumLuminanceForBurnIn {0.0f};
	float BurnInPermanentThreshold {0.0f};
	float BurnInRateValueA {1.0f};
	float BurnInRateValueB {2.0f};
	float BurnInRateValueC {0.0f};
	float BurnInScale {0.0f};
	float BurnInHealingRate {0.0f};
	
	FDelegateHandle OnTubesOnStateChangedDelegateHandle;
	TObjectPtr<UTextureRenderTarget2D> ActiveNVGRenderTarget;
	TRefCountPtr<IPooledRenderTarget> PooledRenderTarget;

	void OnNightVisionComponentStateChanged(USKGNightVisionComponent* NightVisionComponent);
	void CreatePooledRenderTargetFromActiveNVGRenderTarget_RenderThread();
};
