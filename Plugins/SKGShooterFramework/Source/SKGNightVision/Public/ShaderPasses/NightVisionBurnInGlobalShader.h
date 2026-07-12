// Copyright 2025, Dakota Dawe, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "ShaderParameterStruct.h"
#include "GlobalShader.h"


BEGIN_SHADER_PARAMETER_STRUCT(FNightVisionBurnInComputeShaderParams,)
	SHADER_PARAMETER(FVector2f, TextureSize)
	SHADER_PARAMETER_SAMPLER(SamplerState, SceneColorSampler)
	SHADER_PARAMETER_RDG_TEXTURE(Texture2D, SceneColorTexture)
	SHADER_PARAMETER(float, bAreTubesOn)
	SHADER_PARAMETER(float, MinimumLuminanceForBurnIn)
	SHADER_PARAMETER(float, BurnInPermanentThreshold)
	SHADER_PARAMETER(float, BurnInRateValueA)
	SHADER_PARAMETER(float, BurnInRateValueB)
	SHADER_PARAMETER(float, BurnInRateValueC)
	SHADER_PARAMETER(float, BurnInScale)
	SHADER_PARAMETER(float, BurnInHealingRate)
	SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, InputTexture)
	SHADER_PARAMETER_RDG_TEXTURE_UAV(Texture2D, OutputTexture)
END_SHADER_PARAMETER_STRUCT()

class FNightVisionBurnInGlobalShader : public FGlobalShader
{
	DECLARE_EXPORTED_SHADER_TYPE(FNightVisionBurnInGlobalShader, Global, );
	using FParameters = FNightVisionBurnInComputeShaderParams;
	SHADER_USE_PARAMETER_STRUCT(FNightVisionBurnInGlobalShader, FGlobalShader);
};
