// Copyright 2026 out of sCope team - intrxx

using UnrealBuildTool;

public class TinyPOI : ModuleRules
{
	public TinyPOI(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"GameplayTags"
		});
	}
}
