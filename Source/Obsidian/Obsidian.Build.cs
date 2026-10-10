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
				"GameplayDebugger",
				"ModularGameplayActors", 
				"CommonGame",
				"Niagara",
				"GameplayCameras", 
				"MassActors"
			});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore", 
			"AIModule",
			"NavigationSystem",
			"NetCore",
			"GameplayMessageRuntime",
			"CommonUser",
			"AsyncMixin"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

		SetupGameplayDebuggerSupport(Target);
		SetupDebugMenuSupport(Target);
	}

	/** SlateIM Debug Menu (obsidian.ToggleDebugMenu), lives in Private/Debug and is compiled out of Shipping builds. */
	private void SetupDebugMenuSupport(ReadOnlyTargetRules Target)
	{
		if (Target.Configuration != UnrealTargetConfiguration.Shipping)
		{
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"SlateIM",
				"AssetRegistry"
			});

			PrivateDefinitions.Add("WITH_OBSIDIAN_DEBUG_MENU=1");
		}
		else
		{
			PrivateDefinitions.Add("WITH_OBSIDIAN_DEBUG_MENU=0");
		}
	}
}
