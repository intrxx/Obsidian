// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

/**
 * Obsidian Trace Channels.
 */
#define Obsidian_TraceChannel_PlayerCursorTrace	ECC_GameTraceChannel1
#define Obsidian_TraceChannel_Melee ECC_GameTraceChannel2
#define Obsidian_TraceChannel_Test ECC_GameTraceChannel3

/**
 * Obsidian Object Channels.
 */
#define Obsidian_ObjectChannel_Projectile ECC_GameTraceChannel2

/**
 * Stencil Values used for highlight colors.
 */
namespace ObsidianHighlight
{
	static constexpr int32 Red = 250;
	static constexpr int32 Green = 251;
	static constexpr int32 White = 252;
}

/**
 * Collection of FNames Actor tags stored for convenience and typo avoidance.
 */
namespace ObsidianActorTags
{
	static inline FName Player = FName("Player");
	static inline FName Enemy = FName("Enemy");

	static inline FName RegularEnemy = FName("RegularEnemy");
	static inline FName BossEnemy = FName("BossEnemy");

	static inline FName ViewTarget = FName("ViewTarget");
	static inline FName CharacterCreationViewTarget = FName("CharacterCreationViewTarget");
	static inline FName DefaultMainMenuViewTarget = FName("DefaultMainMenuViewTarget");
}

/**
 * Collection of FNames socket names stored for convenience and typo avoidance.
 */
namespace ObsidianAbilitySocketNames
{
	static inline FName Weapon = FName("WeaponAbilitySocket");
	static inline FName RightHand = FName("RightHandAbilitySocket");
	static inline FName LeftHand = FName("LeftHandAbilitySocket");
}

/**
 * Collection of FNames socket names stored for convenience and typo avoidance.
 */
namespace ObsidianMeshSocketNames
{
	static inline FName RightHandWeaponSocket = FName("EquipmentRightHandSocket");
	static inline FName LeftHandWeaponSocket = FName("EquipmentLeftHandSocket");
	static inline FName BackWeaponSocket = FName("EquipmentBackSocket");
}

/**
 * Enum that holds enemy classes.
 */
UENUM(BlueprintType)
enum class EObsidianEnemyClass : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	
	Zombie UMETA(DisplayName = "Zombie"),
	Goblin UMETA(DisplayName = "Goblin"),
	KurukTheTreeOrc UMETA(DisplayName = "Kuruk The Tree Orc"),
};

/**
 * Enum that holds socket spawn locations for abilities.
 */
UENUM(BlueprintType)
enum class EObsidianAbilitySpawnLocation : uint8
{
	DefaultLocation = 0 UMETA(DisplayName = "Default Location"),
	
	LeftHand UMETA(DisplayName = "Left Hand"),
	RightHand UMETA(DisplayName = "Right Hand"),
	LeftHandEquipment UMETA(DisplayName = "Left Hand Equipment"),
	RightHandEquipment UMETA(DisplayName = "Right Hand Equipment"),

	MAX UMETA(DisplayName = "Default MAX") 
};

/**
 * Enum that holds trace types used for advanced combat component.
 */
UENUM(BlueprintType)
enum class EObsidianTraceType  : uint8
{
	SimpleLineTrace = 0 UMETA(DisplayName = "Simple Line Trace"),
	SemiComplexLineTrace UMETA(DisplayName = "Semi Complex Line Trace"),
	ComplexLineTrace UMETA(DisplayName = "Complex Line Trace"),
	SimpleBoxTrace UMETA(DisplayName = "Simple Box Trace"),
	SimpleCapsuleTrace UMETA(DisplayName = "Simple Capsule Trace")
};

/**
 * Enum that holds types of traced mesh e.g. Actor Mesh, Left Hand Weapon Mesh, Right Hand Weapon Mesh.
 */
UENUM(BlueprintType)
enum class EObsidianTracedMeshType  : uint8
{
	CharacterMesh = 0 UMETA(DisplayName = "Character Mesh"),
	CharacterMesh_LeftHand UMETA(DisplayName = "Character Mesh - Left Hand"),
	CharacterMesh_RightHand UMETA(DisplayName = "Character Mesh - Right Hand"),
	RightHandWeaponMesh UMETA(DisplayName = "Right Hand Weapon Mesh"),
	LeftHandWeaponMesh UMETA(DisplayName = "Left Hand Weapon Mesh")
};

/**
 * Enum that holds Hero classes available in Obsidian.
 */
UENUM(BlueprintType)
enum class EObsidianHeroClass : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	
	Witch = 1 UMETA(DisplayName = "Witch"),
	Barbarian = 2 UMETA(DisplayName = "Barbarian"),
	Assassin = 3 UMETA(DisplayName = "Assassin"),
	Paladin = 4 UMETA(DisplayName = "Paladin")
};

/**
 * Enum that holds the game network type.
 */
UENUM()
enum class EObsidianGameNetworkType : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	
	OnlineCoop UMETA(DisplayName = "Online Coop"),
	
	// ~ Fill any Online Network Type here.
	
	OfflineSolo UMETA(DisplayName = "Offline Solo"),

	// ~ Fill any Offline Network Type here.
};



