// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class QuackToHell : ModuleRules
{
	public QuackToHell(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(new string[]{"QuackToHell"});

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "EnhancedInput", "GameplayTags", "HTTP", "Json", "JsonUtilities" });
		PublicDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "OnlineSubsystemSteam", "OnlineSubsystemUtils"});
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "UMG" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
