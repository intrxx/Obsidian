// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project


#include "ObsidianStashTab.generated.h"

struct FObsidianItemPosition;

class UObsidianPlayerStashComponent;
class UObsidianInventoryItemInstance;

/**
 * 
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class OBSIDIAN_API UObsidianStashTab : public UObject
{
	GENERATED_BODY()

public:
	UObsidianStashTab(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	FGameplayTag GetStashTabTag() const;

	virtual void Construct(UObsidianPlayerStashComponent* StashComponent) {}
	virtual void MarkSpaceInTab(UObsidianInventoryItemInstance* ItemInstance, const FObsidianItemPosition& AtPosition) {}
	virtual void UnmarkSpaceInTab(UObsidianInventoryItemInstance* ItemInstance, const FObsidianItemPosition& AtPosition) {}

protected:
	UPROPERTY(EditAnywhere, Category = "Obsidian")
	FGameplayTag StashTabTag = FGameplayTag::EmptyTag;
};
