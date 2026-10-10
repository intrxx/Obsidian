// Copyright 2026 out of sCope team - intrxx

#include "Debug/ObsidianDebugMenuTabs.h"

#if WITH_OBSIDIAN_DEBUG_MENU

#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "SlateIM.h"

#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AI/AObsidianAIControllerBase.h"
#include "AI/ObsidianBossAIController.h"
#include "CharacterComponents/ObsidianPawnExtensionComponent.h"
#include "Characters/Enemies/ObsidianBossEnemy.h"
#include "Characters/Enemies/ObsidianEnemy.h"


namespace ObsidianDebugAI
{
	const FString PauseReason(TEXT("ObsidianDebugMenu"));

	/** None is not a valid rarity for an enemy, so it is skipped in the Combo Box. */
	TArray<FString> GetEntityRarityNames()
	{
		TArray<FString> RarityNames;
		const UEnum* EntityRarityEnum = StaticEnum<EObsidianEntityRarity>();
		for (int32 i = 1; i < EntityRarityEnum->NumEnums() - 1; ++i)
		{
			RarityNames.Add(EntityRarityEnum->GetDisplayNameTextByIndex(i).ToString());
		}
		return RarityNames;
	}

	void TableTextCell(const FStringView& InText, const FSlateColor& InColor = FSlateColor::UseForeground())
	{
		if (SlateIM::NextTableCell())
		{
			SlateIM::Padding(FMargin(4.0f, 0.0f));
			SlateIM::VAlign(VAlign_Center);
			SlateIM::Text(InText, {.Color = InColor});
		}
	}
}

void FObsidianDebugTab_AI::Tick(const FObsidianDebugMenuContext& InContext, const float InDeltaTime)
{
	// Done every frame to also catch the enemies that were spawned by the game after the AI was frozen.
	if (bFreezeAI && InContext.HasAuthority())
	{
		for (TActorIterator<AObsidianEnemy> It(InContext.World); It; ++It)
		{
			SetAILogicPaused(*It, true);
		}
	}
}

void FObsidianDebugTab_AI::Draw(const FObsidianDebugMenuContext& InContext)
{
	DrawSpawning(InContext);
	DrawSpawnedEnemies(InContext);
}

void FObsidianDebugTab_AI::DrawSpawning(const FObsidianDebugMenuContext& InContext)
{
	ObsidianDebugUI::Section(TEXT("Spawning"));

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Enemy"));
		if (EnemyPicker.Draw(AObsidianEnemy::StaticClass()))
		{
			SelectedEnemyClass.Reset(EnemyPicker.LoadSelectedClass());
		}
	}
	SlateIM::EndHorizontalStack();

	const FString SpawnBlocker = SelectedEnemyClass.IsValid() ? GetSpawnBlocker(GetDefault<AObsidianEnemy>(SelectedEnemyClass.Get())) : FString();

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Count"));
		SlateIM::MinWidth(120.0f);
		SlateIM::SpinBox(SpawnCount, {.Min = 1, .Max = 200});
	}
	SlateIM::EndHorizontalStack();

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Distance"));
		SlateIM::SetToolTip(TEXT("How far in front of the Player the enemies are spawned."));
		SlateIM::MinWidth(120.0f);
		SlateIM::SpinBox(SpawnDistance, {.Min = 0.0f, .Max = 10000.0f});
	}
	SlateIM::EndHorizontalStack();

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Spread Radius"));
		SlateIM::SetToolTip(TEXT("Enemies are spawned at random navigable points in this radius."));
		SlateIM::MinWidth(120.0f);
		SlateIM::SpinBox(SpawnSpreadRadius, {.Min = 0.0f, .Max = 5000.0f});
	}
	SlateIM::EndHorizontalStack();

	SlateIM::BeginHorizontalStack();
	{
		SlateIM::VAlign(VAlign_Center);
		SlateIM::MinWidth(150.0f);
		SlateIM::CheckBox(bOverrideLevel, {.Label = TEXT("Override Level")});

		if (bOverrideLevel)
		{
			SlateIM::MinWidth(120.0f);
			SlateIM::SpinBox(LevelOverride, {.Min = 1, .Max = 90});
		}
	}
	SlateIM::EndHorizontalStack();

	SlateIM::BeginHorizontalStack();
	{
		SlateIM::VAlign(VAlign_Center);
		SlateIM::MinWidth(150.0f);
		SlateIM::CheckBox(bOverrideRarity, {.Label = TEXT("Override Rarity")});

		if (bOverrideRarity)
		{
			SlateIM::MinWidth(200.0f);
			RarityComboBox.Draw(ObsidianDebugAI::GetEntityRarityNames(), RarityIndex);
		}
	}
	SlateIM::EndHorizontalStack();

	if (SlateIM::Button(TEXT("Spawn"), {.bEnabled = SpawnBlocker.IsEmpty()}))
	{
		SpawnEnemies(InContext);
	}

	if (SpawnBlocker.IsEmpty() == false)
	{
		SlateIM::HAlign(HAlign_Fill);
		ObsidianDebugUI::WarningText(SpawnBlocker);
	}
}

