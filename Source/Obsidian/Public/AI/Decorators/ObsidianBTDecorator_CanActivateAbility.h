// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "ObsidianBTDecorator_CanActivateAbility.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianBTDecorator_CanActivateAbility : public UBTDecorator
{
	GENERATED_BODY()

public:
	UObsidianBTDecorator_CanActivateAbility();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
	virtual FString GetStaticDescription() const override;
	
protected:
	/** Ability tag to check the activation for. */
	UPROPERTY(EditAnywhere, meta=(Categories="AbilityActivation"), Category = "Obsidian")
	FGameplayTag ActivateAbilityWithTag = FGameplayTag();
	
	/** Enemy Target. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "Target Actor"), Category = "Key")
	FBlackboardKeySelector TargetActor_Selector;

	UPROPERTY(EditAnywhere, Category = "Obsidian")
	bool bDebugEnabled = false;
};
