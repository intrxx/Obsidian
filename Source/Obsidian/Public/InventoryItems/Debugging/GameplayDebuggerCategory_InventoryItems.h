// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#if WITH_GAMEPLAY_DEBUGGER_MENU
#include "GameplayDebuggerCategory.h"

class UObsidianInventoryComponent;

/**
 * 
 */
class FGameplayDebuggerCategory_InventoryItems : public FGameplayDebuggerCategory
{
	
public:
	OBSIDIAN_API FGameplayDebuggerCategory_InventoryItems();

	OBSIDIAN_API virtual void CollectData(APlayerController* InOwnerPC, AActor* InDebugActor) override;
	OBSIDIAN_API virtual void DrawData(APlayerController* InOwnerPC, FGameplayDebuggerCanvasContext& InCanvasContext) override;

	OBSIDIAN_API static TSharedRef<FGameplayDebuggerCategory> MakeInstance();

protected:
	OBSIDIAN_API void DrawItems(APlayerController* InOwnerPC, FGameplayDebuggerCanvasContext& InCanvasContext) const;
	
protected:
	struct FRepData
	{
		struct FInventoryItemDebug
		{
			FString Name;
			FString ItemUniqueID;
			FString Item;
			int32 CurrentStackCount;
			int32 MaxStackCount;
			int32 LimitStackCount;
			FIntPoint GridSpan;
			FIntPoint CurrentGridLocation;
		};
		TArray<FInventoryItemDebug> Items;
		TMap<FIntPoint, bool> InventoryStateMap;

		void Serialize(FArchive& InOutAr);
	};
	FRepData DataPack;

private:
	// Save off the last expected draw size so that we can draw a border around it next frame (and hope we're the same size)
	float LastDrawDataEndSize = 0.0f;
};

#endif // WITH_GAMEPLAY_DEBUGGER_MENU
