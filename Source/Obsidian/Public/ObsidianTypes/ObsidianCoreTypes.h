// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

/** Obsidian Trace Channels. */
#define Obsidian_TraceChannel_PlayerCursorTrace	ECC_GameTraceChannel1
#define Obsidian_TraceChannel_Melee ECC_GameTraceChannel2

/** Obsidian Object Channels. */
#define Obsidian_ObjectChannel_Projectile ECC_GameTraceChannel2

/** Stencil Values used for highlight colors. */
namespace ObsidianHighlight
{
	static constexpr int32 Red = 250;
	static constexpr int32 Green = 251;
	static constexpr int32 White = 252;
}

/** Collection of FNames Actor tags stored for convenience and typo avoidance. */
namespace ObsidianActorTags
{
	static inline FName Player = FName("Player");
	static inline FName Enemy = FName("Enemy");

	static inline FName RegularEnemy = FName("RegularEnemy");
	static inline FName BossEnemy = FName("BossEnemy");
}

/** Collection of FNames socket names stored for convenience and typo avoidance. */
namespace ObsidianAbilitySocketNames
{
	static inline FName Weapon = FName("WeaponAbilitySocket");
	static inline FName RightHand = FName("RightHandAbilitySocket");
	static inline FName LeftHand = FName("LeftHandAbilitySocket");
}

/** Collection of FNames socket names stored for convenience and typo avoidance. */
namespace ObsidianMeshSocketNames
{
	static inline FName RightHandWeaponSocket = FName("EquipmentRightHandSocket");
	static inline FName LeftHandWeaponSocket = FName("EquipmentLeftHandSocket");
	static inline FName BackWeaponSocket = FName("EquipmentBackSocket");
}

/** Enum that holds enemy classes. */
UENUM(BlueprintType)
enum class EObsidianEnemyClass : uint8
{
	EEC_Zombie UMETA(DisplayName = "Zombie"),
	EEC_Goblin UMETA(DisplayName = "Goblin"),
	EEC_KurukTheTreeOrc UMETA(DisplayName = "Kuruk The Tree Orc"),

	EEC_MAX UMETA(DisplayName = "Default MAX")
	
};

/** Enum that holds socket spawn locations for abilities. */
UENUM(BlueprintType)
enum class EObsidianAbilitySpawnLocation : uint8
{
	ASL_DefaultLocation = 0 UMETA(DisplayName = "Default Location"),
	ASL_LeftHand UMETA(DisplayName = "Left Hand"),
	ASL_RightHand UMETA(DisplayName = "Right Hand"),
	ASL_LeftHandEquipment UMETA(DisplayName = "Left Hand Equipment"),
	ASL_RightHandEquipment UMETA(DisplayName = "Right Hand Equipment"),

	ASL_MAX UMETA(DisplayName = "Default MAX") 
};

/** Enum that holds trace types used for advanced combat component. */
UENUM(BlueprintType)
enum class EObsidianTraceType  : uint8
{
	ETT_SimpleLineTrace = 0 UMETA(DisplayName = "Simple Line Trace"),
	ETT_ComplexLineTrace UMETA(DisplayName = "Complex Line Trace"),
	ETT_SimpleBoxTrace UMETA(DisplayName = "Simple Box Trace"),
	ETT_ComplexBoxTrace UMETA(DisplayName = "Complex Box Trace"),
	ETT_SimpleCapsuleTrace UMETA(DisplayName = "Simple Capsule Trace")
};

/** Enum that holds types of traced mesh e.g. Actor Mesh, Left Hand Weapon Mesh, Right Hand Weapon Mesh. */
UENUM(BlueprintType)
enum class EObsidianTracedMeshType  : uint8
{
	ETMT_CharacterMesh = 0 UMETA(DisplayName = "Character Mesh"),
	ETMT_CharacterMesh_LeftHand UMETA(DisplayName = "Character Mesh - Left Hand"),
	ETMT_CharacterMesh_RightHand UMETA(DisplayName = "Character Mesh - Right Hand"),
	ETMT_RightHandWeaponMesh UMETA(DisplayName = "Right Hand Weapon Mesh"),
	ETMT_LeftHandWeaponMesh UMETA(DisplayName = "Left Hand Weapon Mesh")
};