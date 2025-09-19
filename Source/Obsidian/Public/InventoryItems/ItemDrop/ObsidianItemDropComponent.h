// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "Components/ActorComponent.h"
#include "ObsidianItemDropComponent.generated.h"

class UObsidianTreasureList;
class UObsidianInventoryItemDefinition;

UENUM()
enum class EObsidianAdditionalTreasureListPolicy : uint8
{
	/** If the classes from attached treasure list meet the treasure level requirement, it will be simply an additional treasure list to roll from. */
	TryToRoll = 0 UMETA(DisplayName = "Try To Roll"),

	/** This list will always be chosen to roll from, can be used to force some specific Boss loot. */
	AlwaysRoll UMETA(DisplayName = "Always Roll"),

	/** This list will override the common treasure lists, and will be a single source to roll from. */
	OverrideRoll UMETA(DisplayName = "Override Roll")
};

USTRUCT()
struct FObsidianAdditionalTreasureList
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	EObsidianAdditionalTreasureListPolicy TreasureListPolicy;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TSoftObjectPtr<UObsidianTreasureList> TreasureList;
};

DECLARE_LOG_CATEGORY_EXTERN(LogDropComponent, Log, All);

DECLARE_MULTICAST_DELEGATE(FOnDroppingItemsFinishedSignature)

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OBSIDIAN_API UObsidianItemDropComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	UObsidianItemDropComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void InitializeWithItemDropComponent();

	void DropItems(const FVector& InOverrideDropLocation = FVector::ZeroVector);

public:
	FOnDroppingItemsFinishedSignature OnDroppingItemsFinishedDelegate;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	float ItemDropRadius = 60.0f;

	//TODO(intrxx) Maybe just allow one?
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<FObsidianAdditionalTreasureList> AdditionalTreasureLists;

private:
	FTransform GetDropTransformAligned(const AActor* DroppingActor, const FVector& InOverrideDropLocation = FVector::ZeroVector) const;
};
