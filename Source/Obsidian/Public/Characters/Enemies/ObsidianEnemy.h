// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "AI/ObsidianEnemyInterface.h"
#include "Characters/ObsidianCharacterBase.h"
#include "Interaction/ObsidianHighlightInterface.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"

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
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API AObsidianEnemy : public AObsidianCharacterBase, public IObsidianHighlightInterface, public IObsidianEnemyInterface
{
	GENERATED_BODY()

public:
	AObsidianEnemy(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	bool IsHighlighted() const;
	EObsidianEntityRarity GetEnemyRarity() const { return EnemyRarity; }

	//~ Begin IObsidianHighlightInterface Interface
	virtual AActor* GetHighlightAvatarActor() override;
	virtual void StartHighlight() override;
	virtual void StopHighlight() override;
	//~ End IObsidianHighlightInterface Interface

	//~ Begin IObsidianCombatInterface Interface
	virtual bool IsDeadOrDying_Implementation() const override;
	virtual AActor* GetAvatarActor_Implementation() override;
	virtual uint8 GetCharacterLevel() override { return EnemyLevel; }
	//~ End IObsidianCombatInterface Interface

	//~ Begin IObsidianEnemyInterface Interface
	virtual void SetCombatTarget_Implementation(AActor* InTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	virtual EObsidianEntityRarity GetEnemyRarityFromOwner_Implementation() const override;
	//~ End IObsidianEnemyInterface Interface

	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

	UPROPERTY(VisibleAnywhere, Category = "Obsidian|Enemy")
	bool bHitReacting = false;

protected:
	//~ Begin AObsidianCharacterBase Interface
	virtual void OnAbilitySystemInitialized() override;
	virtual void OnAbilitySystemUninitialized() override;
	virtual void OnDeathStarted(AActor* InOwningActor) override;
	virtual void OnDeathFinished(AActor* InOwningActor) override;
	//~ End AObsidianCharacterBase Interface

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
#if WITH_OBSIDIAN_DEBUG_MENU
	friend class FObsidianDebugTab_AI;
#endif

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
