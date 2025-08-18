// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "Interaction/ObsidianInteractionInterface.h"

#include "GameFramework/Actor.h"
#include "ObsidianTownPortal.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class OBSIDIAN_API AObsidianTownPortal : public AActor, public IObsidianInteractionInterface
{
	GENERATED_BODY()
	
public:	
	AObsidianTownPortal(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void InitializePortal();

	bool IsOpeningInProgress() const;
	
	//~ Start of InteractionInterface
	virtual AActor* GetInteractionActor() override;
	virtual bool CanInteract() override;
	virtual float GetInteractionRadius() override;
	virtual void Interact(AObsidianPlayerController* InteractingPlayerController) override;
	//~ End of InteractionInterface

protected:
	virtual void BeginPlay() override;
	
protected:
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
	
private:
	UPROPERTY(Replicated)
	bool bCanTeleport = true;
	
	bool bIsOpening = false;

	UPROPERTY()
	TObjectPtr<AObsidianPlayerController> PortalOwner;
	
	FTimerHandle PortalOpenedTimerHandle;
};
