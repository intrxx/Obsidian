// Copyright 2024 out of sCope team - Michał Ogiński

#include "Characters/Enemies/ObsidianEnemy.h"

// ~ Core
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// ~ Project
#include "AbilitySystem/Data/ObsidianAbilitySet.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianEnemyAttributeSet.h"
#include "CharacterComponents/ObsidianPawnExtensionComponent.h"
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "Characters/ObsidianPawnData.h"
#include "CharacterComponents/ObsidianCharacterMovementComponent.h"
#include "CharacterComponents/ObsidianEnemyOverlayBarComponent.h"
#include "Characters/ObsidianDummyMeshActor.h"
#include "InventoryItems/ItemDrop/ObsidianItemDropComponent.h"

AObsidianEnemy::AObsidianEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetCollisionResponseToChannel(Obsidian_TraceChannel_PlayerCursorTrace, ECR_Block);
	MeshComp->SetCollisionResponseToChannel(Obsidian_ObjectChannel_Projectile, ECR_Overlap);
	MeshComp->SetCustomDepthStencilValue(ObsidianHighlight::Red);
	MeshComp->SetRenderCustomDepth(false);
	
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionResponseToChannel(Obsidian_ObjectChannel_Projectile, ECR_Overlap);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	UObsidianCharacterMovementComponent* ObsidianMovementComp = CastChecked<UObsidianCharacterMovementComponent>(GetCharacterMovement());
	ObsidianMovementComp->bUseControllerDesiredRotation = true;
	ObsidianMovementComp->RotationRate = FRotator(0.f, 360.f, 0.f);
	
	ObsidianAbilitySystemComponent = CreateDefaultSubobject<UObsidianAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ObsidianAbilitySystemComponent->SetIsReplicated(true);
	ObsidianAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	EnemyAttributeSet = CreateDefaultSubobject<UObsidianEnemyAttributeSet>(TEXT("EnemyAttributeSet"));
	
	EnemyAttributesComponent = CreateDefaultSubobject<UObsidianEnemyAttributesComponent>(TEXT("EnemyAttributesComponent"));

	ItemDropComponent = CreateDefaultSubobject<UObsidianItemDropComponent>(TEXT("ItemDropComponent"));
	
	EnemyOverlayBarComponent = CreateDefaultSubobject<UObsidianEnemyOverlayBarComponent>(TEXT("EnemyOverlayBarComponent"));
	
	// Identifies this class as Enemy character
	Tags.Emplace(ObsidianActorTags::Enemy);
}

AActor* AObsidianEnemy::GetHighlightAvatarActor()
{
	return this;
}

void AObsidianEnemy::StartHighlight()
{
	if(USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetRenderCustomDepth(true);
	}
}

void AObsidianEnemy::StopHighlight()
{
	if(USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetRenderCustomDepth(false);
	}
}

bool AObsidianEnemy::IsHighlighted() const
{
	if(const USkeletalMeshComponent* MeshComp = GetMesh())
	{
		return MeshComp->bRenderCustomDepth;
	}
	return false;
}

void AObsidianEnemy::BeginPlay()
{
	Super::BeginPlay();

	check(PawnExtComp);
	PawnExtComp->InitializeAbilitySystem(ObsidianAbilitySystemComponent, this);
}

void AObsidianEnemy::OnAbilitySystemInitialized()
{
	Super::OnAbilitySystemInitialized();

	UObsidianAbilitySystemComponent* ObsidianASC = GetObsidianAbilitySystemComponent();
	check(ObsidianASC);
	
	EnemyAttributesComponent->InitializeWithAbilitySystem(ObsidianASC, this);
	EnemyAttributesComponent->SetEnemyName(EnemyName);

	EnemyOverlayBarComponent->InitializeOverlayBarComponent(ObsidianASC, EnemyAttributesComponent);
	
	if(const UObsidianPawnExtensionComponent* PawnExt = UObsidianPawnExtensionComponent::FindPawnExtComponent(this))
	{
		if(const UObsidianPawnData* PawnData = PawnExt->GetPawnData())
		{
			for(const UObsidianAbilitySet* AbilitySet : PawnData->AbilitySets)
			{
				AbilitySet->GiveToAbilitySystem(ObsidianASC, nullptr, EnemyLevel, this);
			}
		}
	}

	ObsidianASC->BindToOnEffectAppliedDelegate();
}

void AObsidianEnemy::OnAbilitySystemUninitialized()
{
	Super::OnAbilitySystemUninitialized();

	EnemyAttributesComponent->UninitializeFromAbilitySystem();
	EnemyOverlayBarComponent->UninitializeOverlayBarComponent();
}

void AObsidianEnemy::OnDeathStarted(AActor* OwningActor)
{
	Super::OnDeathStarted(OwningActor);

	if(HasAuthority() && IsValid(Controller))
	{
		Controller->UnPossess();
	}

	// If the enemy is highlighted stop it as it looks weird when the stencil disappears later.
	if(IsHighlighted())
	{
		StopHighlight();
	}
	
	if(!DeathMontages.IsEmpty())
	{
		const float DeathMontagesLength = DeathMontages.Num();
		const float RandomIndex = FMath::RandRange(0.f, DeathMontagesLength - 1);

		if(bShouldSpawnDummyMesh)
		{
			float AnimLength = PlayAnimMontage(DeathMontages[RandomIndex]);
			
			// This is a hack to copy the mesh before the anim montage actually ends.
			AnimLength -= 0.2f;

			USkeletalMeshComponent* MeshComp = GetMesh();
			FTimerHandle DeathMontageFinishedHandle;
			GetWorld()->GetTimerManager().SetTimer(DeathMontageFinishedHandle, FTimerDelegate::CreateWeakLambda(this, [MeshComp, this]()
				{
					if(UWorld* World = GetWorld())
					{
						AObsidianDummyMeshActor* Dummy = World->SpawnActorDeferred<AObsidianDummyMeshActor>(AObsidianDummyMeshActor::StaticClass(), GetActorTransform());
						Dummy->SetupDummyMeshActor(MeshComp, DummyMeshLifeSpan);

						Dummy->FinishSpawning(GetActorTransform());
					}
				
				}), AnimLength, false);
		}
		else
		{
			PlayAnimMontage(DeathMontages[RandomIndex]);
		}
	}

	if (ItemDropComponent)
	{
		ItemDropComponent->DropItems();
	}
}

void AObsidianEnemy::OnDeathFinished(AActor* OwningActor)
{
	Super::OnDeathFinished(OwningActor);
}

bool AObsidianEnemy::IsDeadOrDying_Implementation() const
{
	if(EnemyAttributesComponent)
	{
		return EnemyAttributesComponent->IsDeadOrDying();
	}
	return false;
}

AActor* AObsidianEnemy::GetAvatarActor_Implementation()
{
	return this;
}

int32 AObsidianEnemy::GetCharacterLevel()
{
	return EnemyLevel;
}

void AObsidianEnemy::SetCombatTarget_Implementation(AActor* InTarget)
{
	CombatTarget = InTarget;
}

AActor* AObsidianEnemy::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

