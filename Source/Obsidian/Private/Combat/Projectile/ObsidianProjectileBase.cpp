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
	if(!bServerHit && !HasAuthority())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ProjImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ProjImpactEffect, GetActorLocation());
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

	if (OverlappedComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit Comp: [%s]"), *GetNameSafe(OverlappedComponent));
	}

	if(!bServerHit)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ProjImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ProjImpactEffect, GetActorLocation());
	}
	
	if(HasAuthority())
	{
		if(bDestroyOnHit)
		{
			ApplyProjectileDamageToActor(OtherActor);
			Destroy();	
		}
		else
		{
			const bool bAlreadyDamaged = AlreadyHitActors.Contains(OtherActor);
			if(bAllowMultiHit || bAlreadyDamaged == false)
			{
				// Leaving it here despite bAllowMultiHit==true as this can be useful for something else
				AlreadyHitActors.Add(OtherActor);
				ApplyProjectileDamageToActor(OtherActor);
			}
		}
	}
	else
	{		
		bServerHit = true;	
	}
}

void AObsidianProjectileBase::ApplyProjectileDamageToActor(AActor* ActorToDamage) const
{
	if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActorToDamage))
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		UE_LOG(LogTemp, Display, TEXT("Applying Projectile Damage"));
	}
}


