// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

#if WITH_GAMEPLAY_DEBUGGER_MENU
#include "GameplayDebuggerCategory.h"
#include "GameplayTagContainer.h"

// ~ Project

class FGameplayDebuggerCategory_PlayerStash : public FGameplayDebuggerCategory
{
public:
	OBSIDIAN_API FGameplayDebuggerCategory_PlayerStash();

	OBSIDIAN_API virtual void CollectData(APlayerController* OwnerPC, AActor* DebugActor) override;
	OBSIDIAN_API virtual void DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext) override;

	OBSIDIAN_API static TSharedRef<FGameplayDebuggerCategory> MakeInstance();
	
protected:
	OBSIDIAN_API void DrawItems(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext) const;
	
protected:
	struct FRepData
	{
		struct FStashedItemsDebug
		{
			FString Name;
			FString Item;
			int32 CurrentStackCount;
			int32 MaxStackCount;
			int32 LimitStackCount;
			FIntPoint GridSpan;
			FIntPoint CurrentGridLocation;
			FGameplayTag CurrentSlotTag;
		};
		
		struct FStashSlotsDebug
		{
			bool bUsed;
			
			FString SlotTag;
			TArray<FString> AcceptedTags;
			TArray<FString> BannedTags;
		};

		struct FStashGridDebug
		{
			bool bUsed;
			
			
			TMap<FIntPoint, bool> GridStateMap;

			void Reset()
			{
				bUsed = false;
				GridStateMap.Empty();
			};
		};

		bool bStashActive;
		FGameplayTag StashTabTag;
		TArray<FStashedItemsDebug> Items;
		FStashGridDebug Grid;
		TArray<FStashSlotsDebug> Slots;
		
		void Serialize(FArchive& Ar);
	};
	FRepData DataPack;

private:
	// Save off the last expected draw size so that we can draw a border around it next frame (and hope we're the same size)
	float LastDrawDataEndSize = 0.0f;
	
};

#endif // WITH_GAMEPLAY_DEBUGGER_MENU
