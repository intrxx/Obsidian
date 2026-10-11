// Copyright 2026 out of sCope team - intrxx

#include "Characters/Enemies/ObsidianRegularEnemy.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MassCommandBuffer.h"
#include "MassCommonFragments.h"
#include "MassEntityConfigAsset.h"
#include "MassEntityManager.h"
#include "MassEntityUtils.h"
#include "MassEntityView.h"
#include "MassSignalSubsystem.h"
#include "MassSpawnerSubsystem.h"
#include "MassStateTreeTypes.h"

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AI/ObsidianRegularAIController.h"
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"
#include "Mass/ObsidianMassEnemyFragments.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "Obsidian/ObsidianLogCategories.h"
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

void AObsidianRegularEnemy::InitializeFromMassEntity(const FMassEntityHandle InEntity, const FObsidianMassEnemyIdentityFragment& InIdentity)
{
	MassEntity = InEntity;
	SetEnemyIdentity(InIdentity.Level, InIdentity.Rarity);
}

void AObsidianRegularEnemy::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	if(IsDrivenByMass())
	{
		// The brain is the entity, there is no AI Controller to possess the Enemy or to rotate it.
		AutoPossessAI = EAutoPossessAI::Disabled;

		UCharacterMovementComponent* MovementComp = GetCharacterMovement();
		MovementComp->bRunPhysicsWithNoController = true;
		MovementComp->bUseControllerDesiredRotation = false;
	}
}

void AObsidianRegularEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Enemy that was not spawned by Mass (placed in the World, spawned by hand) is replaced with an entity,
	// Mass hydrates it back into an Actor when a Player is close enough.
	if(HasAuthority() && IsDrivenByMass() && MassEntity.IsSet() == false)
	{
		if(ConvertToMassEntity())
		{
			Destroy();
		}
		else
		{
			UE_LOG(ObLogAI, Error, TEXT("Regular Enemy [%s] could not be turned into a Mass entity, it will have no AI."), *GetNameSafe(this));
		}
	}
}

bool AObsidianRegularEnemy::ConvertToMassEntity()
{
	UWorld* World = GetWorld();
	UMassSpawnerSubsystem* SpawnerSubsystem = UWorld::GetSubsystem<UMassSpawnerSubsystem>(World);
	if(SpawnerSubsystem == nullptr)
	{
		return false;
	}

	const FMassEntityTemplate& EntityTemplate = MassEntityConfig->GetOrCreateEntityTemplate(*World);
	if(EntityTemplate.IsValid() == false)
	{
		return false;
	}

	// Observers of the new entity are held back until the creation context is released, so they see what is set below.
	TArray<FMassEntityHandle> Entities;
	const TSharedPtr<FMassEntityManager::FEntityCreationContext> CreationContext = SpawnerSubsystem->SpawnEntities(EntityTemplate, 1, Entities);
	if(Entities.IsEmpty())
	{
		return false;
	}

	const FMassEntityView EntityView(SpawnerSubsystem->GetEntityManagerChecked(), Entities[0]);

	// Entities are located by their feet.
	FTransform EntityTransform = GetActorTransform();
	EntityTransform.AddToTranslation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	EntityView.GetFragmentData<FTransformFragment>().SetTransform(EntityTransform);

	if(FObsidianMassEnemyIdentityFragment* Identity = EntityView.GetFragmentDataPtr<FObsidianMassEnemyIdentityFragment>())
	{
		Identity->Level = GetCharacterLevel();
		Identity->Rarity = GetEnemyRarity();
	}

	return true;
}

void AObsidianRegularEnemy::UpdateMassEnemyState()
{
	UWorld* World = GetWorld();
	if(World == nullptr || HasAuthority() == false || MassEntity.IsSet() == false)
	{
		return;
	}

	const FMassEntityHandle Entity = MassEntity;
	const bool bIsHitReacting = bHitReacting;
	const bool bIsDying = IsDeadOrDying_Implementation();
	const TWeakObjectPtr<UWorld> WeakWorld = World;
	auto UpdateState = [Entity, bIsHitReacting, bIsDying, WeakWorld](FMassEntityManager& InEntityManager)
		{
			FObsidianMassEnemyStateFragment* EnemyState = InEntityManager.IsEntityValid(Entity)
				? InEntityManager.GetFragmentDataPtr<FObsidianMassEnemyStateFragment>(Entity) : nullptr;
			if(EnemyState == nullptr)
			{
				return;
			}

			EnemyState->bHitReacting = bIsHitReacting;
			EnemyState->bDying = bIsDying;

			// State Tree of the entity only runs when it is signaled.
			if(UMassSignalSubsystem* SignalSubsystem = UWorld::GetSubsystem<UMassSignalSubsystem>(WeakWorld.Get()))
			{
				SignalSubsystem->SignalEntity(UE::Mass::Signals::NewStateTreeTaskRequired, Entity);
			}
		};

	// Fragments cannot be touched while Mass is processing them, which is the case when the Ability that hurt the Enemy was used by Mass.
	FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(*World);
	if(EntityManager.IsProcessing())
	{
		EntityManager.Defer().PushCommand<FMassDeferredSetCommand>(UpdateState);
	}
	else
	{
		UpdateState(EntityManager);
	}
}

void AObsidianRegularEnemy::DestroyDeadCharacter()
{
	UWorld* World = GetWorld();
	if(World && MassEntity.IsSet())
	{
		// Mass owns the Actor, destroying the entity releases it. Destroying only the Actor would get it hydrated again.
		FMassEntityManager& EntityManager = UE::Mass::Utils::GetEntityManagerChecked(*World);
		if(EntityManager.IsEntityValid(MassEntity))
		{
			EntityManager.Defer().DestroyEntity(MassEntity);
			return;
		}
	}

	Super::DestroyDeadCharacter();
}

void AObsidianRegularEnemy::PossessedBy(AController* InNewController)
{
	Super::PossessedBy(InNewController);

	if(!HasAuthority())
	{
		return;
	}
	ObsidianRegularAIController = Cast<AObsidianRegularAIController>(InNewController);
	if(ObsidianRegularAIController == nullptr)
	{
		UE_LOG(ObLogAI, Error, TEXT("Regular Enemy [%s] was possessed by [%s] which is not an Obsidian Regular AI Controller, it will have no AI."),
			*GetNameSafe(this), *GetNameSafe(InNewController));
		return;
	}

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

	if(ObsidianRegularAIController && ObsidianRegularAIController->GetBrainComponent())
	{
		ObsidianRegularAIController->GetBrainComponent()->StopLogic(TEXT("Death"));
	}

	UpdateMassEnemyState();
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
	UpdateMassEnemyState();

	if(HasAuthority() && ObsidianRegularAIController)
	{
		if(UBlackboardComponent* BlackboardComponent = ObsidianRegularAIController->GetBlackboardComponent())
		{
			BlackboardComponent->SetValueAsBool(FName("bHitReacting"), bHitReacting);
		}
	}
}

