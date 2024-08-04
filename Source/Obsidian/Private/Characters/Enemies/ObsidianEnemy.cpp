// Copyright 2024 out of sCope team - Michał Ogiński

#include "Characters/Enemies/ObsidianEnemy.h"
#include "AbilitySystem/Data/ObsidianAbilitySet.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianEnemyAttributeSet.h"
#include "CharacterComponents/ObsidianPawnExtensionComponent.h"
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"
#include "ObsidianTypes/ObsidianChannels.h"
#include "Characters/ObsidianPawnData.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "GameplayEffectTypes.h"
#include "AI/ObsidianAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CharacterComponents/ObsidianCharacterMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ObsidianTypes/ObsidianActorTags.h"
#include "ObsidianTypes/ObsidianStencilValues.h"
#include "UI/ObsidianWidgetBase.h"
#include "UI/ProgressBars/ObsidianRegularEnemyHealthBar.h"

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
	
	CommonAttributeSet = CreateDefaultSubobject<UObsidianCommonAttributeSet>(TEXT("CommonAttributeSet"));
	EnemyAttributeSet = CreateDefaultSubobject<UObsidianEnemyAttributeSet>(TEXT("EnemyAttributeSet"));
	
	EnemyAttributesComponent = CreateDefaultSubobject<UObsidianEnemyAttributesComponent>(TEXT("EnemyAttributesComponent"));
	EnemyAttributesComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	EnemyAttributesComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

	HealthBarWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComp->SetupAttachment(GetRootComponent());
	
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

void AObsidianEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(!HasAuthority())
	{
		return;
	}
	ObsidianAIController = Cast<AObsidianAIController>(NewController);
	ObsidianAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	ObsidianAIController->RunBehaviorTree(BehaviorTree);

	UBlackboardComponent* BlackboardComponent = ObsidianAIController->GetBlackboardComponent();
	BlackboardComponent->SetValueAsBool(FName("bHitReacting"), false);
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
	
	CreateHealthBarWidget();
	
	EnemyAttributesComponent->InitializeWithAbilitySystem(ObsidianASC);
	
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
	
	ObsidianASC->RegisterGameplayTagEvent(ObsidianGameplayTags::Effect_HitReact,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::HitReactTagChanged);
}

void AObsidianEnemy::OnAbilitySystemUninitialized()
{
	Super::OnAbilitySystemUninitialized();

	EnemyAttributesComponent->UninitializeFromAbilitySystem();
}

void AObsidianEnemy::OnDeathStarted(AActor* OwningActor)
{
	Super::OnDeathStarted(OwningActor);

	if(HealthBarWidgetComp)
	{
		HealthBarWidgetComp->DestroyComponent();
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

void AObsidianEnemy::CreateHealthBarWidget() const
{
	if(UObsidianRegularEnemyHealthBar* HealthBarWidget = Cast<UObsidianRegularEnemyHealthBar>(HealthBarWidgetComp->GetUserWidgetObject()))
	{
		HealthBarWidget->SetWidgetController(EnemyAttributesComponent);
	}
	else
	{
		HealthBarWidgetComp->InitWidget();
		HealthBarWidget = Cast<UObsidianRegularEnemyHealthBar>(HealthBarWidgetComp->GetUserWidgetObject());
		HealthBarWidget->SetWidgetController(EnemyAttributesComponent);
	}
}

void AObsidianEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;

	if(HasAuthority() && ObsidianAIController)
	{
		if(UBlackboardComponent* BlackboardComponent = ObsidianAIController->GetBlackboardComponent())
		{
			BlackboardComponent->SetValueAsBool(FName("bHitReacting"), bHitReacting);
		}
	}
	
	//TODO Maybe change walk speed here
	// That might not actually be used in the future
}
