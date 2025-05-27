// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "UI/ObsidianWidgetBase.h"
#include "ObsidianDamageNumber.generated.h"

class UCommonTextStyle;
struct FObsidianDamageTextProps;
class UCommonTextBlock;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianDamageNumber : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|DamageNumber")
	void InitializeDamageNumber(const FObsidianDamageTextProps& DamageTextProps);
	
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> DamageNumber_TextBlock;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Styles")
	TSubclassOf<UCommonTextStyle> RegularDamageNumber_Style;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Styles")
	TSubclassOf<UCommonTextStyle> BlockedDamageNumber_Style;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Styles")
	FText BlockedText;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Styles")
	TSubclassOf<UCommonTextStyle> EvadedDamageNumber_Style;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Styles")
	FText EvadedText;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Styles")
	TSubclassOf<UCommonTextStyle> ImmuneDamageNumber_Style;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Styles")
	FText ImmuneText;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Styles")
	TSubclassOf<UCommonTextStyle> SuppressedDamageNumber_Style;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Styles")
	TSubclassOf<UCommonTextStyle> CriticalDamageNumber_Style;

protected:
	void SetDamageNumber(const TSubclassOf<UCommonTextStyle>& Style, const FText& Text) const;
};
