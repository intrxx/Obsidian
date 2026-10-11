// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Interaction/ObsidianHighlightInterface.h"
#include "Interaction/ObsidianInteractionInterface.h"

#include "ObsidianPlayerStash.generated.h"

class UObsidianPlayerInputManager;

UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API AObsidianPlayerStash : public AActor, public IObsidianHighlightInterface, public IObsidianInteractionInterface
{
	GENERATED_BODY()

public:
	AObsidianPlayerStash(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	//~ Begin IObsidianHighlightInterface Interface
	virtual AActor* GetHighlightAvatarActor() override;
	virtual void StartHighlight() override;
	virtual void StopHighlight() override;
	//~ End IObsidianHighlightInterface Interface

	//~ Begin IObsidianInteractionInterface Interface
	virtual AActor* GetInteractionActor() override;
	virtual bool CanInteract() override { return bCanInteract; }
	virtual bool RequiresOngoingInteraction() override;
	virtual float GetInteractionRadius() override { return InteractionRadius; }
	virtual void Interact(AObsidianPlayerController* InInteractingPlayerController) override;
	virtual void StopInteraction(AObsidianPlayerController* InInteractingPlayerController) override;
	//~ End IObsidianInteractionInterface Interface

protected:
	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> RootSceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Interaction")
	float InteractionRadius = 100.0f;

private:
	bool bCanInteract = true;
};
