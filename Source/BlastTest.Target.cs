// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class BlastTestTarget : TargetRules
{
	public BlastTestTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		bUsesSteam = true;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("BlastTest");
		
	}
}
