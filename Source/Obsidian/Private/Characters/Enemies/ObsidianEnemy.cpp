// Copyright 2024 out of sCope team - Michał Ogiński

#include "Characters/Enemies/ObsidianEnemy.h"
#include "AbilitySystem/Data/ObsidianAbilitySet.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianEnemyAttributeSet.h"
#include "CharacterComponents/ObsidianPawnExtensionComponent.h"
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "Characters/ObsidianPawnData.h"
#include "Components/CapsuleComponent.h"
#include "CharacterComponents/ObsidianCharacterMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AObsidianEnemy::AObsidianEnemy(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetCollisionResponseToChannel(Obsidian_TraceChannel_PlayerCursorTrace, ECR_Block);
	MeshComp->SetCollisionResponseToChannel(Obsidian_ObjectChannel_Projectile, ECR_Overlap);
	MeshComp->SetCustomDepthStencilValue(ObsidianHighlight::Red);
	MeshComp->SetRenderCustomDepth(false);
	
	ObsidianAbilitySystemComponent = CreateDefaultSubobject<UObsidianAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	ObsidianAbilitySystemComponent->SetIsReplicated(true);
	ObsidianAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	EnemyAttributeSet = CreateDefaultSubobject<UObsidianEnemyAttributeSet>(TEXT("EnemyAttributeSet"));
	
	EnemyAttributesComponent = CreateDefaultSubobject<UObsidianEnemyAttributesComponent>(TEXT("EnemyAttributesComponent"));
	
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionResponseToChannel(Obsidian_ObjectChannel_Projectile, ECR_Overlap);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	UObsidianCharacterMovementComponent* ObsidianMovementComp = CastChecked<UObsidianCharacterMovementComponent>(GetCharacterMovement());
	ObsidianMovementComp->bUseControllerDesiredRotation = true;
	ObsidianMovementComp->RotationRate = FRotator(0.f, 360.f, 0.f);;

	// Identifies this class as Enemy character
	Tags.Emplace(ObsidianActorTags::Enemy);
}

AActor* AObsidianEnemy::GetHighlightAvatarActor()
{
	return this;
}

void AObsidianEnemy::StartHighlight()
{
	GetMesh()->SetRenderCustomDepth(true);
}

void AObsidianEnemy::StopHighlight()
{
	GetMesh()->SetRenderCustomDepth(false);
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
	
	EnemyAttributesComponent->InitializeWithAbilitySystem(ObsidianASC);
	EnemyAttributesComponent->SetEnemyName(EnemyName);
	
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
}

void AObsidianEnemy::OnAbilitySystemUninitialized()
{
	Super::OnAbilitySystemUninitialized();

	EnemyAttributesComponent->UninitializeFromAbilitySystem();
}

void AObsidianEnemy::OnDeathStarted(AActor* OwningActor)
{
	Super::OnDeathStarted(OwningActor);

	if(HasAuthority() && IsValid(Controller))
	{
		Controller->UnPossess();
	}

	if(!DeathMontages.IsEmpty())
	{
		const float DeathMontagesLength = DeathMontages.Num();
		const float RandomIndex = FMath::RandRange(0.f, DeathMontagesLength - 1);
		
		const float AnimLength = PlayAnimMontage(DeathMontages[RandomIndex]);

		USkeletalMeshComponent* MeshComp = GetMesh();
		FTimerHandle DeathMontageFinishedHandle;
		GetWorld()->GetTimerManager().SetTimer(DeathMontageFinishedHandle, FTimerDelegate::CreateWeakLambda(this, [MeshComp]()
		{
			UE_LOG(LogTemp, Warning, TEXT("Replace the mesh with dummy mesh, hide and destroy the current one"));
			
		}), AnimLength, false);
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

