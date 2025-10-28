// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Sparta_TProject_02 : ModuleRules
{
	public Sparta_TProject_02(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "AIModule", "NavigationSystem" });
	}
}
