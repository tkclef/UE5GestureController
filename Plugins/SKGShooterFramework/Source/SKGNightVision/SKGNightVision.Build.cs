using System.IO;
using UnrealBuildTool;

public class SKGNightVision : ModuleRules
{
    public SKGNightVision(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        DefaultBuildSettings = BuildSettingsVersion.Latest;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        IWYUSupport = IWYUSupport.Full;

        PublicIncludePaths.AddRange( new string[]
            {
                Path.Combine(GetModuleDirectory("Renderer"), "Internal"),
                Path.Combine(GetModuleDirectory("Renderer"), "Private"),
            }
        );
        
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "Engine"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Projects",
                "RHI",
                "Renderer",
                "RenderCore"
            }
        );
    }
}