// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Characters/ObsidianCharacterBase.h"
#include "Interaction/ObsidianHighlightInterface.h"
#include "ObsidianEnemy.generated.h"

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

private:
	UPROPERTY()
	TObjectPtr<UObsidianAbilitySystemComponent> ObsidianAbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UObsidianCommonAttributeSet> CommonAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian|Hero", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UObsidianEnemyAttributesComponent> EnemyAttributesComponent;
};
