// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "BehaviorTree/BTDecorator.h"
#include "CoreMinimal.h"

#include "ObsidianBTDecorator_CanActivateAbility.generated.h"

/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian Key"))
class OBSIDIAN_API UObsidianBTDecorator_CanActivateAbility : public UBTDecorator
{
	GENERATED_BODY()

public:
	UObsidianBTDecorator_CanActivateAbility();

	//~ Begin UBTNode Interface
	virtual FString GetStaticDescription() const override;
	//~ End UBTNode Interface

protected:
	//~ Begin UBTDecorator Interface
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& InOwnerComp, uint8* InNodeMemory) const override;
	//~ End UBTDecorator Interface

	/** Ability tag to check the activation for. */
	UPROPERTY(EditAnywhere, meta=(Categories="AbilityActivation"), Category = "Obsidian")
	FGameplayTag ActivateAbilityWithTag = FGameplayTag();

	/** Enemy Target, used to get tags from it. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "Target Actor"), Category = "Key")
	FBlackboardKeySelector TargetActor_Selector;

	UPROPERTY(EditAnywhere, Category = "Obsidian")
	bool bDebugEnabled = false;
};
