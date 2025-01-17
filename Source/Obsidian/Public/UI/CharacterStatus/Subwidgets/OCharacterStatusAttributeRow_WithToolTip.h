// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/CharacterStatus/Subwidgets/OCharacterStatusAttributeRow.h"
#include "OCharacterStatusAttributeRow_WithToolTip.generated.h"

class UObsidianAttributeToolTip;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UOCharacterStatusAttributeRow_WithToolTip : public UOCharacterStatusAttributeRow
{
	GENERATED_BODY()

public:
	virtual void BeginDestroy() override;

	/** Sets the CharacterStatus variable on AttributeRow and bind callback for destroying the tooltip on closing the CharacterStatus. */
	void SetCharacterStatus(UObsidianCharacterStatus* InCharacterStatus);

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnToolTipButtonHovered();
	UFUNCTION()
	void OnToolTipButtonUnHovered();

	void HandleCharacterStatusSet();
	
protected:	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Tooltip_Button;

	UPROPERTY(EditAnywhere, Category = "Obsidian|ToolTip")
	TSubclassOf<UObsidianAttributeToolTip> AttributeToolTipWidgetClass;

	UPROPERTY()
	TObjectPtr<UObsidianAttributeToolTip> AttributeToolTipWidget;

	UPROPERTY()
	TObjectPtr<UObsidianCharacterStatus> CharacterStatus;

	UPROPERTY(EditAnywhere, Category = "Obsidian|ToolTip")
	FText AttributeDescription = FText::FromString("Attribute Description");

	/** Whether the button is hit testable. */
	UPROPERTY(EditAnywhere, Category = "Obsidian|Properties")
	bool bToolTipButtonEnabled = false;
};
