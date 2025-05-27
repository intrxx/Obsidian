// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#if WITH_GAMEPLAY_DEBUGGER_MENU
#include "GameplayDebuggerCategory.h"
#include "GameplayTagContainer.h"

// ~ Project

class UObsidianAbilitySet;
class AObsidianSpawnedEquipmentPiece;

/**
 * 
 */
class FGameplayDebuggerCategory_Equipment : public FGameplayDebuggerCategory
{

public:
	OBSIDIAN_API FGameplayDebuggerCategory_Equipment();

	OBSIDIAN_API virtual void CollectData(APlayerController* OwnerPC, AActor* DebugActor) override;
	OBSIDIAN_API virtual void DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext) override;

	OBSIDIAN_API static TSharedRef<FGameplayDebuggerCategory> MakeInstance();
	
protected:
	OBSIDIAN_API void DrawItems(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext) const;
	
protected:
	struct FRepData
	{
		struct FEquipmentItemDebug
		{
			FString Name;
			FString Item;
			FGameplayTag SlotTag;
			TArray<FString> SpawnedEquipmentPieces;
			TArray<FString> OwnedAbilitySets;
		};
		TArray<FEquipmentItemDebug> Items;
		
		void Serialize(FArchive& Ar);
	};
	FRepData DataPack;

private:
	// Save off the last expected draw size so that we can draw a border around it next frame (and hope we're the same size)
	float LastDrawDataEndSize = 0.0f;
};

#endif // WITH_GAMEPLAY_DEBUGGER_MENU
