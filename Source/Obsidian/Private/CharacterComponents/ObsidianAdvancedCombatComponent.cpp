// Copyright 2024 out of sCope team - Michał Ogiński


#include "CharacterComponents/ObsidianAdvancedCombatComponent.h"

UObsidianAdvancedCombatComponent::UObsidianAdvancedCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UObsidianAdvancedCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UObsidianAdvancedCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bStartTrace)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tracing"));
	}
}

void UObsidianAdvancedCombatComponent::StartTrace(EObsidianTraceType TraceType, const EObsidianTracedMeshType TracedMeshType)
{
	CurrentTracedMesh = *TracedMeshesMap.FindKey(TracedMeshType);
	
	bStartTrace = true;
}

void UObsidianAdvancedCombatComponent::StopTrace()
{
	bStartTrace = false;
}

void UObsidianAdvancedCombatComponent::AddIgnoredActor(AActor* InIgnoredActor)
{
	if(!IsValid(InIgnoredActor))
	{
		return;
	}
	IgnoredActors.AddUnique(InIgnoredActor);
}

void UObsidianAdvancedCombatComponent::AddIgnoredActors(TArray<AActor*> InIgnoredActors)
{
	for(AActor* IgnoredActor : InIgnoredActors)
	{
		if(!IsValid(IgnoredActor))
		{
			continue;
		}
		IgnoredActors.AddUnique(IgnoredActor);
	}
}

void UObsidianAdvancedCombatComponent::RemoveIgnoredActor(AActor* IgnoredActorToRemove)
{
	if(!IsValid(IgnoredActorToRemove))
	{
		return;
	}
	IgnoredActors.Remove(IgnoredActorToRemove);
}

void UObsidianAdvancedCombatComponent::AddTracedMesh(UPrimitiveComponent* InTracedMesh, const EObsidianTracedMeshType TracedMeshType)
{
	if(!IsValid(InTracedMesh))
	{
		return;
	}
	TracedMeshesMap.Add(InTracedMesh, TracedMeshType);
}

void UObsidianAdvancedCombatComponent::AddTracedMeshes(TMap<UPrimitiveComponent*, EObsidianTracedMeshType> InTracedMeshesMap)
{
	for(TTuple<UPrimitiveComponent*, EObsidianTracedMeshType> TracedMeshPair : InTracedMeshesMap)
	{
		if(!IsValid(TracedMeshPair.Key))
		{
			continue;
		}
		TracedMeshesMap.Add(TracedMeshPair.Key, TracedMeshPair.Value);
	}
}

void UObsidianAdvancedCombatComponent::RemoveTracedMesh(UPrimitiveComponent* TracedMeshToRemove)
{
	if(!IsValid(TracedMeshToRemove))
	{
		return;
	}
	TracedMeshesMap.Remove(TracedMeshToRemove);
}

void UObsidianAdvancedCombatComponent::ClearTracedMeshes()
{
	TracedMeshesMap.Empty();
}

