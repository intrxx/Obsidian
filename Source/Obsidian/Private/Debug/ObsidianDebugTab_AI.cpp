// Copyright 2026 out of sCope team - intrxx

#include "Debug/ObsidianDebugMenuTabs.h"

#if WITH_OBSIDIAN_DEBUG_MENU

#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"
#include "MassEntityQuery.h"
#include "MassEntitySubsystem.h"
#include "NavigationSystem.h"
#include "SlateIM.h"

#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AI/ObsidianBossAIController.h"
#include "AI/ObsidianRegularAIController.h"
#include "CharacterComponents/ObsidianPawnExtensionComponent.h"
#include "Characters/Enemies/ObsidianBossEnemy.h"
#include "Characters/Enemies/ObsidianEnemy.h"
#include "Characters/Enemies/ObsidianRegularEnemy.h"
#include "Mass/ObsidianMassEnemyFragments.h"
#include "Obsidian/ObsidianLogCategories.h"


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

FObsidianDebugTab_AI::FObsidianDebugTab_AI()
	: BehaviorTraceCommand(TEXT("obsidian.DebugMenu.TraceEnemyAI"),
		TEXT("Logs what the enemies do (used Abilities, movement, distance to the Player) so two runs of the game can be compared."
			" Usage: obsidian.DebugMenu.TraceEnemyAI [Duration] [filter=ClassNamePart] [solo] [bring=Time:Distance] [damage=Time:HealthPercent]"
			" [player=Time:X:Y] [quit]. filter traces only the enemies with given text in their class name, solo destroys the rest of them,"
			" bring teleports the traced enemies in front of the Player, damage hurts them, player teleports the Player to given location,"
			" quit closes the game once the trace is done."),
		FConsoleCommandWithWorldAndArgsDelegate::CreateRaw(this, &FObsidianDebugTab_AI::RunBehaviorTrace))
{
}

