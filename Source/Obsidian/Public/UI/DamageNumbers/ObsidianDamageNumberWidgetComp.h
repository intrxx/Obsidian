// Copyright 2024 out of sCope team - Michał Ogiński
#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "ObsidianTypes/ObsidianUITypes.h"
#include "ObsidianDamageNumberWidgetComp.generated.h"

/**
*
 */
UCLASS()
class OBSIDIAN_API UObsidianDamageNumberWidgetComp : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDamageTextProps(const FObsidianDamageTextProps& DamageTextProps);
};

