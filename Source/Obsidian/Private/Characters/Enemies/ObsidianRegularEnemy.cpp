// Copyright 2026 out of sCope team - intrxx

#include "Characters/Enemies/ObsidianRegularEnemy.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AI/AObsidianAIControllerBase.h"
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "UI/ProgressBars/ObsidianRegularEnemyHealthBar.h"


AObsidianRegularEnemy::AObsidianRegularEnemy(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
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

void AObsidianRegularEnemy::PossessedBy(AController* InNewController)
{
	Super::PossessedBy(InNewController);

	if(!HasAuthority())
	{
		return;
	}
	ObsidianRegularAIController = Cast<AObsidianAIControllerBase>(InNewController);
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

	ObsidianASC->RegisterGameplayTagEvent(ObsidianGameplayTags::Effect::HitReact,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::HitReactTagChanged);
}

void AObsidianRegularEnemy::OnDeathStarted(AActor* InOwningActor)
{
	Super::OnDeathStarted(InOwningActor);

	if(HealthBarWidgetComp)
	{
		HealthBarWidgetComp->DestroyComponent();
	}

	if(ObsidianRegularAIController)
	{
		ObsidianRegularAIController->GetBrainComponent()->StopLogic(TEXT("Death"));
	}
}

void AObsidianRegularEnemy::OnDeathFinished(AActor* InOwningActor)
{
	Super::OnDeathFinished(InOwningActor);
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

void AObsidianRegularEnemy::HitReactTagChanged(const FGameplayTag InCallbackTag, int32 InNewCount)
{
	bHitReacting = InNewCount > 0;

	if(HasAuthority() && ObsidianRegularAIController)
	{
		if(UBlackboardComponent* BlackboardComponent = ObsidianRegularAIController->GetBlackboardComponent())
		{
			BlackboardComponent->SetValueAsBool(FName("bHitReacting"), bHitReacting);
		}
	}
}

