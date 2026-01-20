// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianTypes/ObsidianCoreTypes.h"

#include "Components/ActorComponent.h"
#include "ObsidianAdvancedCombatComponent.generated.h"

struct FObsidianAdvancedTraceParams;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackTraceStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackTraceFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackHit, const FHitResult&, Hit);

/** Pair of sockets used in traces. */
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

USTRUCT(BlueprintType)
struct FObsidianAdvancedTraceParams
{
	GENERATED_BODY();
	
	FObsidianAdvancedTraceParams(){}

	FObsidianAdvancedTraceParams(const EObsidianTraceType InTraceType, const EObsidianTracedMeshType InTracedMesh, const bool InAllowOneHitPerTrace)
	{
		TraceType = InTraceType;
		TracedMeshType = InTracedMesh;
		bAllowOneHitPerTrace = InAllowOneHitPerTrace;
	}
	
	UPROPERTY()
	EObsidianTraceType TraceType = EObsidianTraceType::SimpleLineTrace;

	UPROPERTY()
	EObsidianTracedMeshType TracedMeshType = EObsidianTracedMeshType::CharacterMesh;

	UPROPERTY()
	bool bAllowOneHitPerTrace = true;
};

/**
 * Component that gives the functionality to perform advance tracing using sockets on mesh.
 * Should not be used on regular enemies as it might be quite expensive.
 */
UCLASS(BlueprintType)
class OBSIDIAN_API UObsidianAdvancedCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UObsidianAdvancedCombatComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category = "Obsidian Advanced Combat")
	static UObsidianAdvancedCombatComponent* FindAdvancedCombatComponent(const AActor* Owner)
	{
		return (Owner ? Owner->FindComponentByClass<UObsidianAdvancedCombatComponent>() : nullptr);
	}

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Starts the trace on the Component. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void StartTrace(const FObsidianAdvancedTraceParams& TraceParams);

	/** Stops the trace, clearing all variables. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void StopTrace();

	/** Adds actor that will be ignored by the trace. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void AddIgnoredActor(AActor* InIgnoredActor);

	/** Adds array of actors that will be ignored by the trace. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void AddIgnoredActors(TArray<AActor*> InIgnoredActors);

	/** Removes ignored actor. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void RemoveIgnoredActor(AActor* IgnoredActorToRemove);
	
	/** Removes all ignored actors. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void ClearIgnoredActors();

	/** Adds Traced Mesh to the map. It is necessary to state the type of the mesh which can be then paired with proper sockets. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void AddTracedMesh(UPrimitiveComponent* InTracedMesh, const EObsidianTracedMeshType TracedMeshType);

	/** Adds all traced meshes to the map. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void AddTracedMeshes(TMap<EObsidianTracedMeshType, UPrimitiveComponent*> InTracedMeshesMap);

	/** Removes all meshes with a given type. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void RemoveTracedMeshWithType(const EObsidianTracedMeshType TracedMeshType);

	/** Removes all traced meshes. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian Advanced Combat")
	void ClearTracedMeshes();

public:
	/** Trace Channel used in all traces. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian Advanced Combat|Setup")
	TEnumAsByte<ETraceTypeQuery> TraceChannel = UEngineTypes::ConvertToTraceType(Obsidian_TraceChannel_Melee);

	/** Maps Traced Mesh Type to Start and End socket on it. Thanks to that we can choose between different pair of sockets on the same mesh. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian Advanced Combat|Setup")
	TMap<EObsidianTracedMeshType, FObsidianAdvancedCombatSockets> SocketsMap;

	/** Number of traces that will be added within the complex trace. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian Advanced Combat|Setup")
	int32 TraceIntervalCount = 2;

	/** N + 1 number of lines that will be drawn with SemiComplexLineTrace. */
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
	void HandleHit(const bool bHit, const TArray<FHitResult>& HitResults);
	void CalculateNextTracePoint(const int32 Index, const int32 Count, const FVector& Start, const FVector& End, FVector& OutTracePoint);

	void SimpleLineTrace();
	void SemiComplexLineTrace();
	void ComplexLineTrace();
	void SimpleBoxTrace();
	void SimpleCapsuleTrace();
	
private:
	bool bStartTrace = false;

	UPROPERTY()
	FVector CachedStart = FVector::ZeroVector;
	UPROPERTY()
	FVector CachedEnd = FVector::ZeroVector;

	UPROPERTY()
	TObjectPtr<UPrimitiveComponent> CurrentTracedMesh = nullptr;

	UPROPERTY()
	EObsidianTraceType CurrentTraceType = EObsidianTraceType::SimpleLineTrace;

	UPROPERTY()
	FName TraceStartSocketName = "";
	UPROPERTY()
	FName TraceEndSocketName = "";

	UPROPERTY()
	bool bOneHitPerTrace = true;

	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;
};

