// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Abilities/OGameplayAbility_ProjectileSpell.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Combat/ObsidianCombatInterface.h"
#include "Combat/Projectile/ObsidianProjectile.h"
#include "Obsidian/Obsidian.h"

UOGameplayAbility_ProjectileSpell::UOGameplayAbility_ProjectileSpell()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UOGameplayAbility_ProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UOGameplayAbility_ProjectileSpell::SpawnProjectile(const FVector& TargetLocation)
{
	const bool bHasAuthority = GetAvatarActorFromActorInfo()->HasAuthority();
	if(!bHasAuthority)
	{
		return;
	}
	
	const FVector StartLocation = GetSpawnLocation();
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

	for(TTuple<FGameplayTag, FObsidianAbilityDamageRange>& Pair : DamageTypeMap)
	{
		const float Damage = Pair.Value.RollForDamageNumberAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, Damage);
	}
	
	Projectile->SetProjectileDamageSpecHandle(SpecHandle);
	
	Projectile->FinishSpawning(SpawnTransform);
}

//TODO Evaluate this, look at ObsidianCharacterBase FVector AObsidianCharacterBase::GetAbilitySocketLocationForTag_Implementation(const FGameplayTag& Tag)
FVector UOGameplayAbility_ProjectileSpell::GetSpawnLocation() const
{
	AActor* OwningActor = GetAvatarActorFromActorInfo();
	if(OwningActor == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("Owning Actor is nullptr on [%s]"), *GetNameSafe(this));
		return FVector::ZeroVector;
	}
	
	FVector SpawnLocation = OwningActor->GetActorLocation();
	
	switch(AbilitySpawnLocation)
	{
	case EObsidianAbilitySpawnLocation::ASL_DefaultLocation:
		SpawnLocation = IObsidianCombatInterface::Execute_GetAbilityBetweenHandsSocketLocation(OwningActor);
		break;
	case EObsidianAbilitySpawnLocation::ASL_LeftHand:
		SpawnLocation = IObsidianCombatInterface::Execute_GetAbilitySocketLocationFromLeftHand(OwningActor);
		break;
	case EObsidianAbilitySpawnLocation::ASL_RightHand:
		SpawnLocation = IObsidianCombatInterface::Execute_GetAbilitySocketLocationFromRightHand(OwningActor);
		break;
	case EObsidianAbilitySpawnLocation::ASL_LeftHandEquipment:
		SpawnLocation = IObsidianCombatInterface::Execute_GetAbilitySocketLocationFromLHWeapon(OwningActor);
		break;
	case EObsidianAbilitySpawnLocation::ASL_RightHandEquipment:
		SpawnLocation = IObsidianCombatInterface::Execute_GetAbilitySocketLocationFromRHWeapon(OwningActor);
		break;
	default:
		break;
	}

	return SpawnLocation;
}
