// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "UI/ObsidianWidgetBase.h"
#include "ObsidianMainWidgetBase.generated.h"

class UObsidianHeroComponent;
class AObsidianPlayerController;

/**
 * Base for widget that will be the main widget displayed on the screen. All widgets in Obsidian that wish to be the
 * first in the layer must be derived from this class!
 */
UCLASS()
class OBSIDIAN_API UObsidianMainWidgetBase : public UObsidianWidgetBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY()
	TObjectPtr<UObsidianHeroComponent> HeroComp;
};
