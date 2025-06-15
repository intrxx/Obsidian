// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#if WITH_GAMEPLAY_DEBUGGER_MENU
#include "GameplayDebuggerCategory.h"

// ~ Project

class UObsidianInventoryComponent;

/**
 * 
 */
class FGameplayDebuggerCategory_InventoryItems : public FGameplayDebuggerCategory
{
	
public:
	OBSIDIAN_API FGameplayDebuggerCategory_InventoryItems();

	OBSIDIAN_API virtual void CollectData(APlayerController* OwnerPC, AActor* DebugActor) override;
	OBSIDIAN_API virtual void DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext) override;

	OBSIDIAN_API static TSharedRef<FGameplayDebuggerCategory> MakeInstance();

protected:
	OBSIDIAN_API void DrawItems(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext) const;
	
protected:
	struct FRepData
	{
		struct FInventoryItemDebug
		{
			FString Name;
			FString Item;
			int32 CurrentStackCount;
			int32 MaxStackCount;
			int32 LimitStackCount;
			FIntPoint GridSpan;
			FIntPoint CurrentGridLocation;
		};
		TArray<FInventoryItemDebug> Items;

		TMap<FIntPoint, bool> InventoryStateMap;

		void Serialize(FArchive& Ar);
	};
	FRepData DataPack;

private:
	// Save off the last expected draw size so that we can draw a border around it next frame (and hope we're the same size)
	float LastDrawDataEndSize = 0.0f;
};

#endif // WITH_GAMEPLAY_DEBUGGER_MENU