void FObsidianDebugTab_AI::DrawSpawnedEnemies(const FObsidianDebugMenuContext& InContext)
{
	using namespace ObsidianDebugAI;

	ObsidianDebugUI::Section(TEXT("Enemies In The World"));

	TArray<AObsidianEnemy*> Enemies;
	for (TActorIterator<AObsidianEnemy> It(InContext.World); It; ++It)
	{
		Enemies.Add(*It);
	}

	// Enemies cannot be killed or destroyed while drawing the table, so it is done after it.
	TArray<AObsidianEnemy*> EnemiesToKill;
	TArray<AObsidianEnemy*> EnemiesToDestroy;

	SlateIM::HAlign(HAlign_Fill);
	SlateIM::BeginHorizontalStack();
	{
		SlateIM::VAlign(VAlign_Center);
		SlateIM::SetToolTip(TEXT("Pauses the brain of every enemy, works only while the Debug Menu is opened for the newly spawned ones."));
		if (SlateIM::CheckBox(bFreezeAI, {.Label = TEXT("Freeze AI")}))
		{
			for (const AObsidianEnemy* Enemy : Enemies)
			{
				SetAILogicPaused(Enemy, bFreezeAI);
			}
		}

		SlateIM::Padding(FMargin(8.0f, 0.0f));
		SlateIM::VAlign(VAlign_Center);
		SlateIM::SetToolTip(TEXT("Kills are dealt by the chosen Player, so it is rewarded with experience the same way it is in the game."));
		SlateIM::CheckBox(bCreditPlayerForKills, {.Label = TEXT("Credit Player For Kills")});

		SlateIM::Padding(FMargin(8.0f, 0.0f));
		if (SlateIM::Button(TEXT("Kill All")))
		{
			EnemiesToKill = Enemies;
		}

		SlateIM::SetToolTip(TEXT("Destroys the enemies right away, without running the death flow or dropping any items."));
		if (SlateIM::Button(TEXT("Destroy All")))
		{
			EnemiesToDestroy = Enemies;
		}

		SlateIM::Padding(FMargin(8.0f, 0.0f));
		SlateIM::Fill();
		SlateIM::HAlign(HAlign_Fill);
		SlateIM::VAlign(VAlign_Center);
		SlateIM::Text(FString::Printf(TEXT("Count: %d"), Enemies.Num()));
	}
	SlateIM::EndHorizontalStack();

	SlateIM::HAlign(HAlign_Fill);
	SlateIM::MinHeight(160.0f);
	SlateIM::MaxHeight(420.0f);
	SlateIM::BeginTable();
	SlateIM::BeginTableHeader();
	SlateIM::InitialTableColumnWidth(300.0f);
	SlateIM::AddTableColumn(TEXT("Enemy"), TEXT("Enemy"));
	SlateIM::InitialTableColumnWidth(60.0f);
	SlateIM::AddTableColumn(TEXT("Level"), TEXT("Level"));
	SlateIM::InitialTableColumnWidth(140.0f);
	SlateIM::AddTableColumn(TEXT("Rarity"), TEXT("Rarity"));
	SlateIM::InitialTableColumnWidth(140.0f);
	SlateIM::AddTableColumn(TEXT("Health"), TEXT("Health"));
	SlateIM::AddTableColumn(TEXT("Actions"), TEXT("Actions"));
	SlateIM::EndTableHeader();
	SlateIM::BeginTableBody();
	for (AObsidianEnemy* Enemy : Enemies)
	{
		const bool bDead = IObsidianCombatInterface::Execute_IsDeadOrDying(Enemy);

		FString HealthString = TEXT("-");
		float Health = 0.0f;
		float MaxHealth = 0.0f;
		const UAbilitySystemComponent* EnemyASC = Enemy->GetAbilitySystemComponent();
		if (bDead)
		{
			HealthString = TEXT("Dead");
		}
		else if (ObsidianDebugGAS::GetAttributeValue(EnemyASC, UObsidianCommonAttributeSet::GetHealthAttribute(), Health)
			&& ObsidianDebugGAS::GetAttributeValue(EnemyASC, UObsidianCommonAttributeSet::GetMaxHealthAttribute(), MaxHealth))
		{
			HealthString = FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth);
		}

		TableTextCell(Enemy->GetName());
		TableTextCell(FString::FromInt(Enemy->EnemyLevel));
		TableTextCell(StaticEnum<EObsidianEntityRarity>()->GetDisplayNameTextByValue(static_cast<int64>(Enemy->EnemyRarity)).ToString());
		TableTextCell(HealthString, bDead ? FSlateColor(FStyleColors::AccentGray) : FSlateColor::UseForeground());

		if (SlateIM::NextTableCell())
		{
			SlateIM::BeginHorizontalStack();
			if (SlateIM::Button(TEXT("Kill"), {.bEnabled = bDead == false}))
			{
				EnemiesToKill.Add(Enemy);
			}
			if (SlateIM::Button(TEXT("Destroy")))
			{
				EnemiesToDestroy.Add(Enemy);
			}
			SlateIM::SetToolTip(TEXT("Teleports the enemy in front of the chosen Player."));
			if (SlateIM::Button(TEXT("Bring"), {.bEnabled = InContext.Pawn != nullptr}))
			{
				const FVector BringLocation = InContext.Pawn->GetActorLocation() + InContext.Pawn->GetActorForwardVector() * 250.0f;
				Enemy->TeleportTo(BringLocation, Enemy->GetActorRotation(), false, true);
			}
			SlateIM::EndHorizontalStack();
		}
	}
	SlateIM::EndTableBody();
	SlateIM::EndTable();

	if (EnemiesToKill.IsEmpty() == false)
	{
		UAbilitySystemComponent* InstigatorASC = bCreditPlayerForKills ? InContext.GetPlayerASC() : nullptr;

		int32 KilledCount = 0;
		for (const AObsidianEnemy* Enemy : EnemiesToKill)
		{
			if (IObsidianCombatInterface::Execute_IsDeadOrDying(Enemy) == false
				&& ObsidianDebugGAS::Kill(Enemy->GetAbilitySystemComponent(), InstigatorASC))
			{
				++KilledCount;
			}
		}
		InContext.Notify(FString::Printf(TEXT("Killed [%d] enemies."), KilledCount));
	}

	if (EnemiesToDestroy.IsEmpty() == false)
	{
		for (AObsidianEnemy* Enemy : EnemiesToDestroy)
		{
			Enemy->Destroy();
		}
		InContext.Notify(FString::Printf(TEXT("Destroyed [%d] enemies."), EnemiesToDestroy.Num()));
	}
}

