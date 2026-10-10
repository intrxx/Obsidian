// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "ObsidianTreasureList.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "ObsidianItemDropComponent.generated.h"

class UOInventoryItemFragment_Affixes;
class UObsidianInventoryItemDefinition;
class UObsidianItemDataDeveloperSettings;

UENUM()
enum class EObsidianAdditionalTreasureListPolicy : uint8
{
	/** If the classes from attached treasure list meet the treasure level requirement, it will be simply an additional treasure list to roll from. */
	TryToRoll = 0 UMETA(DisplayName = "Try To Roll"),

	/**
	 * If the classes from attached treasure list meet the treasure level requirement, it will always be chosen to special guaranteed roll
	 * (once, and will consume roll) from this list.
	 */
	TryToAddAlwaysRoll UMETA(DisplayName = "Try To Add Always Roll"),

	/**
	 * This list will always be chosen to special guaranteed roll (once, and will consume roll) from, even if the Treasure Quality is lower,
	 * can be used to force some specific Boss loot.
	 */
	AlwaysRoll UMETA(DisplayName = "Always Roll"),

	/** This list will override the common treasure lists, and will be a single source to roll (possibly [1-5] times) from. */
	OverrideRoll UMETA(DisplayName = "Override Roll")
};

USTRUCT()
struct FObsidianAdditionalTreasureList
{
	GENERATED_BODY()
	
public:
	
#if WITH_EDITOR
	EDataValidationResult ValidateData(FDataValidationContext& InContext, const int InIndex) const;
#endif
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	EObsidianAdditionalTreasureListPolicy TreasureListPolicy = EObsidianAdditionalTreasureListPolicy::TryToRoll;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TSoftObjectPtr<UObsidianTreasureList> TreasureList;
};

/**
 * Broadcasts when the DropItem logic is finished, caution, for now this broadcasts with true after successfully
 * requesting some items to drop to the manager, not when the items are actually dropped as the work is passed to the manager.
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnDroppingItemsFinishedSignature, const bool bDroppedItem)

class UObsidianItemDataLoaderSubsystem;

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OBSIDIAN_API UObsidianItemDropComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	UObsidianItemDropComponent(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	
	void DropItems(const EObsidianEntityRarity InDroppingEntityRarity, const uint8 InDroppingEntityLevel, const FVector& InOverrideDropLocation = FVector::ZeroVector);

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& InContext) const override;
#endif

public:
	FOnDroppingItemsFinishedSignature OnDroppingItemsFinishedDelegate;

protected:
	virtual void BeginPlay() override;
	
	void LoadAdditionalTreasuresAsync();

	void ConstructItem(FObsidianItemToDrop& InOutForItemToDrop);
	void GetTreasureClassesToRollFrom(const uint8 InMaxTreasureClassQuality, TArray<FObsidianTreasureClass>& OutTreasureClasses,
		TArray<FObsidianTreasureClass>& OutMustRollFromTreasureClasses);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	float ItemDropRadius = 60.0f;

	/** Lets you specify GameplayTag to limit the common treasure classes to drop, this does not work for AdditionalTreasureLists by design. */
	UPROPERTY(EditAnywhere, Category = "Obsidian")
	uint8 bLimitCommonTreasureCategory:1 = false;

	/** Limits the drop items to be specific Category of Treasure Lists, e.g. "Item.Category.Equipment.Weapon". */
	UPROPERTY(EditAnywhere, Meta = (Categories = "Item.Category", EditCondition = "bLimitCommonTreasureCategory", EditConditionHides), Category = "Obsidian")
	FGameplayTag LimitCommonTreasureCategoryTag = FGameplayTag::EmptyTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<FObsidianAdditionalTreasureList> AdditionalTreasureLists;

private:
	bool ConstructItemToDrop(const FObsidianDropItem& InDropItem, const FVector& InOverrideDropLocation, const uint8 InTreasureQuality,
		FObsidianItemToDrop& OutItemToDrop);
	
	FTransform GetDropTransformAligned(const AActor* InDroppingActor, const FVector& InOverrideDropLocation = FVector::ZeroVector) const;
	EObsidianItemRarity RollItemRarity(const EObsidianItemRarity InMaxRarity);

	void HandleDefaultGeneration(FObsidianItemToDrop& InOutForItemToDrop, const FGameplayTag& InDropItemCategory, const FGameplayTag& InDropItemBaseTypeTag,
		const UOInventoryItemFragment_Affixes* InAffixFragment);
	void HandleFullGeneration(FObsidianItemToDrop& InOutForItemToDrop, const FGameplayTag& InDropItemCategory, const FGameplayTag& InDropItemBaseTypeTag,
		const UOInventoryItemFragment_Affixes* InAffixFragment);
	void HandleNoGeneration(FObsidianItemToDrop& InOutForItemToDrop, const UOInventoryItemFragment_Affixes* InAffixFragment);
	
	void RollSkillImplicits(FObsidianItemToDrop& InOutForItemToDrop, const TArray<FObsidianDynamicItemAffix>& InSkillImplicits);
	void RollImplicit(FObsidianItemToDrop& InOutForItemToDrop, const TArray<FObsidianDynamicItemAffix>& InImplicits);
	void RollAffixesAndPrefixes(FObsidianItemToDrop& InOutForItemToDrop, TArray<FObsidianDynamicItemAffix>& InOutPrefixes, TArray<FObsidianDynamicItemAffix>& InOutSuffixes);
	void TryToGiveStaticImplicit(FObsidianItemToDrop& InOutForItemToDrop, const UOInventoryItemFragment_Affixes* InAffixFragment);
	void TryToGivePrimaryItemAffix(FObsidianItemToDrop& InOutForItemToDrop, const UOInventoryItemFragment_Affixes* InAffixFragment);

	FGameplayTag GetItemBaseTypeFromDropItem(const FObsidianDropItem& InDropItem);
	EObsidianItemRarity GetItemDefaultRarityFromDropItem(const FObsidianDropItem& InDropItem);
	bool ShouldApplyAffixValueMultiplier(const EObsidianItemRarity InForItemRarity);

	uint8 GetNumberOfAffixesToRollWeighted(const EObsidianItemRarity InForItemRarity);

	void AdjustItemRequirementsBasedOnAddedAffixes(FObsidianItemRequirements& OutRequirements, const FObsidianItemToDrop& InFromItemToDrop);

private:
#if WITH_OBSIDIAN_DEBUG_MENU
	friend class FObsidianDebugTab_Items;
#endif
	
	UPROPERTY()
	UObsidianItemDataLoaderSubsystem* CachedItemDataLoader = nullptr;
};
