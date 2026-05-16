using UnrealBuildTool;
using System.Collections.Generic;

public class ShadowTownEditorTarget : TargetRules
{
	public ShadowTownEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
		ExtraModuleNames.Add("ShadowTown");
	}
}
