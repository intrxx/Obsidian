// Copyright 2024 out of sCope team - Michał Ogiński


#include "Characters/Enemies/ObsidianRegularEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AI/AObsidianAIControllerBase.h"
#include "Components/WidgetComponent.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "UI/ProgressBars/ObsidianRegularEnemyHealthBar.h"

AObsidianRegularEnemy::AObsidianRegularEnemy(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	HealthBarWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidgetComponent"));
	HealthBarWidgetComp->SetupAttachment(GetRootComponent());
	HealthBarWidgetComp->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	HealthBarWidgetComp->SetDrawAtDesiredSize(true);
	HealthBarWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);

	bCanHitReact = true;

	// Identifies this class as a Regular Enemy
	Tags.Emplace(ObsidianActorTags::RegularEnemy);
}

void AObsidianRegularEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(!HasAuthority())
	{
		return;
	}
	ObsidianRegularAIController = Cast<AObsidianAIControllerBase>(NewController);
	ObsidianRegularAIController->GetBlackboardComponent()->InitializeBlackboard(*DefaultBehaviorTree->BlackboardAsset);
	ObsidianRegularAIController->RunBehaviorTree(DefaultBehaviorTree);

	UBlackboardComponent* BlackboardComponent = ObsidianRegularAIController->GetBlackboardComponent();
	BlackboardComponent->SetValueAsBool(FName("bHitReacting"), false);
}

void AObsidianRegularEnemy::OnAbilitySystemInitialized()
{
	Super::OnAbilitySystemInitialized();

	CreateHealthBarWidget();

	check(EnemyAttributesComponent);
	EnemyAttributesComponent->OnDeathStarted.AddUniqueDynamic(this, &ThisClass::OnDeathStarted);
	EnemyAttributesComponent->OnDeathFinished.AddUniqueDynamic(this, &ThisClass::OnDeathFinished);

	UObsidianAbilitySystemComponent* ObsidianASC = GetObsidianAbilitySystemComponent();
	check(ObsidianASC);

	ObsidianASC->RegisterGameplayTagEvent(ObsidianGameplayTags::Effect_HitReact,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::HitReactTagChanged);
}

void AObsidianRegularEnemy::OnDeathStarted(AActor* OwningActor)
{
	Super::OnDeathStarted(OwningActor);

	if(HealthBarWidgetComp)
	{
		HealthBarWidgetComp->DestroyComponent();
	}

	if(ObsidianRegularAIController)
	{
		ObsidianRegularAIController->GetBrainComponent()->StopLogic(TEXT("Death"));
	}
}

void AObsidianRegularEnemy::OnDeathFinished(AActor* OwningActor)
{
	Super::OnDeathFinished(OwningActor);
}

void AObsidianRegularEnemy::CreateHealthBarWidget() const
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

void AObsidianRegularEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;

	if(HasAuthority() && ObsidianRegularAIController)
	{
		if(UBlackboardComponent* BlackboardComponent = ObsidianRegularAIController->GetBlackboardComponent())
		{
			BlackboardComponent->SetValueAsBool(FName("bHitReacting"), bHitReacting);
		}
	}
}

