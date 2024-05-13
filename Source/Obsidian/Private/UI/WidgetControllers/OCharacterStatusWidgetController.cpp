// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/WidgetControllers/OCharacterStatusWidgetController.h"

void UOCharacterStatusWidgetController::OnWidgetControllerSetupCompleted()
{
	UE_LOG(LogTemp, Error, TEXT("Set up completed"));
}

void UOCharacterStatusWidgetController::HandleBindingCallbacks(UObsidianAbilitySystemComponent* ObsidianASC)
{
}

void UOCharacterStatusWidgetController::SetInitialAttributeValues() const
{
	
}
