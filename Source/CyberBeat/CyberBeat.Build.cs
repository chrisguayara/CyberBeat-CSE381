// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CyberBeat : ModuleRules
{
	public CyberBeat(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"NavigationSystem",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"Niagara",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"CyberBeat",
			"CyberBeat/Variant_Strategy",
			"CyberBeat/Variant_Strategy/UI",
			"CyberBeat/Variant_TwinStick",
			"CyberBeat/Variant_TwinStick/AI",
			"CyberBeat/Variant_TwinStick/Gameplay",
			"CyberBeat/Variant_TwinStick/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
