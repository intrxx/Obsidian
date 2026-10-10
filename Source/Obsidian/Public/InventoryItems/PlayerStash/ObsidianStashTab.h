// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

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
	UObsidianStashTab(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	
	FString GetStashTabName() const;
	FGameplayTag GetStashTabTag() const;
	EObsidianStashTabAccessability GetStashAccessabilityType() const;
	EObsidianStashTabType GetStashTabType() const;
	TSubclassOf<UObsidianStashTabWidget> GetWidgetClass() const;
	void SetStashData(const FObsidianStashTabDefinition& InDefinition);

	virtual UObsidianInventoryItemInstance* GetInstanceAtPosition(const FObsidianItemPosition& InItemPosition) {return nullptr;}

	virtual bool CanPlaceItemAtSpecificPosition(const FObsidianItemPosition& InSpecifiedPosition, const FGameplayTag& InItemCategory, const FGameplayTag& InItemBaseType, const FIntPoint& InItemGridSpan) {return false;}
	virtual bool FindFirstAvailablePositionForItem(FObsidianItemPosition& OutFirstAvailablePosition, const FGameplayTag& InItemCategory, const FGameplayTag& InItemBaseType, const FIntPoint& InItemGridSpan) {return false;}
	
	virtual bool CanReplaceItemAtSpecificPosition(const FObsidianItemPosition& InSpecifiedPosition, const UObsidianInventoryItemInstance* InReplacingInstance) {return false;}
	virtual bool CanReplaceItemAtSpecificPosition(const FObsidianItemPosition& InSpecifiedPosition, const TSubclassOf<UObsidianInventoryItemDefinition>& InReplacingDef) {return false;}

	/** Return true if the position in Stash Tab is free. */
	virtual bool DebugVerifyPositionFree(const FObsidianItemPosition& InPosition) {return false;}
	
	virtual void Construct(UObsidianPlayerStashComponent* InStashComponent) {}
	virtual void MarkSpaceInTab(UObsidianInventoryItemInstance* InItemInstance, const FObsidianItemPosition& InAtPosition) {}
	virtual void UnmarkSpaceInTab(UObsidianInventoryItemInstance* InItemInstance, const FObsidianItemPosition& InAtPosition) {}

protected:
	FString StashTabName = FString();
	FGameplayTag StashTabTag = FGameplayTag::EmptyTag;
	EObsidianStashTabAccessability StashTabAccessabilityType = EObsidianStashTabAccessability::None;
	EObsidianStashTabType StashTabType = EObsidianStashTabType::STT_None;
	TSubclassOf<UObsidianStashTabWidget> StashTabWidgetClass;
};
