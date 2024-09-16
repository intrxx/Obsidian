// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "ObsidianAdvancedCombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackTraceStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackTraceFinished);
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
	void StartTrace(EObsidianTraceType TraceType = EObsidianTraceType::ETT_SimpleLineTrace, const EObsidianTracedMeshType TracedMeshType = EObsidianTracedMeshType::ETMT_CharacterMesh);

	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void StopTrace();

	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void AddIgnoredActor(AActor* InIgnoredActor);

	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void AddIgnoredActors(TArray<AActor*> InIgnoredActors);

	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void RemoveIgnoredActor(AActor* IgnoredActorToRemove);

	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void AddTracedMesh(UPrimitiveComponent* InTracedMesh, const EObsidianTracedMeshType TracedMeshType);

	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void AddTracedMeshes(TMap<UPrimitiveComponent*, EObsidianTracedMeshType> InTracedMeshesMap);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void RemoveTracedMesh(UPrimitiveComponent* TracedMeshToRemove);

	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void ClearTracedMeshes();

public:
	UPROPERTY(BlueprintAssignable, Category = "Obsidian Advanced Combat|Events")
	FOnAttackTraceStarted OnAttackTraceStartedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian Advanced Combat|Events")
	FOnAttackTraceFinished OnAttackTraceFinishedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian Advanced Combat|Events")
	FOnAttackHit OnAttackHitDelegate;
	
protected:
	virtual void BeginPlay() override;

protected:
	/** Meshes to gather the socket used for trace from, could be a character mesh that hits with hands or weapon mesh. */
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|Collision")
	TMap<UPrimitiveComponent*, EObsidianTracedMeshType> TracedMeshesMap;

	/** Initial ignored Actors for traces. */
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|Collision")
	TArray<AActor*> IgnoredActors;

private:
	bool bStartTrace = false;

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> CurrentTracedMesh = nullptr;
	
};

