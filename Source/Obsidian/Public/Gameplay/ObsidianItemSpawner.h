// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/ObsidianHighlightInterface.h"
#include "ObsidianItemSpawner.generated.h"

class AObsidianDroppableItem;

UCLASS()
class OBSIDIAN_API AObsidianItemSpawner : public AActor, public IObsidianHighlightInterface
{
	GENERATED_BODY()
	
public:	
	AObsidianItemSpawner(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ Start of HighlightInterface
	virtual AActor* GetHighlightAvatarActor() override;
	virtual void StartHighlight() override;
	virtual void StopHighlight() override;
	//~ End of HighlightInterface

	UFUNCTION(BlueprintCallable, Category = "Obsidian|ItemSpawner")
	FVector GetItemSpawnLocation() const;

protected:
	virtual void BeginPlay() override;

	/** This can be overridden to manipulate the ItemToDropClass that will be spawned. */
	void RollItemDrop();

	UFUNCTION()
	void OnMeshClicked(UPrimitiveComponent* TouchedComponent, FKey ButtonPressed);
	// It looks like it can also be triggered with virtual void NotifyActorOnClicked(FKey ButtonPressed) override;, can investigate later //

protected:
	UPROPERTY(EditDefaultsOnly,  Category = "Obsidian|Setup")
	TSubclassOf<AObsidianDroppableItem> ItemToDropClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	bool bRandomizeItem = false;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	int32 MaxSpawnCount = 1;
	
private:
	UFUNCTION(Server, Reliable)
	void ServerSpawnItem();

private:
	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SpawnPointComp;
	
	int32 SpawnedItems = 0;
};
