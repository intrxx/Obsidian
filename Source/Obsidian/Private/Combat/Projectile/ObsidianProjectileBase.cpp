// Copyright 2026 out of sCope team - intrxx

#include "Combat/Projectile/ObsidianProjectileBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

#include "Combat/Projectile/OProjectileMovementComponent.h"
#include "Obsidian/ObsidianLogCategories.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"


AObsidianProjectileBase::AObsidianProjectileBase(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphereComp"));
	SetRootComponent(SphereComponent);

	SphereComponent->SetCollisionObjectType(Obsidian_ObjectChannel_Projectile);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	SphereComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovementComponent = CreateDefaultSubobject<UOProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->InitialSpeed = 550.f;
	ProjectileMovementComponent->MaxSpeed = 550.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
}

void AObsidianProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	SetReplicateMovement(true);
	
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);

	ProjFlyingSoundComp = UGameplayStatics::SpawnSoundAttached(ProjFlyingSound, GetRootComponent());
	if(ProjFlyingSoundComp)
	{
		ProjFlyingSoundComp->bStopWhenOwnerDestroyed = true;
	}
}

void AObsidianProjectileBase::Destroyed()
{
	if(bAllowMultiHit == false && bServerHit == false && !HasAuthority())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ProjImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ProjImpactEffect, GetActorLocation());
	}

	if (HasAuthority())
	{
		GetWorldTimerManager().ClearAllTimersForObject(this);
	}
	
	Super::Destroyed();
}

void AObsidianProjectileBase::OnSphereOverlap(UPrimitiveComponent* InOverlappedComponent, AActor* InOtherActor,
	UPrimitiveComponent* InOtherComp, int32 InOtherBodyIndex, bool bInFromSweep, const FHitResult& InSweepResult)
{
	if(HasAuthority() && DamageEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser() == InOtherActor)
	{
		return;
	}

	if(ClassToIgnore && InOtherActor->IsA(ClassToIgnore))
	{
		return;
	}

	//TODO(intrxx) As for now this will only work for the server
	if(bServerHit == false && bAllowMultiHit == false || (bAllowMultiHit && CanApplyCosmeticMultiHit(InOtherActor)))
	{
		UGameplayStatics::PlaySoundAtLocation(this, ProjImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ProjImpactEffect, GetActorLocation());
	}
	
	if(HasAuthority())
	{
		if(bDestroyOnHit)
		{
			ApplyProjectileDamageToActor(InOtherActor); // In this can we want to Destroy regardless of if ApplyDamage succeeds 
			Destroy();
			return;
		}
		
		if(bAllowMultiHit)
		{
			if (MultiHitCooldownType == EObsidianMultiHitCooldownType::GlobalMultiHitCooldown && bGlobalMultiHitCanHit)
			{
				if (ApplyProjectileDamageToActor(InOtherActor))
				{
					AlreadyHitActors.AddUnique(TWeakObjectPtr<AActor>(InOtherActor));
					bGlobalMultiHitCanHit = false;
					HandleMultiHitGlobalCooldown();
				}
			}
			else if (MultiHitCooldownType == EObsidianMultiHitCooldownType::PerEnemyMultiHitCooldown)
			{
				bool* CanHitPtr = CanHitPerHitActorMap.Find(TWeakObjectPtr<AActor>(InOtherActor));
				if (CanHitPtr == nullptr || (CanHitPtr && *CanHitPtr))
				{
					if (ApplyProjectileDamageToActor(InOtherActor))
					{
						AlreadyHitActors.AddUnique(TWeakObjectPtr<AActor>(InOtherActor));
						HandleMultiHitPerActorCooldown(InOtherActor);
					}
				}
			}
		}
		else if (AlreadyHitActors.Contains(TWeakObjectPtr<AActor>(InOtherActor)) == false)
		{
			if (ApplyProjectileDamageToActor(InOtherActor))
			{
				AlreadyHitActors.Add(TWeakObjectPtr<AActor>(InOtherActor));
			}
		}
	}
	else
	{		
		bServerHit = true;	
	}
}

bool AObsidianProjectileBase::ApplyProjectileDamageToActor(AActor* InActorToDamage) const
{
	if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActorToDamage))
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		return true;
	}
	return false;
}

void AObsidianProjectileBase::HandleMultiHitGlobalCooldown()
{
	GetWorldTimerManager().SetTimer(GlobalMultiHitCooldownTimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				bGlobalMultiHitCanHit = true;
			}),
			MultiHitCooldown, false);
}

void AObsidianProjectileBase::HandleMultiHitPerActorCooldown(AActor* InForHitActor)
{
	bool& bNewCanHit = CanHitPerHitActorMap.FindOrAdd(TWeakObjectPtr<AActor>(InForHitActor));
	bNewCanHit = false;
	UE_LOG(ObLogCombat, VeryVerbose, TEXT("Applying Hit Cooldown for [%s]"), *GetNameSafe(InForHitActor));

	FTimerHandle PerActorMultiHitTimerHandle;
	GetWorldTimerManager().SetTimer(PerActorMultiHitTimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [this, InForHitActor]()
			{
				if (IsValid(InForHitActor) == false)
				{
					return;
				}
			
				if (bool* CanHitPtr = CanHitPerHitActorMap.Find(TWeakObjectPtr<AActor>(InForHitActor)))
				{
					*CanHitPtr = true;
					UE_LOG(ObLogCombat, VeryVerbose, TEXT("Removing Hit Cooldown for [%s]"), *GetNameSafe(InForHitActor));	
				}
			}),
			MultiHitCooldown, false);
}

bool AObsidianProjectileBase::CanApplyCosmeticMultiHit(AActor* InForHitActor)
{
	if (MultiHitCooldownType == EObsidianMultiHitCooldownType::GlobalMultiHitCooldown)
	{
		return bGlobalMultiHitCanHit;
	}
	
	if (MultiHitCooldownType == EObsidianMultiHitCooldownType::PerEnemyMultiHitCooldown)
	{
		if (InForHitActor == nullptr)
		{
			return false;
		}

		if (const bool* CanHitPtr = CanHitPerHitActorMap.Find(TWeakObjectPtr<AActor>(InForHitActor)))
		{
			return *CanHitPtr;
		}
		
		return true;
	}

	return false;
}


