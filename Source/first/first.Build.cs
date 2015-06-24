// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class first : ModuleRules
{
	public first(TargetInfo Target)
	{
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore"});

        PrivateDependencyModuleNames.AddRange(new string[] { "CustomMeshComponent" });
        PrivateIncludePathModuleNames.AddRange(new string[] { "CustomMeshComponent" });

        PublicIncludePaths.AddRange(new string[] { "CustomMeshComponent/Public", "CustomMeshComponent/Classes", "CustomMeshComponent/Private" });

		//PrivateDependencyModuleNames.AddRange(new string[] {  });

     //   PrivateDependencyModuleNames.AddRange(new string[] { "CustomMeshComponent" });
     //   PrivateIncludePathModuleNames.AddRange(new string[] { "CustomMeshComponent" });


        // Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		// if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
		// {
		//		if (UEBuildConfiguration.bCompileSteamOSS == true)
		//		{
		//			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		//		}
		// }
	}
}
