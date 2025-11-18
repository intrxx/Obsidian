// Copyright 2024 out of sCope team - Michał Ogiński

#include "Core/FunctionLibraries/ObsidianUIFunctionLibrary.h"

// ~ Core
#include "Kismet/GameplayStatics.h"

// ~ Project
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Characters/Player/ObsidianPlayerState.h"
#include "UI/ObsidianHUD.h"
#include "UI/ObsidianWidgetControllerBase.h"

UMainOverlayWidgetController* UObsidianUIFunctionLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if(WorldContextObject == nullptr)
	{
		return nullptr;
	}

	if(AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, 0)))
	{
		if(AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD())
		{
			AObsidianPlayerState* ObsidianPS = ObsidianPC->GetObsidianPlayerState();
			UObsidianAbilitySystemComponent* ObsidianASC = ObsidianPS->GetObsidianAbilitySystemComponent();
			UObsidianHeroAttributesComponent* AttributesComponent = UObsidianHeroAttributesComponent::FindAttributesComponent(ObsidianPC->GetOwner());
			
			const FObsidianWidgetControllerParams Params(ObsidianPC, ObsidianPS, ObsidianASC, AttributesComponent);
			return ObsidianHUD->GetMainOverlayWidgetController(Params);
		}
	}

	return nullptr;
}

UOCharacterStatusWidgetController* UObsidianUIFunctionLibrary::GetCharacterStatusWidgetController(const UObject* WorldContextObject)
{
	if(WorldContextObject == nullptr)
	{
		return nullptr;
	}

	if(AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, 0)))
	{
		if(AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD())
		{
			if(AObsidianPlayerState* ObsidianPS = ObsidianPC->GetObsidianPlayerState())
			{
				UObsidianAbilitySystemComponent* ObsidianASC = ObsidianPS->GetObsidianAbilitySystemComponent();
				UObsidianHeroAttributesComponent* AttributesComponent = UObsidianHeroAttributesComponent::FindAttributesComponent(ObsidianPC->GetPawn());
			
				const FObsidianWidgetControllerParams Params(ObsidianPC, ObsidianPS, ObsidianASC, AttributesComponent);
				return ObsidianHUD->GetCharacterStatusWidgetController(Params);
			}
		}
	}
	return nullptr;
}

UObsidianInventoryItemsWidgetController* UObsidianUIFunctionLibrary::GetInventoryItemsWidgetController(const UObject* WorldContextObject)
{
	if(WorldContextObject == nullptr)
	{
		return nullptr;
	}

	if(AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, 0)))
	{
		if(AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD())
		{
			if(AObsidianPlayerState* ObsidianPS = ObsidianPC->GetObsidianPlayerState())
			{
				UObsidianInventoryComponent* InventoryComponent = ObsidianPC->GetInventoryComponent();
				UObsidianEquipmentComponent* EquipmentComponent = ObsidianPC->GetEquipmentComponent();
				UObsidianPlayerStashComponent* PlayerStashComponent = ObsidianPC->GetPlayerStashComponent();
				const FObsidianWidgetControllerParams Params(ObsidianPC, ObsidianPS, nullptr, nullptr, InventoryComponent, EquipmentComponent, PlayerStashComponent);
				return ObsidianHUD->GetInventoryItemsWidgetController(Params);
			}
		}
	}
	return nullptr;
}

FVector2D UObsidianUIFunctionLibrary::GetGameViewportSize()
{
	FVector2D Result = FVector2D::Zero();
	if(GEngine == nullptr)
	{
		return Result;
	}
	
	if(const UGameViewportClient* GameViewport = GEngine->GameViewport)
	{
		GameViewport->GetViewportSize(/** OUT */ Result);
	}
	return Result;
}

