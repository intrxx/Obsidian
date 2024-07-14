// Copyright 2024 out of sCope team - Michał Ogiński

#include "Combat/Projectile/ObsidianProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Combat/Projectile/OProjectileMovementComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ObsidianTypes/ObsidianChannels.h"

AObsidianProjectile::AObsidianProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
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

	ObsidianProjectileMovementComponent = CreateDefaultSubobject<UOProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ObsidianProjectileMovementComponent->SetIsReplicated(true);
	ObsidianProjectileMovementComponent->InitialSpeed = 550.f;
	ObsidianProjectileMovementComponent->MaxSpeed = 550.f;
	ObsidianProjectileMovementComponent->ProjectileGravityScale = 0.f;
}

void AObsidianProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(ProjectileLifeSpan);
	SetReplicateMovement(true);
	
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);

	ProjFlyingSoundComp = UGameplayStatics::SpawnSoundAttached(ProjFlyingSound, GetRootComponent());
	if(ProjFlyingSoundComp)
	{
		ProjFlyingSoundComp->bStopWhenOwnerDestroyed = true;
	}
}

void AObsidianProjectile::Destroyed()
{
	if(!bServerHit && !HasAuthority())
	{
		UGameplayStatics::PlaySoundAtLocation(this, ProjImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ProjImpactEffect, GetActorLocation());
	}
	
	Super::Destroyed();
}

void AObsidianProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(HasAuthority() && DamageEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser() == OtherActor)
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
		if(UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
		
		Destroy();		
	}
	else
	{		
		bServerHit = true;	
	}
}


