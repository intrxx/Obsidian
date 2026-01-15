// Copyright 2024 out of sCope team - Michał Ogiński

#include "Core/FunctionLibraries/ObsidianUIFunctionLibrary.h"

#include <Kismet/GameplayStatics.h>

#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Characters/Player/ObsidianPlayerState.h"
#include "UI/ObsidianHUD.h"
#include "UI/ObsidianWidgetControllerBase.h"
#include "UI/FrontEnd/ObsidianFrontEndHUD.h"

UObMainOverlayWidgetController* UObsidianUIFunctionLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if(WorldContextObject == nullptr)
	{
		return nullptr;
	}

	if(AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(
		UGameplayStatics::GetPlayerController(WorldContextObject, 0)))
	{
		if(AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD())
		{
			if (AObsidianPlayerState* ObsidianPS = ObsidianPC->GetObsidianPlayerState())
			{
				FObsidianWidgetControllerParams Params;
				Params.ObsidianPlayerController = ObsidianPC;
				Params.ObsidianPlayerState = ObsidianPS;
				
				return ObsidianHUD->GetMainOverlayWidgetController(Params);	
			}
		}
	}

	return nullptr;
}

UObCharacterStatusWidgetController* UObsidianUIFunctionLibrary::GetCharacterStatusWidgetController(const UObject* WorldContextObject)
{
	if(WorldContextObject == nullptr)
	{
		return nullptr;
	}

	if(AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(
		UGameplayStatics::GetPlayerController(WorldContextObject, 0)))
	{
		if(AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD())
		{
			if(AObsidianPlayerState* ObsidianPS = ObsidianPC->GetObsidianPlayerState())
			{
				FObsidianWidgetControllerParams Params;
				Params.ObsidianPlayerController = ObsidianPC;
				Params.ObsidianPlayerState = ObsidianPS;
				
				return ObsidianHUD->GetCharacterStatusWidgetController(Params);
			}
		}
	}
	return nullptr;
}

UObInventoryItemsWidgetController* UObsidianUIFunctionLibrary::GetInventoryItemsWidgetController(const UObject* WorldContextObject)
{
	if(WorldContextObject == nullptr)
	{
		return nullptr;
	}

	if(AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(
		UGameplayStatics::GetPlayerController(WorldContextObject, 0)))
	{
		if(AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD())
		{
			if(AObsidianPlayerState* ObsidianPS = ObsidianPC->GetObsidianPlayerState())
			{
				FObsidianWidgetControllerParams Params;
				Params.ObsidianPlayerController = ObsidianPC;
				Params.ObsidianPlayerState = ObsidianPS;
				
				return ObsidianHUD->GetInventoryItemsWidgetController(Params);
			}
		}
	}
	return nullptr;
}

UObCharacterSelectionWidgetController* UObsidianUIFunctionLibrary::GetCharacterSelectionWidgetController(
	const UObject* WorldContextObject)
{
	if(WorldContextObject == nullptr)
	{
		return nullptr;
	}

	if(AObsidianPlayerController* PlayerController = Cast<AObsidianPlayerController>(
		UGameplayStatics::GetPlayerController(WorldContextObject, 0)))
	{
		if(AObsidianFrontEndHUD* FrontEndHUD = Cast<AObsidianFrontEndHUD>(PlayerController->GetHUD()))
		{
			FObsidianWidgetControllerParams Params;
			Params.ObsidianPlayerController = PlayerController;
			Params.ObsidianLocalPlayer = Cast<UObsidianLocalPlayer>(PlayerController->GetLocalPlayer());
			
			return FrontEndHUD->GetCharacterSelectionWidgetController(Params);
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

