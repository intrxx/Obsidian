// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "CommonActivatableWidget.h"
#include "ObsidianActivatableWidget.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EObsidianWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class OBSIDIAN_API UObsidianActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	UObsidianActivatableWidget(const FObjectInitializer& ObjectInitializer);

public:
	//~ Start of UCommonActivatableWidget interface
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	//~ End of UCommonActivatableWidget interface

#if WITH_EDITOR
	virtual void ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, class IWidgetCompilerLog& CompileLog) const override;
#endif

protected:
	/** The desired input mode to use while this UI is activated, for example do you want key presses to still reach the game/player controller? */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Input")
	EObsidianWidgetInputMode InputConfig = EObsidianWidgetInputMode::Default;

	/** The desired mouse behavior when the game gets input. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Input")
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
};
