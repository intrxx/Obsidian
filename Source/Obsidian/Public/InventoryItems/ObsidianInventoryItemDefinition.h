// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
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

	bool HasStacks() const;
	bool IsStackable() const;

	const UObsidianInventoryItemFragment* FindFragmentByClass(const TSubclassOf<UObsidianInventoryItemFragment>& FragmentClass) const;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Display")
	FText DisplayName;

	/** Collection of Fragments that extends and defines this item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Obsidian|Display")
	TArray<TObjectPtr<UObsidianInventoryItemFragment>> ItemFragments;
};
