// Copyright 2024 out of sCope team - Michał Ogiński

#include "Combat/Projectile/ObsidianProjectileBase.h"

// ~ Core
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// ~ Project
#include "Combat/Projectile/OProjectileMovementComponent.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"

AObsidianProjectileBase::AObsidianProjectileBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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

void AObsidianProjectileBase::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(HasAuthority() && DamageEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser() == OtherActor)
	{
		return;
	}

	if(ClassToIgnore && OtherActor->IsA(ClassToIgnore))
	{
		return;
	}

	//TODO(intrxx) As for now this will only work for the server
	if(bServerHit == false && bAllowMultiHit == false || (bAllowMultiHit && CanApplyCosmeticMultiHit(OtherActor)))
	{
		UGameplayStatics::PlaySoundAtLocation(this, ProjImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ProjImpactEffect, GetActorLocation());
	}
	
	if(HasAuthority())
	{
		if(bDestroyOnHit)
		{
			ApplyProjectileDamageToActor(OtherActor); // In this can we want to Destroy regardless of if ApplyDamage succeeds 
			Destroy();
			return;
		}
		
		if(bAllowMultiHit)
		{
			if (MultiHitCooldownType == EObsidianMultiHitCooldownType::GlobalMultiHitCooldown && bGlobalMultiHitCanHit)
			{
				if (ApplyProjectileDamageToActor(OtherActor))
				{
					AlreadyHitActors.AddUnique(TWeakObjectPtr<AActor>(OtherActor));
					bGlobalMultiHitCanHit = false;
					HandleMultiHitGlobalCooldown();
				}
			}
			else if (MultiHitCooldownType == EObsidianMultiHitCooldownType::PerEnemyMultiHitCooldown)
			{
				bool* CanHitPtr = CanHitPerHitActorMap.Find(TWeakObjectPtr<AActor>(OtherActor));
				if (CanHitPtr == nullptr || (CanHitPtr && *CanHitPtr))
				{
					if (ApplyProjectileDamageToActor(OtherActor))
					{
						AlreadyHitActors.AddUnique(TWeakObjectPtr<AActor>(OtherActor));
						HandleMultiHitPerActorCooldown(OtherActor);
					}
				}
			}
		}
		else if (AlreadyHitActors.Contains(TWeakObjectPtr<AActor>(OtherActor)) == false)
		{
			if (ApplyProjectileDamageToActor(OtherActor))
			{
				AlreadyHitActors.Add(TWeakObjectPtr<AActor>(OtherActor));
			}
		}
	}
	else
	{		
		bServerHit = true;	
	}
}

bool AObsidianProjectileBase::ApplyProjectileDamageToActor(AActor* ActorToDamage) const
{
	if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActorToDamage))
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

void AObsidianProjectileBase::HandleMultiHitPerActorCooldown(AActor* ForHitActor)
{
	bool& bNewCanHit = CanHitPerHitActorMap.FindOrAdd(TWeakObjectPtr<AActor>(ForHitActor));
	bNewCanHit = false;
	UE_LOG(LogTemp, Warning, TEXT("Applying Hit Cooldown for [%s]"), *GetNameSafe(ForHitActor));

	FTimerHandle PerActorMultiHitTimerHandle;
	GetWorldTimerManager().SetTimer(PerActorMultiHitTimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [this, ForHitActor]()
			{
				if (IsValid(ForHitActor) == false)
				{
					return;
				}
			
				if (bool* CanHitPtr = CanHitPerHitActorMap.Find(TWeakObjectPtr<AActor>(ForHitActor)))
				{
					*CanHitPtr = true;
					UE_LOG(LogTemp, Warning, TEXT("Removing Hit Cooldown for [%s]"), *GetNameSafe(ForHitActor));	
				}
			}),
			MultiHitCooldown, false);
}

bool AObsidianProjectileBase::CanApplyCosmeticMultiHit(AActor* ForHitActor)
{
	if (MultiHitCooldownType == EObsidianMultiHitCooldownType::GlobalMultiHitCooldown)
	{
		return bGlobalMultiHitCanHit;
	}
	
	if (MultiHitCooldownType == EObsidianMultiHitCooldownType::PerEnemyMultiHitCooldown)
	{
		if (ForHitActor == nullptr)
		{
			return false;
		}

		if (const bool* CanHitPtr = CanHitPerHitActorMap.Find(TWeakObjectPtr<AActor>(ForHitActor)))
		{
			return *CanHitPtr;
		}
		
		return true;
	}

	return false;
}


