// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Characters/ObsidianCharacterBase.h"
#include "Interaction/ObsidianHighlightInterface.h"
#include "ObsidianTypes/ObsidianEnemyType.h"
#include "ObsidianEnemy.generated.h"

class UWidgetComponent;
class UObsidianEnemyAttributeSet;
class UObsidianEnemyAttributesComponent;
class UObsidianCommonAttributeSet;
class UObsidianAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianEnemy : public AObsidianCharacterBase, public IObsidianHighlightInterface
{
	GENERATED_BODY()

public:
	AObsidianEnemy();
	
	virtual void StartHighlight() override;
	virtual void StopHighlight() override;
	
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

public:
	UPROPERTY(VisibleAnywhere, Category = "Obsidian|Enemy")
	bool bHitReacting = false;

protected:
	virtual void BeginPlay() override;

	//~ Start of AObsidianCharacterBase
	virtual void OnAbilitySystemInitialized() override;
	virtual void OnAbilitySystemUninitialized() override;
	//~ End of AObsidianCharacterBase

	//~ Start of CombatInterface
	virtual int32 GetCharacterLevel() override;
	//~ End of CombatInterface

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Defaults")
	EObsidianEnemyClass EnemyClass = EObsidianEnemyClass::EEC_MAX;

private:
	void CreateHealthBarWidget();
	
private:
	UPROPERTY()
	TObjectPtr<UObsidianAbilitySystemComponent> ObsidianAbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UObsidianCommonAttributeSet> CommonAttributeSet;
	UPROPERTY()
	TObjectPtr<UObsidianEnemyAttributeSet> EnemyAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UObsidianEnemyAttributesComponent> EnemyAttributesComponent;

	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> HealthBarWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Obsidian")
	int32 EnemyLevel = 1;
};
