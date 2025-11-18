// Copyright 2024 out of sCope team - Michał Ogiński

#include "AbilitySystem/Abilities/Specific/ObsidianGA_Melee_Slash.h"

#include <NiagaraFunctionLibrary.h>
#include <AbilitySystemBlueprintLibrary.h>
#include <AbilitySystemComponent.h>

#include "Characters/Enemies/ObsidianEnemy.h"
#include "Characters/Heroes/ObsidianHero.h"

void UObsidianGA_Melee_Slash::FireSlash()
{
	AObsidianHero* ObsidianHero = Cast<AObsidianHero>(GetAvatarActorFromActorInfo());
	if (ObsidianHero == nullptr)
	{
		return;
	}
	
	UWorld* World = ObsidianHero->GetWorld();
	if (!World)
	{
		return;
	}

	const FVector OwnerForwardVec = ObsidianHero->GetActorForwardVector();
	const FVector OwnerRightVec = ObsidianHero->GetActorRightVector();
	const FVector BaseLocation = ObsidianHero->GetActorLocation() + OwnerForwardVec * SlashStartOffset;
	const FVector SweepStart = BaseLocation + OwnerRightVec * (SlashWidth * 0.5f);
	const FVector SweepEnd = BaseLocation - OwnerRightVec * (SlashWidth * 0.5f);
	const FRotator Rotation = FRotator(-90.0f, ObsidianHero->GetActorRotation().Yaw, 0.0f);
	
	if (SlashNiagaraSystem)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, SlashNiagaraSystem, SweepStart, Rotation, FVector(1.f),
			true, true, ENCPoolMethod::None, true);
	}
	
	const float CapsuleHalfHeight = SlashHeight * 0.5;
	const FCollisionShape SweepShape = FCollisionShape::MakeCapsule(SlashTraceCapsuleRadius, CapsuleHalfHeight);
 
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(ObsidianHero);
	
	const FQuat CapsuleQuat = Rotation.Quaternion();
	
	TArray<FHitResult> Hits;
	bool bHit = World->SweepMultiByChannel(Hits, SweepStart, SweepEnd, CapsuleQuat, ECC_Pawn, SweepShape, Params);
	
#if !UE_BUILD_SHIPPING
	if (bDrawSlashDebug)
	{
		DrawDebugCapsule(World, SweepStart, CapsuleHalfHeight, SlashTraceCapsuleRadius, CapsuleQuat, FColor::Red, false, 1.0f);
		DrawDebugCapsule(World, SweepEnd, CapsuleHalfHeight, SlashTraceCapsuleRadius, CapsuleQuat, FColor::Blue, false, 1.0f);
		DrawDebugLine(World, SweepStart, SweepEnd, FColor::Green, false, 1.0f, 0, 2.0f);
	}
#endif

	if (bHit == false)
	{
		return;
	}

	const FGameplayEffectSpecHandle SlashSpecHandle = MakeMeleeDamageSpec(ObsidianHero);
	
	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor != ObsidianHero && HitActor->IsA(AObsidianEnemy::StaticClass()))
		{
			if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor))
			{
				UE_LOG(LogTemp, Log, TEXT("Slash hit valid actor: %s"), *HitActor->GetName());
				TargetASC->ApplyGameplayEffectSpecToSelf(*SlashSpecHandle.Data.Get());
			}
		}
	}
}
