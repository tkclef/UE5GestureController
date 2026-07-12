// Copyright 2025, Dakota Dawe, All rights reserved


#include "ShaderPasses/NightVisionSceneViewExtension.h"
#include "ShaderPasses/NightVisionBurnInGlobalShader.h"
#include "Components/SKGNightVisionComponent.h"

#include "RenderTargetPool.h"
#include "ScreenPass.h"
#include "SceneRendering.h"
#include "Engine/TextureRenderTarget2D.h"
#include "PostProcess/PostProcessInputs.h"
#include "PixelShaderUtils.h"
#include "TextureResource.h"
#include "GlobalShader.h"


void FNightVisionSceneViewExtension::RegisterNightVisionComponent(USKGNightVisionComponent* NightVisionComponent)
{
	if (ensureAlwaysMsgf(NightVisionComponent, TEXT("RegisterNightVisionComponent NightVisionComponent INVALID")))
	{
		if (ensureAlwaysMsgf(NightVisionComponent->GetBurnInRenderTarget(), TEXT("RegisterNightVisionComponent BurnInRenderTarget INVALID on NightVisionComponent of Actor: %s"), *NightVisionComponent->GetName()))
		{
			ActiveNVGRenderTarget = NightVisionComponent->GetBurnInRenderTarget();
			bNightVisionDeviceOn = NightVisionComponent->AreTubesOn();
			
			OnTubesOnStateChangedDelegateHandle.Reset();
			OnTubesOnStateChangedDelegateHandle = NightVisionComponent->OnTubesOnStateChangedNative.AddRaw(this, &FNightVisionSceneViewExtension::OnNightVisionComponentStateChanged);
			
			bWantsUpdatedRenderTarget = true;
		}
	}
}

void FNightVisionSceneViewExtension::UnregisterNightVisionComponent(USKGNightVisionComponent* NightVisionComponent)
{
	if (ActiveNVGRenderTarget && ensureAlwaysMsgf(NightVisionComponent, TEXT("UnregisterNightVisionComponent NightVisionComponent INVALID")))
	{
		if (ensureAlwaysMsgf(NightVisionComponent->GetBurnInRenderTarget(), TEXT("RegisterNightVisionComponent BurnInRenderTarget INVALID on NightVisionComponent of Actor: %s"), *NightVisionComponent->GetName()))
		{
			if (NightVisionComponent->GetBurnInRenderTarget() == ActiveNVGRenderTarget)
			{
				OnTubesOnStateChangedDelegateHandle.Reset();
				bWantsToClearRenderTarget = true;
			}
		}
	}
}

void FNightVisionSceneViewExtension::OnNightVisionComponentStateChanged(USKGNightVisionComponent* NightVisionComponent)
{
	if (ensureAlwaysMsgf(NightVisionComponent, TEXT("OnNightVisionComponentStateChanged NightVisionComponent INVALID")))
	{
		bNightVisionDeviceOn = NightVisionComponent->AreTubesOn();
		MinimumLuminanceForBurnIn = NightVisionComponent->GetTubeSettings()->MinimumLuminanceForBurnIn;
		BurnInPermanentThreshold = NightVisionComponent->GetTubeSettings()->BurnInPermanentThreshold;
		BurnInRateValueA = NightVisionComponent->GetTubeSettings()->BurnInRateValueA;
		BurnInRateValueB = NightVisionComponent->GetTubeSettings()->BurnInRateValueB;
		BurnInRateValueC = NightVisionComponent->GetTubeSettings()->BurnInRateValueC;
		BurnInScale = NightVisionComponent->GetTubeSettings()->BurnInScale * 0.5f;
		BurnInHealingRate = NightVisionComponent->GetTubeSettings()->BurnInHealingRate * 0.5f;
	}
}

