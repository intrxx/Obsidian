// Copyright 2024 out of sCope team - Michał Ogiński

#include "Characters/Player/ObsidianPlayerController.h"
#include "UI/ObsidianHUD.h"
#include "Characters/ObsidianCharacterBase.h"
#include "UI/DamageNumbers/ObsidianDamageNumberWidgetComp.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/Player/ObsidianPlayerState.h"
#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"

AObsidianPlayerController::AObsidianPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	InventoryComponent = CreateDefaultSubobject<UObsidianInventoryComponent>(TEXT("Inventory Component"));
	EquipmentComponent = CreateDefaultSubobject<UObsidianEquipmentComponent>(TEXT("Equipment Component"));
}

void AObsidianPlayerController::UpdateHoveredRegularEnemyTarget(AActor* TargetActor, const bool bHoveredOver) const
{
	OnEnemyActorHoveredDelegate.ExecuteIfBound(TargetActor, bHoveredOver);
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



