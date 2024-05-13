// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetController.h"
#include "OCharacterStatusWidgetController.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UOCharacterStatusWidgetController : public UObsidianWidgetController
{
	GENERATED_BODY()

public:

protected:
	virtual void HandleBindingCallbacks(UObsidianAbilitySystemComponent* ObsidianASC) override;
	virtual void SetInitialAttributeValues() const override;
	
};
