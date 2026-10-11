// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Interaction/ObsidianInteractionInterface.h"

#include "ObsidianTownPortal.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API AObsidianTownPortal : public AActor, public IObsidianInteractionInterface
{
	GENERATED_BODY()

public:
	AObsidianTownPortal(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	void InitializePortal();

	bool IsOpeningInProgress() const { return bIsOpening; }

	//~ Begin UObject Interface
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	//~ End UObject Interface

	//~ Begin IObsidianInteractionInterface Interface
	virtual AActor* GetInteractionActor() override;
	virtual bool CanInteract() override { return bCanTeleport; }
	virtual float GetInteractionRadius() override { return InteractionRadius; }
	virtual void Interact(AObsidianPlayerController* InInteractingPlayerController) override;
	//~ End IObsidianInteractionInterface Interface

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Mesh")
	TObjectPtr<UStaticMeshComponent> ClickableMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Effects")
	TObjectPtr<UNiagaraComponent> PortalNiagaraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Effects")
	TObjectPtr<UNiagaraSystem> PortalOpeningEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Effects")
	TObjectPtr<UNiagaraSystem> PortalEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	float PortalPrepareToOpenDuration = 30.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Interaction")
	float InteractionRadius = 50.0f;

private:
	void StartPortalOpeningTimer();

	UPROPERTY(Replicated)
	bool bCanTeleport = true;

	bool bIsOpening = false;

	UPROPERTY()
	TObjectPtr<AObsidianPlayerController> PortalOwner;

	FTimerHandle PortalOpenedTimerHandle;
};