FObsidianDebugTab_AI::~FObsidianDebugTab_AI()
{
	FTSTicker::RemoveTicker(BehaviorTraceTickerHandle);
	FinishBehaviorTrace();
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

		// Enemies driven by Mass turn themselves into entities when they begin play, their Actor is already gone by now.
		if (IsValid(Enemy) == false)
		{
			++SpawnedCount;
			continue;
		}

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

	// Bosses are driven by the State Tree, the rest of the enemies by the Behavior Tree, each needs the controller that runs it.
	const UClass* RequiredControllerClass = nullptr;
	if (const AObsidianBossEnemy* BossDefault = Cast<AObsidianBossEnemy>(InEnemyDefault))
	{
		if (BossDefault->GetDefaultStateTree() == nullptr)
		{
			return FString::Printf(TEXT("[%s] has no Default State Tree set."), *EnemyName);
		}
		RequiredControllerClass = AObsidianBossAIController::StaticClass();
	}
	else if (const AObsidianRegularEnemy* RegularDefault = Cast<AObsidianRegularEnemy>(InEnemyDefault))
	{
		// Enemies driven by Mass need neither the Behavior Tree nor the AI Controller.
		if (RegularDefault->IsDrivenByMass())
		{
			return FString();
		}

		if (RegularDefault->GetDefaultBehaviorTree() == nullptr || RegularDefault->GetDefaultBehaviorTree()->BlackboardAsset == nullptr)
		{
			return FString::Printf(TEXT("[%s] has no Default Behavior Tree with a Blackboard set."), *EnemyName);
		}
		RequiredControllerClass = AObsidianRegularAIController::StaticClass();
	}
	else
	{
		return FString::Printf(TEXT("[%s] is neither a Boss nor a Regular Enemy, it has no AI to run."), *EnemyName);
	}

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

void FObsidianDebugTab_AI::RunBehaviorTrace(const TArray<FString>& InArgs, UWorld* InWorld)
{
	if (BehaviorTraceTickerHandle.IsValid())
	{
		UE_LOG(ObLogDebugMenu, Warning, TEXT("Enemy AI behavior trace is already running."));
		return;
	}

	BehaviorTrace = MakeUnique<FBehaviorTrace>();
	for (const FString& Arg : InArgs)
	{
		FString Key;
		FString Value;
		if (Arg.Equals(TEXT("quit"), ESearchCase::IgnoreCase))
		{
			BehaviorTrace->bQuitWhenDone = true;
		}
		else if (Arg.Equals(TEXT("solo"), ESearchCase::IgnoreCase))
		{
			BehaviorTrace->bSolo = true;
		}
		else if (Arg.IsNumeric())
		{
			BehaviorTrace->Duration = FMath::Clamp(FCString::Atof(*Arg), 1.0f, 3600.0f);
		}
		else if (Arg.Split(TEXT("="), &Key, &Value))
		{
			if (Key.Equals(TEXT("filter"), ESearchCase::IgnoreCase))
			{
				BehaviorTrace->ClassFilter = Value;
				continue;
			}

			FString TimeString;
			FString ValueString;
			FString SecondValueString;
			const bool bBring = Key.Equals(TEXT("bring"), ESearchCase::IgnoreCase);
			const bool bDamage = Key.Equals(TEXT("damage"), ESearchCase::IgnoreCase);
			const bool bMovePlayer = Key.Equals(TEXT("player"), ESearchCase::IgnoreCase);
			if ((bBring || bDamage) && Value.Split(TEXT(":"), &TimeString, &ValueString))
			{
				FBehaviorTraceAction& Action = BehaviorTrace->Actions.AddDefaulted_GetRef();
				Action.Type = bBring ? FBehaviorTraceAction::EType::Bring : FBehaviorTraceAction::EType::Damage;
				Action.Time = FCString::Atof(*TimeString);
				Action.Value = FCString::Atof(*ValueString);
			}
			else if (bMovePlayer && Value.Split(TEXT(":"), &TimeString, &ValueString) && ValueString.Split(TEXT(":"), &ValueString, &SecondValueString))
			{
				FBehaviorTraceAction& Action = BehaviorTrace->Actions.AddDefaulted_GetRef();
				Action.Type = FBehaviorTraceAction::EType::MovePlayer;
				Action.Time = FCString::Atof(*TimeString);
				Action.Location = FVector2D(FCString::Atof(*ValueString), FCString::Atof(*SecondValueString));
			}
			else
			{
				UE_LOG(ObLogDebugMenu, Warning, TEXT("Enemy AI behavior trace does not understand the [%s] argument, it is skipped."), *Arg);
			}
		}
	}

	// The command can be run before the game is ready (e.g. with -ExecCmds), so it waits for the Player.
	BehaviorTrace->GiveUpTime = FPlatformTime::Seconds() + 120.0;
	BehaviorTraceTickerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(
		[this](float InDeltaTime)
		{
			return TickBehaviorTrace();
		}));
}

