// Copyright 2026 out of sCope team - intrxx

#include "CharacterComponents/ObsidianAdvancedCombatComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Obsidian/ObsidianLogCategories.h"


UObsidianAdvancedCombatComponent::UObsidianAdvancedCombatComponent(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	SocketsMap =
	{
		{EObsidianTracedMeshType::CharacterMesh, FObsidianAdvancedCombatSockets("AdvancedCombat_Start", "AdvancedCombat_End")},
		{EObsidianTracedMeshType::CharacterMesh_LeftHand, FObsidianAdvancedCombatSockets("AdvancedCombat_LeftHandStart", "AdvancedCombat_LeftHandEnd")},
		{EObsidianTracedMeshType::CharacterMesh_RightHand, FObsidianAdvancedCombatSockets("AdvancedCombat_RightHandStart", "AdvancedCombat_RightHandEnd")},
		{EObsidianTracedMeshType::LeftHandWeaponMesh, FObsidianAdvancedCombatSockets("AdvancedCombat_WeaponStart", "AdvancedCombat_WeaponEnd")},
		{EObsidianTracedMeshType::RightHandWeaponMesh, FObsidianAdvancedCombatSockets("AdvancedCombat_WeaponStart", "AdvancedCombat_WeaponEnd")}
	};
}

void UObsidianAdvancedCombatComponent::TickComponent(float InDeltaTime, ELevelTick InTickType, FActorComponentTickFunction* InThisTickFunction)
{
	Super::TickComponent(InDeltaTime, InTickType, InThisTickFunction);

	if(bStartTrace)
	{
		TickTrace();
	}
}

void UObsidianAdvancedCombatComponent::TickTrace()
{
	switch(CurrentTraceType)
	{
	case EObsidianTraceType::SimpleLineTrace:
		SimpleLineTrace();
		break;
	case EObsidianTraceType::SemiComplexLineTrace:
		SemiComplexLineTrace();
		break;
	case EObsidianTraceType::ComplexLineTrace:
		ComplexLineTrace();
		break;
	case EObsidianTraceType::SimpleBoxTrace:
		SimpleBoxTrace();
		break;
	case EObsidianTraceType::SimpleCapsuleTrace:
		SimpleCapsuleTrace();
		break;
		default:
			break;
	}
}

void UObsidianAdvancedCombatComponent::StartTrace(const FObsidianAdvancedTraceParams& InTraceParams)
{
	CurrentTraceType = InTraceParams.TraceType;
	CurrentTracedMesh = TracedMeshesMap[InTraceParams.TracedMeshType];
	bOneHitPerTrace = InTraceParams.bAllowOneHitPerTrace;
	
	const FObsidianAdvancedCombatSockets Sockets = SocketsMap[InTraceParams.TracedMeshType];
	TraceStartSocketName = Sockets.StartSocketName;
	TraceEndSocketName = Sockets.EndSocketName;

	if(CurrentTracedMesh == nullptr)
	{
		UE_LOG(ObLogCombat, Error, TEXT("CurrentTracedMesh is invalid on [%s] for [%s]."), *GetNameSafe(this), *GetNameSafe(GetOwner()));
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

void UObsidianAdvancedCombatComponent::GetSocketsLocationsByMesh(const UPrimitiveComponent* InMesh,
	FVector& OutStartSocketLoc, FVector& OutEndSocketLoc) const
{
	OutStartSocketLoc = InMesh->GetSocketLocation(TraceStartSocketName);
	OutEndSocketLoc = InMesh->GetSocketLocation(TraceEndSocketName);
}

void UObsidianAdvancedCombatComponent::HandleHit(const bool bInHit, const TArray<FHitResult>& InHitResults)
{
	if(!bInHit)
	{
		return;
	}

	for(const FHitResult& HitResult : InHitResults)
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

void UObsidianAdvancedCombatComponent::CalculateNextTracePoint(const int32 InIndex, const int32 InCount, const FVector& InStart,
	const FVector& InEnd, FVector& OutTracePoint)
{
	OutTracePoint = InStart + InIndex * ((InEnd - InStart) / InCount);
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

void UObsidianAdvancedCombatComponent::RemoveIgnoredActor(AActor* InIgnoredActorToRemove)
{
	if(!IsValid(InIgnoredActorToRemove))
	{
		return;
	}
	IgnoredActors.Remove(InIgnoredActorToRemove);
}

void UObsidianAdvancedCombatComponent::ClearIgnoredActors()
{
	IgnoredActors.Empty();
}

void UObsidianAdvancedCombatComponent::AddTracedMesh(UPrimitiveComponent* InTracedMesh, const EObsidianTracedMeshType InTracedMeshType)
{
	if(!IsValid(InTracedMesh))
	{
		return;
	}
	TracedMeshesMap.Add(InTracedMeshType, InTracedMesh);
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

void UObsidianAdvancedCombatComponent::RemoveTracedMeshWithType(const EObsidianTracedMeshType InTracedMeshType)
{
	TracedMeshesMap.Remove(InTracedMeshType);
}

void UObsidianAdvancedCombatComponent::ClearTracedMeshes()
{
	TracedMeshesMap.Empty();
}

