// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "AI/ObsidianEnemyInterface.h"
#include "Interaction/ObsidianHighlightInterface.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"

#include "Characters/ObsidianCharacterBase.h"
#include "ObsidianEnemy.generated.h"

class UObsidianItemDropComponent;
class UObsidianEnemyOverlayBarComponent;
class UBehaviorTree;
class UWidgetComponent;
class UObsidianEnemyAttributeSet;
class UObsidianEnemyAttributesComponent;
class UObsidianCommonAttributeSet;
class UObsidianAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianEnemy : public AObsidianCharacterBase, public IObsidianHighlightInterface, public IObsidianEnemyInterface
{
	GENERATED_BODY()

public:
	AObsidianEnemy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	//~ Start of HighlightInterface
	virtual AActor* GetHighlightAvatarActor() override;
	virtual void StartHighlight() override;
	virtual void StopHighlight() override;
	//~ End of HighlightInterface

	bool IsHighlighted() const;
	EObsidianEntityRarity GetEnemyRarity() const;

public:
	UPROPERTY(VisibleAnywhere, Category = "Obsidian|Enemy")
	bool bHitReacting = false;

protected:
	virtual void BeginPlay() override;

	//~ Start of AObsidianCharacterBase
	virtual void OnAbilitySystemInitialized() override;
	virtual void OnAbilitySystemUninitialized() override;
	virtual void OnDeathStarted(AActor* OwningActor) override;
	virtual void OnDeathFinished(AActor* OwningActor) override;
	//~ End of AObsidianCharacterBase

	//~ Start of CombatInterface
	virtual bool IsDeadOrDying_Implementation() const override;
	virtual AActor* GetAvatarActor_Implementation() override;
	virtual uint8 GetCharacterLevel() override;
	//~ End of CombatInterface
	
	//~ Start of EnemyInterface
	virtual void SetCombatTarget_Implementation(AActor* InTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	virtual EObsidianEntityRarity GetEnemyRarityFromOwner_Implementation() const override;
	//~ End of EnemyInterface

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UObsidianEnemyAttributesComponent> EnemyAttributesComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UObsidianEnemyOverlayBarComponent> EnemyOverlayBarComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UObsidianItemDropComponent> ItemDropComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Obsidian|AI")
	TObjectPtr<UBehaviorTree> DefaultBehaviorTree;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Defaults")
	EObsidianEnemyClass EnemyClass = EObsidianEnemyClass::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Defaults")
	EObsidianEntityRarity EnemyRarity = EObsidianEntityRarity::None;
	
	UPROPERTY(BlueprintReadWrite, Category = "Obsidian|Combat")
	TObjectPtr<AActor> CombatTarget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Obsidian|Defaults")
	FText EnemyName = FText::FromString("Lorem");

private:
	UPROPERTY()
	TObjectPtr<UObsidianAbilitySystemComponent> ObsidianAbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UObsidianEnemyAttributeSet> EnemyAttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Obsidian")
	bool bShouldSpawnDummyMesh = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true, EditCondition = "bShouldSpawnDummyMesh", EditConditionHides), Category = "Obsidian")
	float DummyMeshLifeSpan = 25.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "Obsidian|Level")
	int32 EnemyLevel = 1;
};
