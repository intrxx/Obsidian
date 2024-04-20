// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Characters/ObsidianCharacterBase.h"
#include "Interaction/ObsidianHighlightInterface.h"
#include "ObsidianEnemy.generated.h"

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

protected:
	virtual void BeginPlay() override;

	//~ Start of AObsidianCharacterBase
	virtual void OnAbilitySystemInitialized() override;
	virtual void OnAbilitySystemUninitialized() override;
	//~ End of AObsidianCharacterBase

private:
	UPROPERTY()
	TObjectPtr<UObsidianAbilitySystemComponent> ObsidianAbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UObsidianCommonAttributeSet> CommonAttributeSet;
	UPROPERTY()
	TObjectPtr<UObsidianEnemyAttributeSet> EnemyAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Hero", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UObsidianEnemyAttributesComponent> EnemyAttributesComponent;
};
