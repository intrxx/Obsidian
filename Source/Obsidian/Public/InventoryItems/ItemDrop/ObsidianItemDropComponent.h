// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "Components/ActorComponent.h"
#include "ObsidianItemDropComponent.generated.h"

class UObsidianInventoryItemDefinition;

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

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<const TSubclassOf<UObsidianInventoryItemDefinition>> TempItemDropTable;

private:
	FTransform GetDropTransformAligned(const AActor* DroppingActor, const FVector& InOverrideDropLocation = FVector::ZeroVector) const;
};
