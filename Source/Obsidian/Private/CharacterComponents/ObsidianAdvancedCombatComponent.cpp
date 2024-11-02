// Copyright 2024 out of sCope team - Michał Ogiński


#include "CharacterComponents/ObsidianAdvancedCombatComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Obsidian/Obsidian.h"

UObsidianAdvancedCombatComponent::UObsidianAdvancedCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	SocketsMap =
	{
		{EObsidianTracedMeshType::ETMT_CharacterMesh, FObsidianAdvancedCombatSockets("AdvancedCombat_Start", "AdvancedCombat_End")},
		{EObsidianTracedMeshType::ETMT_CharacterMesh_LeftHand, FObsidianAdvancedCombatSockets("AdvancedCombat_LeftHandStart", "AdvancedCombat_LeftHandEnd")},
		{EObsidianTracedMeshType::ETMT_CharacterMesh_RightHand, FObsidianAdvancedCombatSockets("AdvancedCombat_RightHandStart", "AdvancedCombat_RightHandEnd")},
		{EObsidianTracedMeshType::ETMT_LeftHandWeaponMesh, FObsidianAdvancedCombatSockets("AdvancedCombat_WeaponStart", "AdvancedCombat_WeaponEnd")},
		{EObsidianTracedMeshType::ETMT_RightHandWeaponMesh, FObsidianAdvancedCombatSockets("AdvancedCombat_WeaponStart", "AdvancedCombat_WeaponEnd")}
	};
}

void UObsidianAdvancedCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bStartTrace)
	{
		TickTrace();
	}
}

void UObsidianAdvancedCombatComponent::TickTrace()
{
	switch(CurrentTraceType)
	{
	case EObsidianTraceType::ETT_SimpleLineTrace:
		SimpleLineTrace();
		break;
	case EObsidianTraceType::ETT_SemiComplexLineTrace:
		SemiComplexLineTrace();
		break;
	case EObsidianTraceType::ETT_ComplexLineTrace:
		ComplexLineTrace();
		break;
	case EObsidianTraceType::ETT_SimpleBoxTrace:
		SimpleBoxTrace();
		break;
	case EObsidianTraceType::ETT_SimpleCapsuleTrace:
		SimpleCapsuleTrace();
		break;
		default:
			break;
	}
}

void UObsidianAdvancedCombatComponent::StartTrace(const FObsidianAdvancedTraceParams& TraceParams)
{
	CurrentTraceType = TraceParams.TraceType;
	CurrentTracedMesh = TracedMeshesMap[TraceParams.TracedMeshType];
	bOneHitPerTrace = TraceParams.bAllowOneHitPerTrace;
	
	const FObsidianAdvancedCombatSockets Sockets = SocketsMap[TraceParams.TracedMeshType];
	TraceStartSocketName = Sockets.StartSocketName;
	TraceEndSocketName = Sockets.EndSocketName;

	if(CurrentTracedMesh == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("CurrentTracedMesh is invalid on [%s] for [%s]."), *GetNameSafe(this), *GetNameSafe(GetOwner()));
		return;
	}

	// Initial positions
	GetSocketsLocationsByMesh(CurrentTracedMesh, /** OUT */ CachedStart, /** OUT */ CachedEnd);

	GetOwner()->GetAttachedActors(IgnoredActors);
	
	bStartTrace = true;
	
	OnAttackTraceStartedDelegate.Broadcast();
	if(bStartInCurrentTick)
	{
		TickTrace();
	}
}

void UObsidianAdvancedCombatComponent::StopTrace()
{
	bStartTrace = false;

	CurrentTracedMesh = nullptr;
	CachedStart = FVector::ZeroVector;
	CachedEnd = FVector::ZeroVector;
	TraceStartSocketName = "";
	TraceEndSocketName = "";

	bOneHitPerTrace = true;
	AlreadyHitActors.Empty();

	OnAttackTraceFinishedDelegate.Broadcast();
}

void UObsidianAdvancedCombatComponent::GetSocketsLocationsByMesh(const UPrimitiveComponent* Mesh,
	FVector& OutStartSocketLoc, FVector& OutEndSocketLoc) const
{
	OutStartSocketLoc = Mesh->GetSocketLocation(TraceStartSocketName);
	OutEndSocketLoc = Mesh->GetSocketLocation(TraceEndSocketName);
}

void UObsidianAdvancedCombatComponent::HandleHit(const bool bHit, const TArray<FHitResult>& HitResults)
{
	if(!bHit)
	{
		return;
	}

	for(const FHitResult& HitResult : HitResults)
	{
		if(bOneHitPerTrace)
		{
			AActor* ActorHit = HitResult.GetActor();
			if(!AlreadyHitActors.Contains(ActorHit))
			{
				AlreadyHitActors.Add(ActorHit);
				OnAttackHitDelegate.Broadcast(HitResult);
			}
		}
		else
		{
			OnAttackHitDelegate.Broadcast(HitResult);
		}
	}
}

void UObsidianAdvancedCombatComponent::CalculateNextTracePoint(const int32 Index, const int32 Count, const FVector& Start,
	const FVector& End, FVector& OutTracePoint)
{
	OutTracePoint = Start + Index * ((End - Start) / Count);
}

