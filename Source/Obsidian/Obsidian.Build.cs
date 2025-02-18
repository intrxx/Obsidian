// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Obsidian : ModuleRules
{
	public Obsidian(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] 
			{ 
				"Core", 
				"CoreUObject", 
				"Engine", 
				"InputCore", 
				"ModularGameplay",
				"GameFeatures",
				"EnhancedInput",
				"GameplayTags",
				"GameplayAbilities",
				"GameplayTasks",
				"CommonUI",
				"MotionWarping",
				"AIModule",
				"GameplayDebugger"
			});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore", 
			"AIModule",
			"NavigationSystem",
			"Niagara",
			"AIModule",
			"NetCore",
			"GameplayMessageRuntime"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
		
		SetupGameplayDebuggerSupport(Target);
	}
}
