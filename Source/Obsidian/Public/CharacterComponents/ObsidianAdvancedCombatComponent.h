// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "ObsidianAdvancedCombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackTraceStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackTraceFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackHit, FHitResult, Hit);

USTRUCT(BlueprintType)
struct FObsidianAdvancedCombatSockets
{
	GENERATED_BODY()

	FObsidianAdvancedCombatSockets(){};
	
	FObsidianAdvancedCombatSockets(const FName InStartSocketName, const FName InEndSocketName)
	{
		StartSocketName = InStartSocketName;
		EndSocketName = InEndSocketName;
	};
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian Advanced Combat|Setup")
	FName StartSocketName = "AdvancedCombat_Start";

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian Advanced Combat|Setup")
	FName EndSocketName = "AdvancedCombat_End";
};

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
	void StartTrace(const EObsidianTraceType TraceType = EObsidianTraceType::ETT_SimpleLineTrace, const EObsidianTracedMeshType TracedMeshType = EObsidianTracedMeshType::ETMT_CharacterMesh);

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
	void AddTracedMeshes(TMap<EObsidianTracedMeshType, UPrimitiveComponent*> InTracedMeshesMap);
	
	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void RemoveTracedMeshWithType(const EObsidianTracedMeshType TracedMeshType);

	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void ClearTracedMeshes();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian Advanced Combat|Setup")
	TEnumAsByte<ETraceTypeQuery> TraceChannel = UEngineTypes::ConvertToTraceType(Obsidian_TraceChannel_Melee);

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian Advanced Combat|Setup")
	TMap<EObsidianTracedMeshType, FObsidianAdvancedCombatSockets> SocketsMap;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian Advanced Combat|Setup")
	int32 TraceIntervalCount = 5;

	/** n + 1 number of lines that will be drawn with SemiComplexLineTrace. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian Advanced Combat|Setup|LineTrace")
	int32 MultiLineCount = 3;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian Advanced Combat|Setup|BoxTrace")
	FVector BoxTraceHalfSize = FVector(10.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian Advanced Combat|Setup|CapsuleTrace")
	float CapsuleRadius = 5.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian Advanced Combat|Setup|CapsuleTrace")
	float CapsuleHalfHeight = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian Advanced Combat|Setup")
	bool bTraceComplex = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian Advanced Combat|Setup")
	bool bStartInCurrentTick = true;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian Advanced Combat|Debug")
	bool bWithDebug = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian Advanced Combat|Debug")
	FLinearColor DebugTraceColor = FLinearColor::Green;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian Advanced Combat|Debug")
	FLinearColor DebugHitColor = FLinearColor::Red;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian Advanced Combat|Debug")
	float DebugDuration = 5.0f;
	
	UPROPERTY(BlueprintAssignable, Category = "Obsidian Advanced Combat|Events")
	FOnAttackTraceStarted OnAttackTraceStartedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian Advanced Combat|Events")
	FOnAttackTraceFinished OnAttackTraceFinishedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Obsidian Advanced Combat|Events")
	FOnAttackHit OnAttackHitDelegate;

protected:
	/** Meshes to gather the socket used for trace from, could be a character mesh that hits with hands or weapon mesh. */
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|Collision")
	TMap<EObsidianTracedMeshType, UPrimitiveComponent*> TracedMeshesMap;

	/** Initial ignored Actors for traces. */
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|Collision")
	TArray<AActor*> IgnoredActors;

private:
	void TickTrace();

	void GetSocketsLocationsByMesh(const UPrimitiveComponent* Mesh, FVector& OutStartSocketLoc, FVector& OutEndSocketLoc) const;
	void HandleHit(const bool bHit, const TArray<FHitResult>& HitResults) const;
	void CalculateNextTracePoint(const int32 Index, const int32 Count, const FVector& Start, const FVector& End, FVector& OutTracePoint);

	void SimpleLineTrace() const;
	void SemiComplexLineTrace();
	void ComplexLineTrace();
	void SimpleBoxTrace() const;
	void ComplexBoxTrace();
	void SimpleCapsuleTrace() const;
	
private:
	bool bStartTrace = false;

	UPROPERTY()
	FVector CachedStart = FVector::ZeroVector;
	UPROPERTY()
	FVector CachedEnd = FVector::ZeroVector;

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> CurrentTracedMesh = nullptr;

	UPROPERTY()
	EObsidianTraceType CurrentTraceType = EObsidianTraceType::ETT_SimpleLineTrace;

	UPROPERTY()
	FName TraceStartSocketName = "";
	UPROPERTY()
	FName TraceEndSocketName = "";
	
};

