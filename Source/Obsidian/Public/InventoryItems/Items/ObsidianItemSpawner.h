// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "Interaction/ObsidianHighlightInterface.h"
#include "Interaction/ObsidianInteractionInterface.h"

#include "GameFramework/Actor.h"
#include "ObsidianItemSpawner.generated.h"

class AObsidianDroppableItem;
class UObsidianHeroComponent;

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
	
	void SpawnItem();

protected:
	/** This can be overridden to manipulate the ItemToDropClass that will be spawned. */
	void RollItemDrop();

protected:
	UPROPERTY(EditDefaultsOnly,  Category = "Obsidian|Setup")
	TSubclassOf<AObsidianDroppableItem> ItemToDropClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	bool bRandomizeItem = false;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	int32 MaxSpawnCount = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	float InteractionRadius = 150.0f;

private:
	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SpawnPointComp;
	
	int32 SpawnedItems = 0;
	
	bool bCanInteract = true;
};
