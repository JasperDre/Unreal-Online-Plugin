// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class UOnlineProject : ModuleRules
{
	public UOnlineProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem", "OnlineSubsystemUtils", "Steamworks" });
        DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
    }
}
