// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


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
	/** Sets the CharacterStatus variable on AttributeRow and bind callback for destroying the tooltip on closing the CharacterStatus. */
	void SetCharacterStatus(UObsidianCharacterStatus* InCharacterStatus);

protected:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
	void OnToolTipButtonHovered();
	void OnToolTipButtonUnHovered();

	void HandleCharacterStatusSet();
	
protected:	
	UPROPERTY(EditAnywhere, Category = "Obsidian|ToolTip")
	TSubclassOf<UObsidianAttributeToolTip> AttributeToolTipWidgetClass;

	UPROPERTY()
	TObjectPtr<UObsidianAttributeToolTip> AttributeToolTipWidget;

	UPROPERTY()
	TObjectPtr<UObsidianCharacterStatus> CharacterStatus;

	UPROPERTY(EditAnywhere, Category = "Obsidian|ToolTip")
	FText AttributeDescription = FText::FromString("Attribute Description");

	/** Whether the tooltip is enabled. */
	UPROPERTY(EditAnywhere, Category = "Obsidian|Properties")
	bool bToolTipEnabled = false;
};
