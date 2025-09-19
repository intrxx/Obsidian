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
class UObsidianItemDropComponent;
class UObsidianPlayerInputManager;

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
	void OnSpawningItemsFinished();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UObsidianItemDropComponent> ItemDropComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	float InteractionRadius = 150.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	int32 TimesToDrop = 1;

private:
	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SpawnPointComp;
	
	bool bCanInteract = true;

	int32 TimesDropped = 0;
};
