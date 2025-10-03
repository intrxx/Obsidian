// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project


#include "ObsidianInventoryItemDefinition.generated.h"

class UObsidianInventoryItemFragment;
class UOInventoryItemFragment_Affixes;

DECLARE_LOG_CATEGORY_EXTERN(LogItems, Log, All);

/**
 * Constant config of the item.
 */
UCLASS(Blueprintable, Const, Abstract)
class OBSIDIAN_API UObsidianInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UObsidianInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(BlueprintCallable, Category="Obsidian|Debug")
	FString GetDebugName() const
	{
		return DebugName;
	}

	UFUNCTION(BlueprintCallable, Category = "Obsidian|ItemDefinition")
	FGameplayTag GetItemRarityTag() const;
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian|ItemDefinition")
	FGameplayTag GetItemCategoryTag() const;
	
	int32 GetItemLevel() const;
	
	bool HasStacks() const;
	bool IsStackable() const;
	bool IsEquippable() const;
	bool DoesStartIdentified() const;
	bool DoesItemNeedsTwoSlots() const;

	const UObsidianInventoryItemFragment* FindFragmentByClass(const TSubclassOf<UObsidianInventoryItemFragment>& FragmentClass) const;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories="Item.Category"), Category = "Obsidian")
	FGameplayTag ItemCategory = FGameplayTag::EmptyTag;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Obsidian")
	int32 ItemLevel = INDEX_NONE;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Debug")
	FString DebugName;

	/** Collection of Fragments that extends and defines this item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Obsidian")
	TArray<TObjectPtr<UObsidianInventoryItemFragment>> ItemFragments;
};
