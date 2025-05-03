// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UE_Grapple2 : ModuleRules
{
	public UE_Grapple2(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "CableComponent", "Niagara" });
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