bool FObsidianDebugTab_AI::TickBehaviorTrace()
{
	FObsidianDebugMenuContext Context;
	if (GEngine)
	{
		for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
		{
			UWorld* GameWorld = WorldContext.World();
			const bool bGameWorld = WorldContext.WorldType == EWorldType::Game || WorldContext.WorldType == EWorldType::PIE;
			if (GameWorld == nullptr || bGameWorld == false || GameWorld->GetNetMode() == NM_Client || GameWorld->HasBegunPlay() == false)
			{
				continue;
			}

			APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GameWorld);
			if (LocalPlayerController && LocalPlayerController->GetPawn())
			{
				Context.World = GameWorld;
				Context.PlayerController = LocalPlayerController;
				Context.Pawn = LocalPlayerController->GetPawn();
				break;
			}
		}
	}

	const bool bStarted = BehaviorTrace->StartWorldTime >= 0.0;
	if (Context.Pawn == nullptr || (bStarted && BehaviorTrace->World.Get() != Context.World))
	{
		if (bStarted == false && FPlatformTime::Seconds() < BehaviorTrace->GiveUpTime)
		{
			return true;
		}

		UE_LOG(ObLogDebugMenu, Error, TEXT("Enemy AI behavior trace was stopped, there is no game World with a local Player that has a Pawn."));
		BehaviorTraceTickerHandle.Reset();
		FinishBehaviorTrace();
		return false;
	}

	if (bStarted == false)
	{
		StartBehaviorTrace(Context);
	}

	const double TraceTime = Context.World->GetTimeSeconds() - BehaviorTrace->StartWorldTime;

	TrackNewEnemies(Context, TraceTime);

	for (FBehaviorTraceAction& Action : BehaviorTrace->Actions)
	{
		if (Action.bDone == false && TraceTime >= Action.Time)
		{
			Action.bDone = true;
			RunBehaviorTraceAction(Context, Action, TraceTime);
		}
	}

	for (FBehaviorTracedEnemy& TracedEnemy : BehaviorTrace->Enemies)
	{
		const AObsidianEnemy* Enemy = TracedEnemy.Enemy.Get();
		if (Enemy == nullptr)
		{
			if (TracedEnemy.bGoneLogged == false)
			{
				TracedEnemy.bGoneLogged = true;
				UE_LOG(ObLogDebugMenu, Display, TEXT("AITRACE [%7.2f] %s | GONE"), TraceTime, *TracedEnemy.Name);
			}
		}
		else if (TracedEnemy.bDeathLogged == false && IObsidianCombatInterface::Execute_IsDeadOrDying(Enemy))
		{
			TracedEnemy.bDeathLogged = true;
			UE_LOG(ObLogDebugMenu, Display, TEXT("AITRACE [%7.2f] %s | DEATH"), TraceTime, *TracedEnemy.Name);
		}
	}

	if (TraceTime >= BehaviorTrace->NextSampleTime)
	{
		BehaviorTrace->NextSampleTime += BehaviorTrace->SampleInterval;
		LogBehaviorTraceSamples(Context, TraceTime);

		// The Player needs to stay alive for the whole trace, as it is the target of the enemies.
		float PlayerMaxHealth = 0.0f;
		UAbilitySystemComponent* PlayerASC = Context.GetPlayerASC();
		if (ObsidianDebugGAS::GetAttributeValue(PlayerASC, UObsidianCommonAttributeSet::GetMaxHealthAttribute(), PlayerMaxHealth))
		{
			PlayerASC->SetNumericAttributeBase(UObsidianCommonAttributeSet::GetHealthAttribute(), PlayerMaxHealth);
		}
	}

	if (TraceTime < BehaviorTrace->Duration)
	{
		return true;
	}

	const bool bQuitWhenDone = BehaviorTrace->bQuitWhenDone;
	UE_LOG(ObLogDebugMenu, Display, TEXT("AITRACE [%7.2f] END"), TraceTime);
	BehaviorTraceTickerHandle.Reset();
	FinishBehaviorTrace();

	if (bQuitWhenDone)
	{
		FPlatformMisc::RequestExit(false);
	}
	return false;
}

void FObsidianDebugTab_AI::StartBehaviorTrace(const FObsidianDebugMenuContext& InContext)
{
	BehaviorTrace->World = InContext.World;
	BehaviorTrace->StartWorldTime = InContext.World->GetTimeSeconds();
	BehaviorTrace->NextSampleTime = 0.0;

	UE_LOG(ObLogDebugMenu, Display, TEXT("AITRACE BEGIN Map=[%s] Duration=[%.1f] Filter=[%s] Solo=[%d] Player=[%s] at %s"),
		*InContext.World->GetMapName(), BehaviorTrace->Duration, *BehaviorTrace->ClassFilter, BehaviorTrace->bSolo,
		*GetNameSafe(InContext.Pawn), *InContext.Pawn->GetActorLocation().ToCompactString());

	if (BehaviorTrace->bSolo && BehaviorTrace->ClassFilter.IsEmpty() == false)
	{
		TArray<AObsidianEnemy*> EnemiesToDestroy;
		for (TActorIterator<AObsidianEnemy> It(InContext.World); It; ++It)
		{
			if (It->GetClass()->GetName().Contains(BehaviorTrace->ClassFilter) == false)
			{
				EnemiesToDestroy.Add(*It);
			}
		}

		for (AObsidianEnemy* Enemy : EnemiesToDestroy)
		{
			Enemy->Destroy();
		}
	}
}

