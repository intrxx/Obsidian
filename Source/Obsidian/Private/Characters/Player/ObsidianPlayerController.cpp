// Copyright 2026 out of sCope team - intrxx

#include "Characters/Player/ObsidianPlayerController.h"


#include "UI/ObsidianHUD.h"
#include "Characters/ObsidianCharacterBase.h"
#include "UI/DamageNumbers/ObsidianDamageNumberWidgetComp.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/Heroes/ObsidianHero.h"
#include "Characters/Player/ObsidianPlayerState.h"
#include "Core/ObsidianGameplayStatics.h"
#include "Game/Save/ObsidianSaveGameSubsystem.h"
#include "Game/Save/ObsidianSharedStashSaveGame.h"
#include "InventoryItems/ObsidianItemManagerComponent.h"
#include "InventoryItems/Crafting/ObsidianCraftingComponent.h"
#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"
#include "InventoryItems/Items/ObsidianItemSpawner.h"
#include "ObsidianTypes/ObsidianSavedTypes.h"

AObsidianPlayerController::AObsidianPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	InventoryComponent = CreateDefaultSubobject<UObsidianInventoryComponent>(TEXT("Inventory Component"));
	EquipmentComponent = CreateDefaultSubobject<UObsidianEquipmentComponent>(TEXT("Equipment Component"));
	PlayerStashComponent = CreateDefaultSubobject<UObsidianPlayerStashComponent>(TEXT("Player Stash Component"));
	CraftingComponent = CreateDefaultSubobject<UObsidianCraftingComponent>(TEXT("Crafting Component"));
	ItemManagerComponent = CreateDefaultSubobject<UObsidianItemManagerComponent>(TEXT("Item Manager Component"));
}

void AObsidianPlayerController::UpdateHoveredRegularEnemyTarget(AActor* TargetActor, const bool bHoveredOver) const
{
	OnEnemyActorHoveredDelegate.ExecuteIfBound(TargetActor, bHoveredOver);
}

void AObsidianPlayerController::ServerSpawnItemFromSpawner_Implementation(AObsidianItemSpawner* ItemSpawner)
{
	if(ItemSpawner)
	{
		ItemSpawner->SpawnItem();
	}
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

void AObsidianPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if(UObsidianAbilitySystemComponent* ObsidianASC = GetObsidianAbilitySystemComponent())
	{
		ObsidianASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
	
	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void AObsidianPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (PlayerStashComponent)
	{
		PlayerStashComponent->InitializeStashTabs();

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

void AObsidianPlayerController::LoadSharedStashData(UObsidianSharedStashSaveGame* SharedStashSaveGame)
{
	if (PlayerStashComponent && SharedStashSaveGame)
	{
		for (const FObsidianSavedItem& SavedItem : SharedStashSaveGame->SharedStashData.StashedSavedItems)
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

void AObsidianPlayerController::TogglePlayerStash(const bool bShowStash) const
{
	if(const AObsidianHUD* ObsidianHUD = GetObsidianHUD())
	{
		ObsidianHUD->TogglePlayerStash(bShowStash);
	}
}

void AObsidianPlayerController::ClientShowDamageNumber_Implementation(const FObsidianDamageTextProps& DamageTextProps, AObsidianCharacterBase* TargetCharacter)
{
	// I use IsValid on the character to also check if the character is currently pending kill
	if(IsValid(TargetCharacter) && DamageNumberWidgetCompClass && IsLocalController())
	{
		UObsidianDamageNumberWidgetComp* DamageNumberWidgetComp = NewObject<UObsidianDamageNumberWidgetComp>(TargetCharacter, DamageNumberWidgetCompClass);
		DamageNumberWidgetComp->RegisterComponent();
		DamageNumberWidgetComp->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

		// After attaching the component its widget will play animation right away, so we don't want the widget to follow the Target Character around
		DamageNumberWidgetComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageNumberWidgetComp->SetDamageTextProps(DamageTextProps);
	}
}



