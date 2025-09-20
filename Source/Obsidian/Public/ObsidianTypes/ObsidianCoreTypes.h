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
 * Enum that holds the Rarity of Objects the Player interacts with to drop items mainly enemies and stashes placed in the level.
 */
UENUM()
enum class EObsidianEntityRarity : uint8
{
	None = 0 UMETA(DisplayName = "None"),

	/** This entity is roll logic ambiguous, can be used for quest item spawners to just drop one specific item. */
	Neutral UMETA(DisplayName = "Neutral"),

	/** Normal Enemies, the default rarity of monsters and Item Spawners. */
	Normal UMETA(DisplayName = "Normal"),

	/** Primary used for Chests, slighly righer chance to get items. */
	Magic UMETA(DisplayName = "Magic"),

	/** Used for special packs of monsters with extra loot. */
	Rare UMETA(DisplayName = "Rare"),

	/** Used for companions of Mini Bosses, slightly more loot than Rare enemies. */
	MiniBossCompanion UMETA(DisplayName = "Mini Boss Companion"),

	/** Used for Mini Bosses in the level, high loot. */
	MiniBoss UMETA(DisplayName = "Mini Boss"),

	/** Used for Special Act/Area Bosses. */
	SpecialBoss UMETA(DisplayName = "Special Boss"),
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
	
	Witch UMETA(DisplayName = "Witch"),
	Barbarian UMETA(DisplayName = "Barbarian"),
	Assassin UMETA(DisplayName = "Assassin"),
	Paladin UMETA(DisplayName = "Paladin")
};



