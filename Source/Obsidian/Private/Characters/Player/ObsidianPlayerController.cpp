// Copyright 2026 out of sCope team - intrxx

#include "Characters/Player/ObsidianPlayerController.h"

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/ObsidianPlayerInputManager.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "Characters/ObsidianCharacterBase.h"
#include "Characters/Player/ObsidianPlayerState.h"
#include "Core/ObsidianGameplayStatics.h"
#include "Game/Save/ObsidianSaveGameSubsystem.h"
#include "Game/Save/ObsidianSharedStashSaveGame.h"
#include "InventoryItems/Crafting/ObsidianCraftingComponent.h"
#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/Items/ObsidianItemSpawner.h"
#include "InventoryItems/ObsidianItemManagerComponent.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"
#include "Obsidian/ObsidianLogCategories.h"
#include "ObsidianTypes/ObsidianSavedTypes.h"
#include "UI/DamageNumbers/ObsidianDamageNumberWidgetComp.h"
#include "UI/ObsidianHUD.h"


AObsidianPlayerController::AObsidianPlayerController(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	bReplicates = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	InventoryComponent = CreateDefaultSubobject<UObsidianInventoryComponent>(TEXT("Inventory Component"));
	EquipmentComponent = CreateDefaultSubobject<UObsidianEquipmentComponent>(TEXT("Equipment Component"));
	PlayerStashComponent = CreateDefaultSubobject<UObsidianPlayerStashComponent>(TEXT("Player Stash Component"));
	CraftingComponent = CreateDefaultSubobject<UObsidianCraftingComponent>(TEXT("Crafting Component"));
	ItemManagerComponent = CreateDefaultSubobject<UObsidianItemManagerComponent>(TEXT("Item Manager Component"));
	
	SetGenericTeamId(FGenericTeamId(2));
}

void AObsidianPlayerController::UpdateHoveredRegularEnemyTarget(AActor* InTargetActor, const bool bInHoveredOver) const
{
	OnEnemyActorHoveredDelegate.ExecuteIfBound(InTargetActor, bInHoveredOver);
}

void AObsidianPlayerController::ServerSpawnItemFromSpawner_Implementation(AObsidianItemSpawner* InItemSpawner)
{
	if(InItemSpawner == nullptr || InItemSpawner->CanInteract() == false)
	{
		return;
	}

	const APawn* OwnedPawn = GetPawn();
	if(OwnedPawn == nullptr)
	{
		return;
	}

	const float DistanceToSpawnerSquared = FVector::DistSquared2D(OwnedPawn->GetActorLocation(), InItemSpawner->GetActorLocation());
	if(DistanceToSpawnerSquared > FMath::Square(InItemSpawner->GetInteractionRadius() + ObsidianPlayerInputStatics::InteractionRangeTolerance))
	{
		UE_LOG(ObLogInteraction, Warning, TEXT("[%hs]: Item Spawner is too far to be interacted with!"), __FUNCTION__);
		return;
	}

	InItemSpawner->SpawnItem();
}

void AObsidianPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	SetMouseCursorWidget(EMouseCursor::Default, DefaultCursor);
	CurrentMouseCursor = EMouseCursor::Default;
	
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AObsidianPlayerController::PostProcessInput(const float InDeltaTime, const bool bInGamePaused)
{
	if(UObsidianAbilitySystemComponent* ObsidianASC = GetObsidianAbilitySystemComponent())
	{
		ObsidianASC->ProcessAbilityInput(InDeltaTime, bInGamePaused);
	}
	
	Super::PostProcessInput(InDeltaTime, bInGamePaused);
}

void AObsidianPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (PlayerStashComponent)
	{
		PlayerStashComponent->InitializeStashTabs();
	}
}

void AObsidianPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (PlayerStashComponent && HasAuthority() && IsLocalController() && bRequestedSharedStashData == false)
	{
		bRequestedSharedStashData = true;

		if (const UGameInstance* GameInstance = GetGameInstance())
		{
			if (UObsidianSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UObsidianSaveGameSubsystem>())
			{
				const EObsidianGameNetworkType CurrentNetworkType = UObsidianGameplayStatics::GetCurrentNetworkType(this);
				if (UObsidianSharedStashSaveGame* SharedStashSaveGame =  SaveGameSubsystem->GetStashSaveGameObject(CurrentNetworkType))
				{
					LoadSharedStashData(SharedStashSaveGame);
				}
				else
				{
					SharedStashDataLoadDelegateHandle = SaveGameSubsystem->OnSharedStashDataLoadedDelegate.AddUObject(
						this, &ThisClass::LoadSharedStashData);
				}
			}
		}
	}
}

