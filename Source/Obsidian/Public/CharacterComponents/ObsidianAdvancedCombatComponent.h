// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "ObsidianAdvancedCombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackHit, FHitResult, Hit);

/**
 * Component that gives the functionality to perform advance tracing using sockets on mesh.
 * Should not be used on regular enemies as it might be quite expensive.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OBSIDIAN_API UObsidianAdvancedCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UObsidianAdvancedCombatComponent();

	UFUNCTION(BlueprintPure, Category = "Obsidian Advanced Combat")
	static UObsidianAdvancedCombatComponent* FindAdvancedCombatComponent(const AActor* Owner)
	{
		return (Owner ? Owner->FindComponentByClass<UObsidianAdvancedCombatComponent>() : nullptr);
	}

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void StartTrace(EObsidianTraceType TraceType = EObsidianTraceType::SimpleLineTrace);

	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void StopTrace();
	
protected:
	virtual void BeginPlay() override;

protected:
	/** Meshes to gather the socket used for trace from, could be a character mesh that hits with hands or weapon mesh. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Collision")
	TArray<UPrimitiveComponent*> TracedMeshes;

	/** Initial ignored Actors for traces. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Collision")
	TArray<AActor*> IgnoredActors;

private:
	bool bStartTrace = false;
	
};