void FObsidianDebugTab_AI::FinishBehaviorTrace()
{
	if (BehaviorTrace == nullptr)
	{
		return;
	}

	for (const FBehaviorTracedEnemy& TracedEnemy : BehaviorTrace->Enemies)
	{
		if (UAbilitySystemComponent* ASC = TracedEnemy.ASC.Get())
		{
			ASC->AbilityActivatedCallbacks.Remove(TracedEnemy.AbilityActivatedHandle);
			ASC->AbilityEndedCallbacks.Remove(TracedEnemy.AbilityEndedHandle);
			ASC->AbilityFailedCallbacks.Remove(TracedEnemy.AbilityFailedHandle);
		}
	}
	BehaviorTrace.Reset();
}

void FObsidianDebugTab_AI::TrackNewEnemies(const FObsidianDebugMenuContext& InContext, const double InTraceTime)
{
	for (TActorIterator<AObsidianEnemy> It(InContext.World); It; ++It)
	{
		AObsidianEnemy* Enemy = *It;
		if (BehaviorTrace->ClassFilter.IsEmpty() == false && Enemy->GetClass()->GetName().Contains(BehaviorTrace->ClassFilter) == false)
		{
			continue;
		}

		const bool bAlreadyTracked = BehaviorTrace->Enemies.ContainsByPredicate([Enemy](const FBehaviorTracedEnemy& InTracedEnemy)
			{
				return InTracedEnemy.Enemy == Enemy;
			});
		UAbilitySystemComponent* ASC = Enemy->GetAbilitySystemComponent();
		if (bAlreadyTracked || ASC == nullptr)
		{
			continue;
		}

		FBehaviorTracedEnemy& TracedEnemy = BehaviorTrace->Enemies.AddDefaulted_GetRef();
		TracedEnemy.Enemy = Enemy;
		TracedEnemy.ASC = ASC;
		TracedEnemy.Name = Enemy->GetName();

		// World and the start time are captured instead of the trace, as the Abilities can end after the trace is finished.
		const TWeakObjectPtr<UWorld> WeakWorld = InContext.World;
		const TWeakObjectPtr<const APawn> WeakPlayerPawn = InContext.Pawn;
		const TWeakObjectPtr<const AObsidianEnemy> WeakEnemy = Enemy;
		const double StartWorldTime = BehaviorTrace->StartWorldTime;
		const FString EnemyName = TracedEnemy.Name;
		auto LogAbilityEvent = [WeakWorld, WeakPlayerPawn, WeakEnemy, StartWorldTime, EnemyName](const TCHAR* InEvent, const UGameplayAbility* InAbility)
			{
				const UWorld* World = WeakWorld.Get();
				const AObsidianEnemy* TracedEnemy = WeakEnemy.Get();
				const APawn* PlayerPawn = WeakPlayerPawn.Get();
				const float Distance = (TracedEnemy && PlayerPawn) ? TracedEnemy->GetDistanceTo(PlayerPawn) : -1.0f;
				UE_LOG(ObLogDebugMenu, Display, TEXT("AITRACE [%7.2f] %s | %s %s | dist=%.0f"), World ? World->GetTimeSeconds() - StartWorldTime : -1.0,
					*EnemyName, InEvent, InAbility ? *ObsidianDebugUI::GetCleanClassName(InAbility->GetClass()) : TEXT("None"), Distance);
			};

		TracedEnemy.AbilityActivatedHandle = ASC->AbilityActivatedCallbacks.AddLambda([LogAbilityEvent](UGameplayAbility* InAbility)
			{
				LogAbilityEvent(TEXT("ABILITY_ACTIVATED"), InAbility);
			});
		TracedEnemy.AbilityEndedHandle = ASC->AbilityEndedCallbacks.AddLambda([LogAbilityEvent](UGameplayAbility* InAbility)
			{
				LogAbilityEvent(TEXT("ABILITY_ENDED"), InAbility);
			});
		TracedEnemy.AbilityFailedHandle = ASC->AbilityFailedCallbacks.AddLambda([LogAbilityEvent](const UGameplayAbility* InAbility, const FGameplayTagContainer& InFailureTags)
			{
				LogAbilityEvent(TEXT("ABILITY_FAILED"), InAbility);
			});

		const AController* Controller = Enemy->GetController();
		const AAIController* AIController = Cast<AAIController>(Controller);
		const UBrainComponent* BrainComponent = AIController ? AIController->GetBrainComponent() : nullptr;
		UE_LOG(ObLogDebugMenu, Display, TEXT("AITRACE [%7.2f] %s | TRACKED Class=[%s] Level=[%d] Controller=[%s] Brain=[%s] at %s"), InTraceTime,
			*TracedEnemy.Name, *ObsidianDebugUI::GetCleanClassName(Enemy->GetClass()), Enemy->EnemyLevel,
			Controller ? *ObsidianDebugUI::GetCleanClassName(Controller->GetClass()) : TEXT("None"),
			BrainComponent ? *BrainComponent->GetClass()->GetName() : TEXT("None"), *Enemy->GetActorLocation().ToCompactString());
	}
}

