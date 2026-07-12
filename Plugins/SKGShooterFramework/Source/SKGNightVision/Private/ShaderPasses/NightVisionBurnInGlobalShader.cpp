// Copyright 2025, Dakota Dawe, All rights reserved


#include "ShaderPasses/NightVisionBurnInGlobalShader.h"


IMPLEMENT_SHADER_TYPE(, FNightVisionBurnInGlobalShader, TEXT("/NightVisionShaders/private/NightVisionBurnInComputeShader.usf"), TEXT("MainCS"), SF_Compute);