// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetController.h"
#include "OCharacterStatusWidgetController.generated.h"

class UObsidianAttributeInfo;
struct FOAttributeInfo;

DECLARE_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FOAttributeInfo& /** Attribute Info */);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class OBSIDIAN_API UOCharacterStatusWidgetController : public UObsidianWidgetController
{
	GENERATED_BODY()

public:
	// ~ Start of UObsidianWidgetController
	virtual void OnWidgetControllerSetupCompleted() override;
	virtual void SetInitialAttributeValues() const override;
	// ~ End of UObsidianWidgetController

public:
	FAttributeInfoSignature AttributeInfoDelegate;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TObjectPtr<UObsidianAttributeInfo> AttributeInfo;
	
protected:
	virtual void HandleBindingCallbacks(UObsidianAbilitySystemComponent* ObsidianASC) override;
	
	
};
