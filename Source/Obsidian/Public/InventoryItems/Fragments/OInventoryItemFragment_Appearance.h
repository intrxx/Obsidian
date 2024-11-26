// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "OInventoryItemFragment_Appearance.generated.h"

class UObsidianItemWidget;

/**
 * 
 */
UCLASS(DisplayName="Appearance")
class OBSIDIAN_API UOInventoryItemFragment_Appearance : public UObsidianInventoryItemFragment
{
	GENERATED_BODY()

public:
	virtual void OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const override;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
	TObjectPtr<USkeletalMesh> SkeletalMesh;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
	TObjectPtr<UTexture2D> ItemImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
	FText DisplayName;
};
