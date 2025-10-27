// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"
#include "ObsidianTreasureList.h"

#include "Components/ActorComponent.h"
#include "ObsidianItemDropComponent.generated.h"

class UOInventoryItemFragment_Affixes;
class UObsidianInventoryItemDefinition;
class UObsidianItemDataDeveloperSettings;

UENUM()
enum class EObsidianAdditionalTreasureListPolicy : uint8
{
	/** If the classes from attached treasure list meet the treasure level requirement, it will be simply an additional treasure list to roll from. */
	TryToRoll = 0 UMETA(DisplayName = "Try To Roll"),

	/** If the classes from attached treasure list meet the treasure level requirement, it will always be chosen to special guaranteed roll (once, and will consume roll) from this list. */
	TryToAddAlwaysRoll UMETA(DisplayName = "Try To Add Always Roll"),

	/** This list will always be chosen to special guaranteed roll (once, and will consume roll) from, even if the Treasure Quality is lower, can be used to force some specific Boss loot. */
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
	EDataValidationResult ValidateData(FDataValidationContext& Context, const int Index) const;
#endif
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	EObsidianAdditionalTreasureListPolicy TreasureListPolicy = EObsidianAdditionalTreasureListPolicy::TryToRoll;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TSoftObjectPtr<UObsidianTreasureList> TreasureList;
};

DECLARE_LOG_CATEGORY_EXTERN(LogDropComponent, Log, All);

/**
 * Broadcasts when the DropItem logic is finished, caution, for now this broadcasts with true after successfully
 * requesting some items to drop to the manager, not when the items are actually dropped.
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
	UObsidianItemDropComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	void DropItems(const EObsidianEntityRarity DroppingEntityRarity, const uint8 DroppingEntityLevel, const FVector& InOverrideDropLocation = FVector::ZeroVector);

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif

public:
	FOnDroppingItemsFinishedSignature OnDroppingItemsFinishedDelegate;

protected:
	virtual void BeginPlay() override;
	
	void LoadAdditionalTreasuresAsync();

	void GenerateItem(FObsidianItemToDrop& ForItemToDrop, const uint8 MaxTreasureClassQuality);
	void GetTreasureClassesToRollFrom(const uint8 MaxTreasureClassQuality, TArray<FObsidianTreasureClass>& OutTreasureClasses,
		TArray<FObsidianTreasureClass>& OutMustRollFromTreasureClasses);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	float ItemDropRadius = 60.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<FObsidianAdditionalTreasureList> AdditionalTreasureLists;

private:
	bool ConstructItemToDrop(const FObsidianDropItem& DropItem, const FVector& InOverrideDropLocation, const uint8 TreasureQuality, FObsidianItemToDrop& OutItemToDrop);
	
	FTransform GetDropTransformAligned(const AActor* DroppingActor, const FVector& InOverrideDropLocation = FVector::ZeroVector) const;
	EObsidianItemRarity RollItemRarity(const EObsidianItemRarity MaxRarity);

	void HandleDefaultGeneration(FObsidianItemToDrop& ForItemToDrop, const FGameplayTag& DropItemCategory, const UOInventoryItemFragment_Affixes* AffixFragment,
		const uint8 MaxTreasureClassQuality);
	void HandleFullGeneration(FObsidianItemToDrop& ForItemToDrop, const FGameplayTag& DropItemCategory, const uint8 MaxTreasureClassQuality);
	void HandleNoGeneration(FObsidianItemToDrop& ForItemToDrop, const UOInventoryItemFragment_Affixes* AffixFragment, const uint8 MaxTreasureClassQuality);
	
	void RollSkillImplicits(FObsidianItemToDrop& ForItemToDrop, const TArray<FObsidianDynamicItemAffix>& SkillImplicits, const uint8 MaxTreasureClassQuality);
	void RollImplicit(FObsidianItemToDrop& ForItemToDrop, const TArray<FObsidianDynamicItemAffix>& Implicits, const uint8 MaxTreasureClassQuality);
	void RollAffixesAndPrefixes(FObsidianItemToDrop& ForItemToDrop, TArray<FObsidianDynamicItemAffix>& Prefixes, TArray<FObsidianDynamicItemAffix>& Suffixes,
		const uint8 MaxTreasureClassQuality);

	FGameplayTag GetItemBaseTypeFromDropItem(const FObsidianDropItem& DropItem);
	EObsidianItemRarity GetItemDefaultRarityFromDropItem(const FObsidianDropItem& DropItem);
	
private:
	UPROPERTY()
	UObsidianItemDataLoaderSubsystem* CachedItemDataLoader = nullptr;
};