void UObsidianAdvancedCombatComponent::SimpleLineTrace()
{
	FVector StartLocation;
	FVector EndLocation;
	GetSocketsLocationsByMesh(CurrentTracedMesh, /* OUT **/ StartLocation, /* OUT **/ EndLocation);
	
	TArray<FHitResult> HitResults;
	
	const bool bHit = UKismetSystemLibrary::LineTraceMulti(this, StartLocation, EndLocation, TraceChannel, bTraceComplex,
		IgnoredActors, bWithDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, HitResults, true, DebugTraceColor,
		DebugHitColor, DebugDuration);
	
	HandleHit(bHit, HitResults);
}

void UObsidianAdvancedCombatComponent::SemiComplexLineTrace()
{
	for(int32 i = 0; i <= MultiLineCount; i++)
	{
		FVector TempStart = CachedStart;
		FVector TempEnd = CachedEnd;

		FVector StartLocation;
		FVector EndLocation;
		
		CalculateNextTracePoint(i, MultiLineCount, TempStart, TempEnd, /* OUT **/ StartLocation);
		
		GetSocketsLocationsByMesh(CurrentTracedMesh, /* OUT **/ TempStart, /* OUT **/ TempEnd);
		CalculateNextTracePoint(i, MultiLineCount, TempStart, TempEnd, /* OUT **/ EndLocation);

		TArray<FHitResult> HitResults;
		
		const bool bHit = UKismetSystemLibrary::LineTraceMulti(this, StartLocation, EndLocation, TraceChannel, bTraceComplex,
			IgnoredActors, bWithDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, HitResults, true, DebugTraceColor,
			DebugHitColor, DebugDuration);

		HandleHit(bHit, HitResults);
	}
	
	GetSocketsLocationsByMesh(CurrentTracedMesh, /* OUT **/ CachedStart, /* OUT **/ CachedEnd);
}

void UObsidianAdvancedCombatComponent::ComplexLineTrace()
{
	SimpleLineTrace();

	for(int32 i = 0; i <= TraceIntervalCount; i++)
	{
		FVector TempStart = CachedStart;
		FVector TempEnd = CachedEnd;

		FVector StartLocation;
		FVector EndLocation;
		
		CalculateNextTracePoint(i, TraceIntervalCount, TempStart, TempEnd, /* OUT **/ StartLocation);
		
		GetSocketsLocationsByMesh(CurrentTracedMesh, /* OUT **/ TempStart, /* OUT **/ TempEnd);
		CalculateNextTracePoint(i, TraceIntervalCount, TempStart, TempEnd, /* OUT **/ EndLocation);

		TArray<FHitResult> HitResults;
		
		const bool bHit = UKismetSystemLibrary::LineTraceMulti(this, StartLocation, EndLocation, TraceChannel, bTraceComplex,
			IgnoredActors, bWithDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, HitResults, true, DebugTraceColor,
			DebugHitColor, DebugDuration);

		HandleHit(bHit, HitResults);
	}
	
	GetSocketsLocationsByMesh(CurrentTracedMesh, /* OUT **/ CachedStart, /* OUT **/ CachedEnd);
}

void UObsidianAdvancedCombatComponent::SimpleBoxTrace()
{
	FVector StartLocation;
	FVector EndLocation;
	GetSocketsLocationsByMesh(CurrentTracedMesh, StartLocation, EndLocation);

	const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), StartLocation);

	TArray<FHitResult> HitResults;
	
	const bool bHit = UKismetSystemLibrary::BoxTraceMulti(this, StartLocation, EndLocation, BoxTraceHalfSize,
		Rotation, TraceChannel, bTraceComplex, IgnoredActors, bWithDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResults, true, DebugTraceColor, DebugHitColor, DebugDuration);

	HandleHit(bHit, HitResults);
}

void UObsidianAdvancedCombatComponent::SimpleCapsuleTrace()
{
	FVector StartLocation;
	FVector EndLocation;
	GetSocketsLocationsByMesh(CurrentTracedMesh, StartLocation, EndLocation);
	
	TArray<FHitResult> HitResults;
	
	const bool bHit = UKismetSystemLibrary::CapsuleTraceMulti(this, StartLocation, EndLocation, CapsuleRadius,
		CapsuleHalfHeight, TraceChannel, bTraceComplex, IgnoredActors, bWithDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResults, true, DebugTraceColor, DebugHitColor, DebugDuration);

	HandleHit(bHit, HitResults);
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

void UObsidianAdvancedCombatComponent::ClearIgnoredActors()
{
	IgnoredActors.Empty();
}

void UObsidianAdvancedCombatComponent::AddTracedMesh(UPrimitiveComponent* InTracedMesh, const EObsidianTracedMeshType TracedMeshType)
{
	if(!IsValid(InTracedMesh))
	{
		return;
	}
	TracedMeshesMap.Add(TracedMeshType, InTracedMesh);
}

void UObsidianAdvancedCombatComponent::AddTracedMeshes(TMap<EObsidianTracedMeshType, UPrimitiveComponent*> InTracedMeshesMap)
{
	for(TTuple<EObsidianTracedMeshType, UPrimitiveComponent*> TracedMeshPair : InTracedMeshesMap)
	{
		if (!IsValid(TracedMeshPair.Value))
		{
			continue;
		}
		TracedMeshesMap.Add(TracedMeshPair.Key, TracedMeshPair.Value);
	}
}

void UObsidianAdvancedCombatComponent::RemoveTracedMeshWithType(const EObsidianTracedMeshType TracedMeshType)
{
	TracedMeshesMap.Remove(TracedMeshType);
}

void UObsidianAdvancedCombatComponent::ClearTracedMeshes()
{
	TracedMeshesMap.Empty();
}

