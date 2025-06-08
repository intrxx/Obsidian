// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/OOSInventoryItemsManagerComponent.h"

// ~ Core
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "UserSettings/EnhancedInputUserSettings.h"

// ~ Plugin
#include "Input/OOSInventoryItemsInputDefinition.h"

UOOSInventoryItemsManagerComponent::UOOSInventoryItemsManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	
}

void UOOSInventoryItemsManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UOOSInventoryItemsManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UOOSInventoryItemsManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UOOSInventoryItemsManagerComponent::InitializePlayerInventoryInput(UInputComponent* InputComponent)
{
	check(InputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if(Pawn == nullptr)
	{
		return;
	}

	APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = Cast<ULocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(EnhancedInputSubsystem);

	EnhancedInputSubsystem->ClearAllMappings();

	if(InventoryItemsInputDefinition)
	{
		for(const FOSSInputMappingContextAndPriority& Mapping : InventoryItemsInputDefinition->InventoryItemsDefaultMappingContext)
		{
			if(UInputMappingContext* IMC = Mapping.InputMapping)
			{
				if(Mapping.bRegisterWithSettings)
				{
					if(UEnhancedInputUserSettings* Settings = EnhancedInputSubsystem->GetUserSettings())
					{
						Settings->RegisterInputMappingContext(IMC);
					}

					FModifyContextOptions Options = {};
					Options.bIgnoreAllPressedKeysUntilRelease = false;

					EnhancedInputSubsystem->AddMappingContext(IMC, Mapping.Priority, Options);
				}
			}
		}

		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
		if(ensureMsgf(EnhancedInputComponent, TEXT("Unexpected Input Component.")))
		{
			if(ensureMsgf(InventoryItemsInputDefinition->DropItemInputAction, TEXT("Invalid Drop Item Input Action.")))
			{
				EnhancedInputComponent->BindAction(InventoryItemsInputDefinition->DropItemInputAction, ETriggerEvent::Triggered, this, &ThisClass::Input_DropItem);
			}
			if(ensureMsgf(InventoryItemsInputDefinition->ReleaseUsingItemInputAction, TEXT("Invalid Release Using Item Input Action.")))
			{
				EnhancedInputComponent->BindAction(InventoryItemsInputDefinition->ReleaseUsingItemInputAction, ETriggerEvent::Triggered, this, &ThisClass::Input_ReleaseUsingItem);
			}
			if(ensureMsgf(InventoryItemsInputDefinition->ReleaseContinuouslyUsingItemInputAction, TEXT("Invalid Release Using Item Input Action.")))
			{
				EnhancedInputComponent->BindAction(InventoryItemsInputDefinition->ReleaseContinuouslyUsingItemInputAction, ETriggerEvent::Completed, this, &ThisClass::Input_ReleaseUsingItem);
			}
			if(ensureMsgf(InventoryItemsInputDefinition->WeaponSwapInputAction, TEXT("Invalid Weapon Swap Input Action.")))
			{
				EnhancedInputComponent->BindAction(InventoryItemsInputDefinition->WeaponSwapInputAction, ETriggerEvent::Triggered, this, &ThisClass::Input_WeaponSwap);
			}
		}
	}
	
}

void UOOSInventoryItemsManagerComponent::Input_DropItem()
{
}

void UOOSInventoryItemsManagerComponent::Input_ReleaseUsingItem()
{
}

void UOOSInventoryItemsManagerComponent::Input_WeaponSwap()
{
}

