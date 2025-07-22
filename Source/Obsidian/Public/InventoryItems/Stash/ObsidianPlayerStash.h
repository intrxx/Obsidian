// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

// ~ Project
#include "Interaction/ObsidianHighlightInterface.h"
#include "Interaction/ObsidianInteractionInterface.h"

#include "ObsidianPlayerStash.generated.h"

class UObsidianHeroComponent;

UCLASS()
class OBSIDIAN_API AObsidianPlayerStash : public AActor, public IObsidianHighlightInterface, public IObsidianInteractionInterface
{
	GENERATED_BODY()
	
public:	
	AObsidianPlayerStash(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

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
