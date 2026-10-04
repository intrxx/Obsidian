// Copyright 2026 out of sCope team - intrxx

using System.IO;
using UnrealBuildTool;

/**
 * Automation tests of the Obsidian module, written with CQTest.
 * This is an Editor module, so neither the tests nor the test only classes end up in the game.
 */
public class ObsidianTests : ModuleRules
{
	public ObsidianTests(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"GameplayTags",
			"GameplayAbilities",
			"AIModule",
			"Obsidian",
			"CQTest"
		});

		// Obsidian headers include some of its own headers relative to the Source directory, e.g. "Obsidian/ObsidianGameplayTags.h".
		PrivateIncludePaths.Add(Path.GetFullPath(Path.Combine(ModuleDirectory, "..")));
	}
}
