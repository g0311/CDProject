// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CDServer : ModuleRules
{
	public CDServer(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"GameLiftServerSDK",
			"GameplayTags",
			"HTTP"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore"
		});
	}
}
