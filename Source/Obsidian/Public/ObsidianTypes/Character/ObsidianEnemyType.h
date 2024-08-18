#pragma once

UENUM(BlueprintType)
enum class EObsidianEnemyClass : uint8
{
    EEC_Zombie UMETA(DisplayName = "Zombie"),
    EEC_Goblin UMETA(DisplayName = "Goblin"),
    EEC_KurukTheTreeOrc UMETA(DisplayName = "Kuruk The Tree Orc"),

    EEC_MAX UMETA(DisplayName = "Default MAX")
	
};
