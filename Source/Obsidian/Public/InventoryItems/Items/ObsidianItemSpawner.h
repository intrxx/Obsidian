// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "Interaction/ObsidianHighlightInterface.h"
#include "Interaction/ObsidianInteractionInterface.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "GameFramework/Actor.h"
#include "ObsidianItemSpawner.generated.h"

class AObsidianDroppableItem;
class UObsidianItemDropComponent;
class UObsidianPlayerInputManager;

UENUM()
enum class EObsidianItemSpawnerLevelPolicy : uint8
{
	None = 0 UMETA(DisplayName = "None"),

	/**
	 * After setting the Policy to Static it will allow for setting the ItemSpawnerStaticLevel which will be used for
	 * this spawner intead of Area Level.
	 */
	Static UMETA(DisplayName = "Static"),

	/** Base level of the Spawner will equal the Area Level where the Spawner resides. */
	InheritFromArea UMETA(DisplayName = "Inherit From Area"),
};

UCLASS()
class OBSIDIAN_API AObsidianItemSpawner : public AActor, public IObsidianHighlightInterface, public IObsidianInteractionInterface
{
	GENERATED_BODY()
	
public:	
	AObsidianItemSpawner(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ Start of HighlightInterface
	virtual AActor* GetHighlightAvatarActor() override;
	virtual void StartHighlight() override;
	virtual void StopHighlight() override;
	//~ End of HighlightInterface

	//~ Start of InteractionInterface
	virtual AActor* GetInteractionActor() override;
	virtual bool CanInteract() override;
	virtual float GetInteractionRadius() override;
	virtual void Interact(AObsidianPlayerController* InteractingPlayerController) override;
	//~ End of InteractionInterface

	UFUNCTION(BlueprintCallable, Category = "Obsidian|ItemSpawner")
	FVector GetItemSpawnLocation() const;
	
	uint8 GetItemSpawnerLevel() const;

	void SpawnItem();
	
protected:
	void OnSpawningItemsFinished(const bool bDroppedItem);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UObsidianItemDropComponent> ItemDropComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	EObsidianEntityRarity ItemSpawnerRarity = EObsidianEntityRarity::Neutral;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	EObsidianItemSpawnerLevelPolicy ItemSpawnerLevelPolicy = EObsidianItemSpawnerLevelPolicy::Static;

	/** Level used when ItemSpawnerLevelPolicy is Static. */
	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "ItemSpawnerLevelPolicy == EObsidianItemSpawnerLevelPolicy::Static"), Category = "Obsidian|Setup")
	uint8 ItemSpawnerStaticLevel = 1;

	/** Items to drop count, 0 for infinite. */ 
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	uint8 TimesToDrop = 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	float InteractionRadius = 150.0f;

private:
	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SpawnPointComp;
	
	bool bCanInteract = true;

	int32 TimesDropped = 0;
};
