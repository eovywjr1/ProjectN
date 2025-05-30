// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectN : ModuleRules
{
	public ProjectN(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
			{ "Core", "CoreUObject", "Engine", "InputCore", "NetCore", "HeadMountedDisplay", "EnhancedInput", "ModularGameplay", "AIModule" });

		PrivateIncludePaths.AddRange(new string[] { "ProjectN" });
		
		PrivateDependencyModuleNames.AddRange(new string[] {
			"GameFeatures",
			"GameplayAbilities",
			"GameplayTasks",
			"GameplayTags"
		});
	}
}