void FObsidianDebugTab_AI::SpawnEnemies(const FObsidianDebugMenuContext& InContext)
{
	const TSubclassOf<AObsidianEnemy> EnemyClass = EnemyPicker.LoadSelectedClass();
	if (EnemyClass == nullptr)
	{
		InContext.Notify(TEXT("Could not spawn the enemies, chosen class is invalid."));
		return;
	}

	SelectedEnemyClass.Reset(EnemyClass.Get());

	const FString SpawnBlocker = GetSpawnBlocker(EnemyClass.GetDefaultObject());
	if (SpawnBlocker.IsEmpty() == false)
	{
		InContext.Notify(FString::Printf(TEXT("Could not spawn the enemies. %s"), *SpawnBlocker));
		return;
	}

	if (InContext.Pawn == nullptr)
	{
		InContext.Notify(TEXT("Could not spawn the enemies, chosen Player has no Pawn to spawn them around."));
		return;
	}

	UWorld* World = InContext.World;
	const FVector PlayerLocation = InContext.Pawn->GetActorLocation();
	const FVector SpawnOrigin = PlayerLocation + InContext.Pawn->GetActorForwardVector() * SpawnDistance;
	const UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(World);

	// Navigable points are on the ground, while the characters are spawned by the center of their capsule.
	const UCapsuleComponent* DefaultCapsule = EnemyClass.GetDefaultObject()->GetCapsuleComponent();
	const float CapsuleHalfHeight = DefaultCapsule ? DefaultCapsule->GetScaledCapsuleHalfHeight() : 0.0f;

	int32 SpawnedCount = 0;
	for (int32 i = 0; i < SpawnCount; ++i)
	{
		FVector SpawnLocation = SpawnOrigin;
		FNavLocation NavigableLocation;
		if (NavigationSystem && NavigationSystem->GetRandomPointInNavigableRadius(SpawnOrigin, FMath::Max(SpawnSpreadRadius, 1.0f), NavigableLocation))
		{
			SpawnLocation = NavigableLocation.Location + FVector(0.0f, 0.0f, CapsuleHalfHeight);
		}

		// Spawned enemies face the Player.
		FRotator SpawnRotation = (PlayerLocation - SpawnLocation).Rotation();
		SpawnRotation.Pitch = 0.0f;
		SpawnRotation.Roll = 0.0f;
		const FTransform SpawnTransform(SpawnRotation, SpawnLocation);

		AObsidianEnemy* Enemy = World->SpawnActorDeferred<AObsidianEnemy>(EnemyClass, SpawnTransform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		if (Enemy == nullptr)
		{
			continue;
		}

		// Both need to be set before Begin Play as this is when the enemy initializes its Ability System with them.
		if (bOverrideLevel)
		{
			Enemy->EnemyLevel = LevelOverride;
		}
		if (bOverrideRarity)
		{
			// First entry of the enum (None) is skipped in the Combo Box.
			Enemy->EnemyRarity = static_cast<EObsidianEntityRarity>(RarityIndex + 1);
		}

		Enemy->FinishSpawning(SpawnTransform);

		// Enemies set to be possessed only when placed in the World would be left without the brain.
		if (Enemy->GetController() == nullptr)
		{
			Enemy->SpawnDefaultController();
		}

		if (bFreezeAI)
		{
			SetAILogicPaused(Enemy, true);
		}

		++SpawnedCount;
	}

	InContext.Notify(FString::Printf(TEXT("Spawned [%d] out of [%d] [%s]."), SpawnedCount, SpawnCount,
		*ObsidianDebugUI::GetCleanClassName(EnemyClass.Get())));
}

FString FObsidianDebugTab_AI::GetSpawnBlocker(const AObsidianEnemy* InEnemyDefault)
{
	if (InEnemyDefault == nullptr)
	{
		return FString();
	}

	const FString EnemyName = ObsidianDebugUI::GetCleanClassName(InEnemyDefault);

	if (InEnemyDefault->PawnExtComp == nullptr || InEnemyDefault->PawnExtComp->GetPawnData() == nullptr)
	{
		return FString::Printf(TEXT("[%s] has no Pawn Data set, it is most likely a base class."), *EnemyName);
	}

	if (InEnemyDefault->DefaultBehaviorTree == nullptr || InEnemyDefault->DefaultBehaviorTree->BlackboardAsset == nullptr)
	{
		return FString::Printf(TEXT("[%s] has no Default Behavior Tree with a Blackboard set."), *EnemyName);
	}

	// Bosses use the features of their own controller, the rest of the enemies is fine with the base one.
	const UClass* RequiredControllerClass = InEnemyDefault->IsA<AObsidianBossEnemy>() ? AObsidianBossAIController::StaticClass() :
		AObsidianAIControllerBase::StaticClass();
	if (InEnemyDefault->AIControllerClass == nullptr || InEnemyDefault->AIControllerClass->IsChildOf(RequiredControllerClass) == false)
	{
		return FString::Printf(TEXT("[%s] needs its AI Controller Class to be a [%s]."), *EnemyName, *RequiredControllerClass->GetName());
	}

	return FString();
}

void FObsidianDebugTab_AI::SetAILogicPaused(const AObsidianEnemy* InEnemy, const bool bInPaused)
{
	AAIController* AIController = InEnemy ? Cast<AAIController>(InEnemy->GetController()) : nullptr;
	UBrainComponent* BrainComponent = AIController ? AIController->GetBrainComponent() : nullptr;
	if (BrainComponent == nullptr || BrainComponent->IsPaused() == bInPaused)
	{
		return;
	}

	if (bInPaused)
	{
		BrainComponent->PauseLogic(ObsidianDebugAI::PauseReason);
		AIController->StopMovement();
	}
	else
	{
		BrainComponent->ResumeLogic(ObsidianDebugAI::PauseReason);
	}
}

#endif // WITH_OBSIDIAN_DEBUG_MENU
