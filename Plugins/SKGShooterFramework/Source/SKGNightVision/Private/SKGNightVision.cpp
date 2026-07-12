#include "SKGNightVision.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FSKGNightVisionModule"

void FSKGNightVisionModule::StartupModule()
{
    const FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("SKGShooterFramework"))->GetBaseDir(), TEXT("Shaders/NightVision"));

    if (!AllShaderSourceDirectoryMappings().Contains(TEXT("/NightVisionShaders")))
    {
        AddShaderSourceDirectoryMapping(TEXT("/NightVisionShaders"), PluginShaderDir);
    }
}

void FSKGNightVisionModule::ShutdownModule()
{
    
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FSKGNightVisionModule, SKGNightVision)