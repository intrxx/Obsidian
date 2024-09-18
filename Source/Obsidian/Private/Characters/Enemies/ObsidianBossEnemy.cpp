// Copyright 2024 out of sCope team - Michał Ogiński


#include "Characters/Enemies/ObsidianBossEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/ObsidianBossAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "CharacterComponents/ObsidianAdvancedCombatComponent.h"
#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"

AObsidianBossEnemy::AObsidianBossEnemy(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	AdvancedCombatComponent = CreateDefaultSubobject<UObsidianAdvancedCombatComponent>(TEXT("AdvancedCombatComponent"));

	UMeshComponent* CharacterMesh = GetMesh();
	check(CharacterMesh);
	
	const TMap<EObsidianTracedMeshType, UPrimitiveComponent*> TracedMeshesMap
	{
		{EObsidianTracedMeshType::ETMT_CharacterMesh, CharacterMesh},
		{EObsidianTracedMeshType::ETMT_CharacterMesh_LeftHand, CharacterMesh},
		{EObsidianTracedMeshType::ETMT_CharacterMesh_RightHand, CharacterMesh},
		{EObsidianTracedMeshType::ETMT_RightHandWeaponMesh, RightHandEquipmentMesh},
		{EObsidianTracedMeshType::ETMT_LeftHandWeaponMesh, LeftHandEquipmentMesh}
	};
	AdvancedCombatComponent->AddTracedMeshes(TracedMeshesMap);
	AdvancedCombatComponent->AddIgnoredActor(this);
	
	Tags.Emplace(ObsidianActorTags::BossEnemy);
}

void AObsidianBossEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(!HasAuthority())
	{
		return;
	}
	
	ObsidianBossAIController = Cast<AObsidianBossAIController>(NewController);
	ObsidianBossAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	ObsidianBossAIController->RunBehaviorTree(BehaviorTree);
}

void AObsidianBossEnemy::OnAbilitySystemInitialized()
{
	Super::OnAbilitySystemInitialized();

	check(EnemyAttributesComponent);
	EnemyAttributesComponent->OnDeathStarted.AddUniqueDynamic(this, &ThisClass::OnDeathStarted);
	EnemyAttributesComponent->OnDeathFinished.AddUniqueDynamic(this, &ThisClass::OnDeathFinished);
}

void AObsidianBossEnemy::OnDeathStarted(AActor* OwningActor)
{
	Super::OnDeathStarted(OwningActor);
}

void AObsidianBossEnemy::OnDeathFinished(AActor* OwningActor)
{
	Super::OnDeathFinished(OwningActor);
}