void FObsidianDebugTab_AI::RunBehaviorTraceAction(const FObsidianDebugMenuContext& InContext, const FBehaviorTraceAction& InAction, const double InTraceTime)
{
	if (InAction.Type == FBehaviorTraceAction::EType::MovePlayer)
	{
		UE_LOG(ObLogDebugMenu, Display, TEXT("AITRACE [%7.2f] ACTION MovePlayer to [%s]"), InTraceTime, *InAction.Location.ToString());

		const FVector NewLocation(InAction.Location.X, InAction.Location.Y, InContext.Pawn->GetActorLocation().Z);
		InContext.Pawn->TeleportTo(NewLocation, InContext.Pawn->GetActorRotation(), false, true);
		return;
	}

	TArray<AObsidianEnemy*> AliveEnemies;
	for (const FBehaviorTracedEnemy& TracedEnemy : BehaviorTrace->Enemies)
	{
		AObsidianEnemy* Enemy = TracedEnemy.Enemy.Get();
		if (Enemy && IObsidianCombatInterface::Execute_IsDeadOrDying(Enemy) == false)
		{
			AliveEnemies.Add(Enemy);
		}
	}

	if (InAction.Type == FBehaviorTraceAction::EType::Bring)
	{
		UE_LOG(ObLogDebugMenu, Display, TEXT("AITRACE [%7.2f] ACTION Bring [%d] enemies to [%.0f] units from the Player"), InTraceTime,
			AliveEnemies.Num(), InAction.Value);

		const FVector PlayerLocation = InContext.Pawn->GetActorLocation();
		const FVector Forward = InContext.Pawn->GetActorForwardVector();
		const FVector Right = InContext.Pawn->GetActorRightVector();
		const UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(InContext.World);

		// Enemies are lined up next to each other, so they do not need to push themselves out of the same spot.
		const float Spacing = 200.0f;
		for (int32 i = 0; i < AliveEnemies.Num(); ++i)
		{
			AObsidianEnemy* Enemy = AliveEnemies[i];
			const float SideOffset = (i - (AliveEnemies.Num() - 1) * 0.5f) * Spacing;
			FVector BringLocation = PlayerLocation + Forward * InAction.Value + Right * SideOffset;

			FNavLocation NavigableLocation;
			if (NavigationSystem && NavigationSystem->ProjectPointToNavigation(BringLocation, NavigableLocation, FVector(200.0f, 200.0f, 500.0f)))
			{
				const UCapsuleComponent* Capsule = Enemy->GetCapsuleComponent();
				BringLocation = NavigableLocation.Location + FVector(0.0f, 0.0f, Capsule ? Capsule->GetScaledCapsuleHalfHeight() : 0.0f);
			}

			FRotator BringRotation = (PlayerLocation - BringLocation).Rotation();
			BringRotation.Pitch = 0.0f;
			BringRotation.Roll = 0.0f;
			Enemy->TeleportTo(BringLocation, BringRotation, false, true);
		}
	}
	else if (InAction.Type == FBehaviorTraceAction::EType::Damage)
	{
		UE_LOG(ObLogDebugMenu, Display, TEXT("AITRACE [%7.2f] ACTION Damage [%d] enemies for [%.0f] percent of their Max Health"), InTraceTime,
			AliveEnemies.Num(), InAction.Value);

		for (const AObsidianEnemy* Enemy : AliveEnemies)
		{
			float MaxHealth = 0.0f;
			UAbilitySystemComponent* EnemyASC = Enemy->GetAbilitySystemComponent();
			if (ObsidianDebugGAS::GetAttributeValue(EnemyASC, UObsidianCommonAttributeSet::GetMaxHealthAttribute(), MaxHealth))
			{
				// Damage needs to go through Incoming Damage for the regular damage flow (hit reacts, boss thresholds) to be run.
				ObsidianDebugGAS::ApplyInstantAttributeMod(InContext.GetPlayerASC(), EnemyASC, UObsidianCommonAttributeSet::GetIncomingDamageAttribute(),
					EGameplayModOp::Override, MaxHealth * InAction.Value / 100.0f);
			}
		}
	}
}

