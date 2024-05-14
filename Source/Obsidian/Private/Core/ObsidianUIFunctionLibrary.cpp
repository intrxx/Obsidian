// Copyright 2024 out of sCope team - Michał Ogiński

#include "Core/ObsidianUIFunctionLibrary.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Characters/Player/ObsidianPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ObsidianHUD.h"
#include "UI/ObsidianWidgetController.h"

UMainOverlayWidgetController* UObsidianUIFunctionLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if(WorldContextObject == nullptr)
	{
		return nullptr;
	}

	if(APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if(AObsidianHUD* ObsidianHUD = Cast<AObsidianHUD>(PC->GetHUD()))
		{
			AObsidianPlayerState* ObsidianPS = PC->GetPlayerState<AObsidianPlayerState>();
			UObsidianAbilitySystemComponent* ObsidianASC = ObsidianPS->GetObsidianAbilitySystemComponent();
			UObsidianHeroAttributesComponent* AttributesComponent = UObsidianHeroAttributesComponent::FindAttributesComponent(PC->GetOwner());
			
			const FWidgetControllerParams Params(PC, ObsidianPS, ObsidianASC, AttributesComponent);
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
		if(AObsidianHUD* ObsidianHUD = Cast<AObsidianHUD>(ObsidianPC->GetHUD()))
		{
			if(AObsidianPlayerState* ObsidianPS = ObsidianPC->GetObsidianPlayerState())
			{
				UObsidianAbilitySystemComponent* ObsidianASC = ObsidianPS->GetObsidianAbilitySystemComponent();
				UObsidianHeroAttributesComponent* AttributesComponent = UObsidianHeroAttributesComponent::FindAttributesComponent(ObsidianPC->GetPawn());
			
				const FWidgetControllerParams Params(ObsidianPC, ObsidianPS, ObsidianASC, AttributesComponent);
				return ObsidianHUD->GetCharacterStatusWidgetController(Params);
			}
		}
	}
	return nullptr;
}