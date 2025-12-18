// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>
#include <GameplayTagContainer.h>

#include "ObsidianStashTabsConfig.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "ObsidianStashTab.generated.h"

struct FObsidianItemPosition;

class UObsidianPlayerStashComponent;
class UObsidianInventoryItemInstance;

/**
 * 
 */
UCLASS(Blueprintable, Abstract)
class OBSIDIAN_API UObsidianStashTab : public UObject
{
	GENERATED_BODY()

public:
	UObsidianStashTab(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	FString GetStashTabName() const;
	FGameplayTag GetStashTabTag() const;
	EObsidianStashTabAccessability GetStashAccessabilityType() const;
	EObsidianStashTabType GetStashTabType() const;
	TSubclassOf<UObsidianStashTabWidget> GetWidgetClass() const;
	void SetStashData(const FObsidianStashTabDefinition& InDefinition);

	virtual UObsidianInventoryItemInstance* GetInstanceAtPosition(const FObsidianItemPosition& ItemPosition) {return nullptr;}

	virtual bool CanPlaceItemAtSpecificPosition(const FObsidianItemPosition& SpecifiedPosition, const FGameplayTag& ItemCategory, const FGameplayTag& ItemBaseType, const FIntPoint& ItemGridSpan) {return false;}
	virtual bool FindFirstAvailablePositionForItem(FObsidianItemPosition& OutFirstAvailablePosition, const FGameplayTag& ItemCategory, const FGameplayTag& ItemBaseType, const FIntPoint& ItemGridSpan) {return false;}
	
	virtual bool CanReplaceItemAtSpecificPosition(const FObsidianItemPosition& SpecifiedPosition, const UObsidianInventoryItemInstance* ReplacingInstance) {return false;}
	virtual bool CanReplaceItemAtSpecificPosition(const FObsidianItemPosition& SpecifiedPosition, const TSubclassOf<UObsidianInventoryItemDefinition>& ReplacingDef) {return false;}

	/** Return true if the position in Stash Tab is free. */
	virtual bool DebugVerifyPositionFree(const FObsidianItemPosition& Position) {return false;}
	
	virtual void Construct(UObsidianPlayerStashComponent* StashComponent) {}
	virtual void MarkSpaceInTab(UObsidianInventoryItemInstance* ItemInstance, const FObsidianItemPosition& AtPosition) {}
	virtual void UnmarkSpaceInTab(UObsidianInventoryItemInstance* ItemInstance, const FObsidianItemPosition& AtPosition) {}

protected:
	FString StashTabName = FString();
	FGameplayTag StashTabTag = FGameplayTag::EmptyTag;
	EObsidianStashTabAccessability StashTabAccessabilityType = EObsidianStashTabAccessability::None;
	EObsidianStashTabType StashTabType = EObsidianStashTabType::STT_None;
	TSubclassOf<UObsidianStashTabWidget> StashTabWidgetClass;
};