void FObsidianDebugTab_AI::LogBehaviorTraceSamples(const FObsidianDebugMenuContext& InContext, const double InTraceTime)
{
	const FVector PlayerLocation = InContext.Pawn->GetActorLocation();

	// Enemies driven by Mass exist as entities, only the ones close to the Player are hydrated into the Actors that get traced.
	if (UMassEntitySubsystem* EntitySubsystem = UWorld::GetSubsystem<UMassEntitySubsystem>(InContext.World))
	{
		FMassEntityQuery EnemyQuery(EntitySubsystem->GetMutableEntityManager().AsShared());
		EnemyQuery.AddTagRequirement<FObsidianMassEnemyTag>(EMassFragmentPresence::All);
		const int32 EntityCount = EnemyQuery.GetNumMatchingEntities();
		if (EntityCount > 0 || BehaviorTrace->bMassEntitiesSeen)
		{
			int32 HydratedCount = 0;
			for (TActorIterator<AObsidianEnemy> It(InContext.World); It; ++It)
			{
				HydratedCount += It->IsDrivenByMass() ? 1 : 0;
			}

			BehaviorTrace->bMassEntitiesSeen = true;
			UE_LOG(ObLogDebugMenu, Display, TEXT("AITRACE [%7.2f] MASS entities=%d hydrated=%d"), InTraceTime, EntityCount, HydratedCount);
		}
	}

	for (const FBehaviorTracedEnemy& TracedEnemy : BehaviorTrace->Enemies)
	{
		const AObsidianEnemy* Enemy = TracedEnemy.Enemy.Get();
		if (Enemy == nullptr || TracedEnemy.bDeathLogged)
		{
			continue;
		}

		float Health = 0.0f;
		float MaxHealth = 0.0f;
		const UAbilitySystemComponent* EnemyASC = Enemy->GetAbilitySystemComponent();
		ObsidianDebugGAS::GetAttributeValue(EnemyASC, UObsidianCommonAttributeSet::GetHealthAttribute(), Health);
		ObsidianDebugGAS::GetAttributeValue(EnemyASC, UObsidianCommonAttributeSet::GetMaxHealthAttribute(), MaxHealth);

		// How far the enemy is from facing the Player, shows whether it tracks its target the same way.
		const FVector ToPlayer = (PlayerLocation - Enemy->GetActorLocation()).GetSafeNormal2D();
		const float FacingError = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Enemy->GetActorForwardVector().GetSafeNormal2D(), ToPlayer)));

		const AAIController* AIController = Cast<AAIController>(Enemy->GetController());
		UE_LOG(ObLogDebugMenu, Display, TEXT("AITRACE [%7.2f] %s | SAMPLE dist=%.0f speed=%.0f hp=%.0f facing=%.0f yaw=%.0f ctrl=%.0f focus=%s target=%s"), InTraceTime,
			*TracedEnemy.Name, Enemy->GetDistanceTo(InContext.Pawn), Enemy->GetVelocity().Size2D(), MaxHealth > 0.0f ? Health / MaxHealth * 100.0f : 0.0f,
			FacingError, Enemy->GetActorRotation().Yaw, Enemy->GetControlRotation().Yaw, AIController ? *GetNameSafe(AIController->GetFocusActor()) : TEXT("-"),
			*GetNameSafe(IObsidianEnemyInterface::Execute_GetCombatTarget(Enemy)));
	}
}

#endif // WITH_OBSIDIAN_DEBUG_MENU
