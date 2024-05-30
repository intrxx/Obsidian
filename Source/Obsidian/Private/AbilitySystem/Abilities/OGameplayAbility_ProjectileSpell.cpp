// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Abilities/OGameplayAbility_ProjectileSpell.h"
#include "Combat/ObsidianCombatInterface.h"
#include "GameFramework/Character.h"
#include "Combat/Projectile/ObsidianProjectile.h"

void UOGameplayAbility_ProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                                        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	const bool bHasAuthority = HasAuthority(&ActivationInfo);
	if(!bHasAuthority)
	{
		return;
	}
	
	if(AController* Controller = CastChecked<ACharacter>(GetAvatarActorFromActorInfo())->GetController())
	{
		Controller->StopMovement();
	}
}

void UOGameplayAbility_ProjectileSpell::SpawnProjectile(const FVector& TargetLocation)
{
	const FVector StartLocation = GetSpawnLocation();
	FRotator Rotation = (TargetLocation - StartLocation).Rotation();
	Rotation.Pitch = 0.f;
	
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(StartLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());
	
	AObsidianProjectile* Projectile = GetWorld()->SpawnActorDeferred<AObsidianProjectile>(ProjectileClass, SpawnTransform,
		GetOwningActorFromActorInfo(), Cast<APawn>(GetOwningActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	//TODO: Give a projectile a Gameplay Effect Spec for causing Damage
	
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
