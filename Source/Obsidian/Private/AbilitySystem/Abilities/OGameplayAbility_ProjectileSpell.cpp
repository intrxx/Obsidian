// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Abilities/OGameplayAbility_ProjectileSpell.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Combat/ObsidianCombatInterface.h"
#include "Combat/Projectile/ObsidianProjectile.h"
#include "Obsidian/ObsidianGameModule.h"
#include "Obsidian/ObsidianMacros.h"

UOGameplayAbility_ProjectileSpell::UOGameplayAbility_ProjectileSpell()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UOGameplayAbility_ProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UOGameplayAbility_ProjectileSpell::SpawnProjectile(const FVector& SpawnLocation, const FVector& TargetLocation, const bool bWithDebug)
{
	const bool bHasAuthority = GetAvatarActorFromActorInfo()->HasAuthority();
	if(!bHasAuthority)
	{
		return;
	}

	if(bWithDebug)
	{
		DEBUG_SPHERE(TargetLocation);
		DEBUG_LINE(SpawnLocation, TargetLocation);
	}
	
	const FVector StartLocation = SpawnLocation;
	FRotator Rotation = (TargetLocation - StartLocation).Rotation();

	// This can actually cause a bug on client, when the socket location would be different on the firing client.
	// This will need to be addressed later.
	Rotation.Pitch = 0.f;
	
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(StartLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion()); 
	
	AObsidianProjectile* Projectile = GetWorld()->SpawnActorDeferred<AObsidianProjectile>(ProjectileClass, SpawnTransform,
		GetOwningActorFromActorInfo(), Cast<APawn>(GetOwningActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
	const UAbilitySystemComponent* OwningASC = GetAbilitySystemComponentFromActorInfo();
	
	FGameplayEffectContextHandle ContextHandle = OwningASC->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(Projectile);
	
	const FGameplayEffectSpecHandle SpecHandle = OwningASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), ContextHandle);

	for(TTuple<FGameplayTag, FObsidianAbilityDamageRange>& Pair : ProjectileDamageTypeMap)
	{
		const float Damage = Pair.Value.RollForDamageNumberAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, Damage);
	}
	
	Projectile->SetProjectileDamageSpecHandle(SpecHandle);
	Projectile->FinishSpawning(SpawnTransform);
}