FGenericTeamId AObsidianPlayerController::GetGenericTeamId() const
{
	return TeamId;
}

void AObsidianPlayerController::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	if (TeamId != InTeamID)
	{
		TeamId = InTeamID;
	}
}

UObsidianInventoryComponent* AObsidianPlayerController::GetInventoryComponent() const
{
	return InventoryComponent;
}

UObsidianEquipmentComponent* AObsidianPlayerController::GetEquipmentComponent() const
{
	return EquipmentComponent;
}

UObsidianPlayerStashComponent* AObsidianPlayerController::GetPlayerStashComponent() const
{
	return PlayerStashComponent;
}

UObsidianCraftingComponent* AObsidianPlayerController::GetCraftingComponent() const
{
	return CraftingComponent;
}

UObsidianItemManagerComponent* AObsidianPlayerController::GetItemManagerComponent() const
{
	return ItemManagerComponent;
}

void AObsidianPlayerController::LoadSharedStashData(UObsidianSharedStashSaveGame* InSharedStashSaveGame)
{
	if (PlayerStashComponent && InSharedStashSaveGame)
	{
		for (const FObsidianSavedItem& SavedItem : InSharedStashSaveGame->SharedStashData.StashedSavedItems)
		{
			PlayerStashComponent->LoadStashedItem(SavedItem);
		}

		//TODO(intrxx) Load Stash Tab Names
	}

	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UObsidianSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UObsidianSaveGameSubsystem>())
		{
			SaveGameSubsystem->OnSharedStashDataLoadedDelegate.Remove(SharedStashDataLoadDelegateHandle);
		}
	}
}

EObsidianHeroClass AObsidianPlayerController::GetHeroClass() const
{
	if (const AObsidianHero* ObsidianHero = GetObsidianHero())
	{
		return ObsidianHero->GetHeroClass();
	}
	return EObsidianHeroClass::None;
}

AObsidianPlayerState* AObsidianPlayerController::GetObsidianPlayerState() const
{
	return CastChecked<AObsidianPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UObsidianAbilitySystemComponent* AObsidianPlayerController::GetObsidianAbilitySystemComponent() const
{
	const AObsidianPlayerState* ObsidianPS = GetObsidianPlayerState();
	return (ObsidianPS ? ObsidianPS->GetObsidianAbilitySystemComponent() : nullptr);
}

AObsidianHUD* AObsidianPlayerController::GetObsidianHUD() const
{
	return GetHUD<AObsidianHUD>();
}

AObsidianHero* AObsidianPlayerController::GetObsidianHero() const
{
	return CastChecked<AObsidianHero>(GetCharacter(), ECastCheckedType::NullAllowed);
}

void AObsidianPlayerController::TogglePlayerStash(const bool bInShowStash) const
{
	if(const AObsidianHUD* ObsidianHUD = GetObsidianHUD())
	{
		ObsidianHUD->TogglePlayerStash(bInShowStash);
	}
}

void AObsidianPlayerController::ClientShowDamageNumber_Implementation(const FObsidianDamageTextProps& InDamageTextProps, AObsidianCharacterBase* InTargetCharacter)
{
	// I use IsValid on the character to also check if the character is currently pending kill
	if(IsValid(InTargetCharacter) && DamageNumberWidgetCompClass && IsLocalController())
	{
		UObsidianDamageNumberWidgetComp* DamageNumberWidgetComp = NewObject<UObsidianDamageNumberWidgetComp>(InTargetCharacter, DamageNumberWidgetCompClass);
		DamageNumberWidgetComp->RegisterComponent();
		DamageNumberWidgetComp->AttachToComponent(InTargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

		// After attaching the component its widget will play animation right away, so we don't want the widget to follow the Target Character around
		DamageNumberWidgetComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageNumberWidgetComp->SetDamageTextProps(InDamageTextProps);
	}
}



