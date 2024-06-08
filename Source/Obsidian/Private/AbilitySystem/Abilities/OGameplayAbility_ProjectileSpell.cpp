// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Abilities/OGameplayAbility_ProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Combat/ObsidianCombatInterface.h"
#include "Combat/Projectile/ObsidianProjectile.h"
#include "Obsidian/ObsidianGameplayTags.h"

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
	Rotation.Pitch = 0.f;
	
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(StartLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());
	
	AObsidianProjectile* Projectile = GetWorld()->SpawnActorDeferred<AObsidianProjectile>(ProjectileClass, SpawnTransform,
		GetOwningActorFromActorInfo(), Cast<APawn>(GetOwningActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
	const UAbilitySystemComponent* OwningASC = GetAbilitySystemComponentFromActorInfo();
	const FGameplayEffectSpecHandle SpecHandle = OwningASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), OwningASC->MakeEffectContext());

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, ObsidianGameplayTags::Ability_Damage, 20.f);
	
	Projectile->SetProjectileDamageSpecHandle(SpecHandle);
	
	Projectile->FinishSpawning(SpawnTransform);
}

FVector UOGameplayAbility_ProjectileSpell::GetSpawnLocation() const
{
	FVector SpawnLocation = FVector::ZeroVector;
	
	IObsidianCombatInterface* CombatInterface = Cast<IObsidianCombatInterface>(GetAvatarActorFromActorInfo());
	if(CombatInterface == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Combat Interface is nullptr on [%s] for Avatar Actor [%s]"), *GetNameSafe(this), *GetNameSafe(GetAvatarActorFromActorInfo()));
		return SpawnLocation;
	}
	
	switch(AbilitySpawnLocation)
	{
	case EObsidianAbilitySpawnLocation::ASL_DefaultLocation:
		SpawnLocation = CombatInterface->GetAbilityDefaultLocation();
		break;
	case EObsidianAbilitySpawnLocation::ASL_LeftHand:
		SpawnLocation = CombatInterface->GetAbilitySocketLocationFromLeftHand();
		break;
	case EObsidianAbilitySpawnLocation::ASL_RightHand:
		SpawnLocation = CombatInterface->GetAbilitySocketLocationFromRightHand();
		break;
	case EObsidianAbilitySpawnLocation::ASL_LeftHandEquipment:
		SpawnLocation = CombatInterface->GetAbilitySocketLocationFromLHWeapon();
		break;
	case EObsidianAbilitySpawnLocation::ASL_RightHandEquipment:
		SpawnLocation = CombatInterface->GetAbilitySocketLocationFromRHWeapon();
		break;
	default:
		break;
	}

	return SpawnLocation;
}