void FNightVisionSceneViewExtension::CreatePooledRenderTargetFromActiveNVGRenderTarget_RenderThread()
{
	checkf(IsInRenderingThread() || IsInRHIThread(), TEXT("Cannot create Pooled Render Target from outside the rendering thread"));
	if (ActiveNVGRenderTarget)
	{
		// Render target resources require the render thread
		if (const FTextureRenderTargetResource* RenderTargetResource = ActiveNVGRenderTarget->GetRenderTargetResource())
		{
			const FTextureRHIRef RenderTargetRHI = RenderTargetResource->GetRenderTargetTexture();

			FSceneRenderTargetItem RenderTargetItem;
			RenderTargetItem.TargetableTexture = RenderTargetRHI;
			RenderTargetItem.ShaderResourceTexture = RenderTargetRHI;

			// Flags allow it to be used as a render target, shader resource, UAV 
			FPooledRenderTargetDesc RenderTargetDesc = FPooledRenderTargetDesc::Create2DDesc(RenderTargetResource->GetSizeXY(), RenderTargetRHI->GetDesc().Format, FClearValueBinding::Black, TexCreate_RenderTargetable | TexCreate_ShaderResource | TexCreate_UAV, TexCreate_None, false);
	
			GRenderTargetPool.CreateUntrackedElement(RenderTargetDesc, PooledRenderTarget, RenderTargetItem);
			
			//UE_LOG(LogTemp, Warning, TEXT("NEW POOLED RT: %s"), *PooledRenderTarget.GetReference()->GetDesc().Extent.ToString());
			bWantsUpdatedRenderTarget = false;
		}
	}
}

void FNightVisionSceneViewExtension::PrePostProcessPass_RenderThread(FRDGBuilder& GraphBuilder, const FSceneView& View, const FPostProcessingInputs& Inputs)
{
	if (bWantsUpdatedRenderTarget)
	{
		// Setup way to update this, such as when ActiveNVGRenderTarget is swapped or has its resolution changed
		CreatePooledRenderTargetFromActiveNVGRenderTarget_RenderThread();
	}
	if (ActiveNVGRenderTarget)
	{
		checkSlow(View.bIsViewInfo);
		const FIntRect Viewport = static_cast<const FViewInfo&>(View).ViewRect;

		const FScreenPassTexture SceneColourTexture((*Inputs.SceneTextures)->SceneColorTexture, Viewport);

		FRDGTextureRef RenderTargetTexture = GraphBuilder.RegisterExternalTexture(PooledRenderTarget, TEXT("Bound Render Target"));
		// Since we're rendering to the render target, we're going to use the full size of the render target rather than the screen
		const FIntRect RenderViewport = FIntRect(0, 0, RenderTargetTexture->Desc.Extent.X, RenderTargetTexture->Desc.Extent.Y);
		FRDGTextureRef TempTexture = GraphBuilder.CreateTexture(RenderTargetTexture->Desc, TEXT("Temp Texture"));
		FRDGTextureUAVDesc TempUAVDesc = FRDGTextureUAVDesc(TempTexture);
		FRDGTextureUAVRef TempUAV = GraphBuilder.CreateUAV(TempUAVDesc);
		FRDGTextureSRVDesc TempUAVDesc2 = FRDGTextureSRVDesc(RenderTargetTexture);
		FRDGTextureSRVRef TempUAV2 = GraphBuilder.CreateSRV(TempUAVDesc2);
	
		const FIntPoint ThreadCount = RenderViewport.Size();
		const FIntVector GroupCount = FComputeShaderUtils::GetGroupCount(ThreadCount, FIntPoint(32, 32));
	
		FNightVisionBurnInGlobalShader::FParameters* Parameters = GraphBuilder.AllocParameters<FNightVisionBurnInGlobalShader::FParameters>();
		Parameters->TextureSize = RenderTargetTexture->Desc.Extent;
		Parameters->SceneColorSampler = TStaticSamplerState<SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
		Parameters->SceneColorTexture = SceneColourTexture.Texture;
		Parameters->bAreTubesOn = bNightVisionDeviceOn;
		Parameters->MinimumLuminanceForBurnIn = MinimumLuminanceForBurnIn;
		Parameters->BurnInPermanentThreshold = BurnInPermanentThreshold;
		Parameters->BurnInRateValueA = BurnInRateValueA;
		Parameters->BurnInRateValueB = BurnInRateValueB;
		Parameters->BurnInRateValueC = BurnInRateValueC;
		Parameters->BurnInScale = BurnInScale;
		Parameters->BurnInHealingRate = BurnInHealingRate;
		Parameters->InputTexture = TempUAV2;
		Parameters->OutputTexture = TempUAV;

		const FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
		TShaderMapRef<FNightVisionBurnInGlobalShader> ComputeShader(GlobalShaderMap);
		FComputeShaderUtils::AddPass(GraphBuilder, FRDGEventName(TEXT("Night Vision Burn In Compute Shader Pass")), ERDGPassFlags::Compute, ComputeShader, Parameters, GroupCount);
		AddCopyTexturePass(GraphBuilder, TempTexture, RenderTargetTexture);
	}
	if (bWantsToClearRenderTarget)
	{
		bWantsToClearRenderTarget = false;
		ActiveNVGRenderTarget = nullptr;
	}
}