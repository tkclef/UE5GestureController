// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE5GestureController : ModuleRules
{
	public UE5GestureController(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"UE5GestureController",
			"UE5GestureController/Variant_Platforming",
			"UE5GestureController/Variant_Platforming/Animation",
			"UE5GestureController/Variant_Combat",
			"UE5GestureController/Variant_Combat/AI",
			"UE5GestureController/Variant_Combat/Animation",
			"UE5GestureController/Variant_Combat/Gameplay",
			"UE5GestureController/Variant_Combat/Interfaces",
			"UE5GestureController/Variant_Combat/UI",
			"UE5GestureController/Variant_SideScrolling",
			"UE5GestureController/Variant_SideScrolling/AI",
			"UE5GestureController/Variant_SideScrolling/Gameplay",
			"UE5GestureController/Variant_SideScrolling/Interfaces",
			"UE5GestureController/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
