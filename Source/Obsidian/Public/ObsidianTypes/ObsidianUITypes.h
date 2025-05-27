// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "GameplayEffect.h"
#include "ObsidianUITypes.generated.h"

class UObsidianDurationalEffectInfo;
class UObsidianEffectInfoBase;
class UOStackingDurationalEffectInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeValueChangedSignature, float, NewValue);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHeroLevelUpSignature, const int32 NewLevel);
DECLARE_DELEGATE_OneParam(FOnAttributeValueChangedOneParam, const float /** Value */);
DECLARE_DELEGATE_TwoParams(FOnAttributeValueChangedTwoParams, const float /** Value 1 */, const float /** Value 2 */);

/*
 * Struct that describes style of the Special Resource Globe for specific Hero characters.
 */
USTRUCT(BlueprintType)
struct FObsidianSpecialResourceVisuals
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FText SpecialResourceName = FText();

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FSlateBrush SpecialResourceGlobeMaterial = FSlateBrush();
};


/*
 * Struct that ties the specific Effect Tag to Fill image so the system can for example replace the Red Progress globe with green one for poison.
 */
USTRUCT(BlueprintType)
struct FObsidianProgressBarEffectFillImage
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta=(Categories="UI"))
	FGameplayTag EffectTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly)
	FSlateBrush ProgressBarFillImage = FSlateBrush();
};

/**
 * Types of Gameplay Effects used by widgets to display with different kind of components.
 */
UENUM(BlueprintType)
enum class EObsidianInfoWidgetType : uint8
{
	EIWT_SimpleEffectInfo UMETA(DisplayName = "Simple Effect Info"),
	EIWT_DurationalEffectInfo UMETA(DisplayName = "Durational Effect Info"),
	EIWT_StackingEffectInfo UMETA(DisplayName = "Stacking Effect Info"),
	EIWT_StackingDurationalEffectInfo UMETA(DisplayName = "Stacking Durational Effect Info"),

	EIWT_MAX UMETA(DisplayName = "Default MAX") 
};

/**
 * Enum defining the container to assign the effect when displaying it on the screen so the Player can quickly spot any bad effects he needs to be aware of.
 */
UENUM(BlueprintType)
enum class EObsidianUIEffectClassification : uint8
{
	EUEC_NoClassification UMETA(DisplayName = "No Classification"), 
	EUEC_Buff UMETA(DisplayName = "Buff"), 
	EUEC_Debuff UMETA(DisplayName = "Debuff"), 

	EUEC_MAX UMETA(DisplayName = "Default MAX") 
};

/**
 * Struct that defines the Gameplay Effect data that is needed for displaying it on the Player's overlay.
 */
USTRUCT(BlueprintType)
struct FObsidianEffectUIDataWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EObsidianInfoWidgetType InfoWidgetType = EObsidianInfoWidgetType::EIWT_SimpleEffectInfo;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag EffectTag = FGameplayTag();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EObsidianUIEffectClassification EffectClassification = EObsidianUIEffectClassification::EUEC_NoClassification;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText EffectName = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText EffectDesc = FText();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> EffectImage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="InfoWidgetType == EObsidianInfoWidgetType::EIWT_SimpleEffectInfo", EditConditionHides))
	TSubclassOf<UObsidianEffectInfoBase> SimpleEffectWidget;

	/** Indicates that this effect is an Aura Gameplay Ability that will be cleared manually */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="InfoWidgetType == EObsidianInfoWidgetType::EIWT_SimpleEffectInfo", EditConditionHides))
	bool bAuraEffect = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="InfoWidgetType == EObsidianInfoWidgetType::EIWT_DurationalEffectInfo", EditConditionHides))
	TSubclassOf<UObsidianDurationalEffectInfo> DurationalEffectWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="InfoWidgetType == EObsidianInfoWidgetType::EIWT_StackingEffectInfo", EditConditionHides))
	TSubclassOf<UObsidianEffectInfoBase> StackingEffectWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(EditCondition="InfoWidgetType == EObsidianInfoWidgetType::EIWT_StackingDurationalEffectInfo", EditConditionHides))
	TSubclassOf<UOStackingDurationalEffectInfo> StackingDurationalEffectWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float EffectDuration = 0.f;
};

/**
 * Struct that defines Stacking Effect displayed on screen.
 */
USTRUCT(BlueprintType)
struct FObsidianEffectUIStackingData
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(BlueprintReadOnly)
	int32 EffectStackCount = 0;

	UPROPERTY(BlueprintReadOnly)
	EGameplayEffectStackingExpirationPolicy EffectExpirationDurationPolicy = EGameplayEffectStackingExpirationPolicy();

	UPROPERTY(BlueprintReadOnly)
	EGameplayEffectStackingDurationPolicy EffectStackingDurationPolicy = EGameplayEffectStackingDurationPolicy();
};

/** Broadcasts DataTable Row that corresponds to a given asset tag */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEffectUIDataWidgetRow, FObsidianEffectUIDataWidgetRow, Row);

/** Broadcasts DataTable Row that corresponds to a given asset tag as well as the stacking information */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStackingEffectUIDataWidgetRow, FObsidianEffectUIDataWidgetRow, Row, FObsidianEffectUIStackingData, StackingData);


/**
 * Struct used to define damage number text displayed on screen.
 */
USTRUCT(BlueprintType)
struct FObsidianDamageTextProps
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float DamageMagnitude = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	bool bIsBlockedAttack = false;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsCriticalAttack = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsEvadedHit = false;

	UPROPERTY(BlueprintReadOnly)
	bool bIsSuppressedSpell = false;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsTargetImmune = false;
};
