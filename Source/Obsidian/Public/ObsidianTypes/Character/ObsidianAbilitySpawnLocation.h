#pragma once

UENUM(BlueprintType)
enum class EObsidianAbilitySpawnLocation : uint8
{
	ASL_DefaultLocation UMETA(DisplayName = "Default Location"),
	ASL_LeftHand UMETA(DisplayName = "Left Hand"),
	ASL_RightHand UMETA(DisplayName = "Right Hand"),
	ASL_LeftHandEquipment UMETA(DisplayName = "Left Hand Equipment"),
	ASL_RightHandEquipment UMETA(DisplayName = "Right Hand Equipment"),

	ASL_MAX UMETA(DisplayName = "Default MAX") 
};
