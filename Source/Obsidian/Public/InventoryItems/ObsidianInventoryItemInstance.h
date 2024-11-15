// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianInventoryItemInstance.generated.h"

class UObsidianInventoryItemFragment;
class UObsidianInventoryItemDefinition;
/**
 * Instance of and Item Definition. When a player obtain an item, this is the actual thing that they receive.
 */
UCLASS(BlueprintType)
class OBSIDIAN_API UObsidianInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UObsidianInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ Start of UObject interface
	virtual bool IsSupportedForNetworking() const override {return true;}
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End of UObject interface

	TSubclassOf<UObsidianInventoryItemDefinition> GetItemDef() const
	{
		return ItemDef;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta=(DeterminesOutputType = FragmentClass))
	const UObsidianInventoryItemFragment* FindFragmentByClass(TSubclassOf<UObsidianInventoryItemFragment> FragmentClass) const;

	template <typename T>
	const T* FindFragmentByClass() const
	{
		return (T*)FindFragmentByClass(T::StaticClass());
	}

	void SetItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef)
	{
		ItemDef = InItemDef;
	}

private:
	UPROPERTY(Replicated)
	TSubclassOf<UObsidianInventoryItemDefinition> ItemDef;
};
