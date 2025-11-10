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
#include "Characters/Player/ObsidianPlayerState.h"
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

	if(!bServerHit)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ProjImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ProjImpactEffect, GetActorLocation());
	}
	
	if(HasAuthority())
	{
		if(bDestroyOnHit)
		{
			if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
			}
			
			Destroy();	
		}
		else
		{
			if(!AlreadyHitActors.Contains(OtherActor))
			{
				AlreadyHitActors.Add(OtherActor);
				
				if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
				{
					TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
				}
			}
		}
	}
	else
	{		
		bServerHit = true;	
	}
}


