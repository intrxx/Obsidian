// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ObsidianInventoryItemDefinition.generated.h"

class UObsidianInventoryItemFragment;

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
	
	bool HasStacks() const;
	bool IsStackable() const;

	const UObsidianInventoryItemFragment* FindFragmentByClass(const TSubclassOf<UObsidianInventoryItemFragment>& FragmentClass) const;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Debug")
	FString DebugName;

	/** Collection of Fragments that extends and defines this item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Obsidian|Display")
	TArray<TObjectPtr<UObsidianInventoryItemFragment>> ItemFragments;
};
