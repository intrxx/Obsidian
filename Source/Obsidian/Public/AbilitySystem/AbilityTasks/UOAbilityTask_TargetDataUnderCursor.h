// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "CoreMinimal.h"

#include "UOAbilityTask_TargetDataUnderCursor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCursorTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UOAbilityTask_TargetDataUnderCursor : public UAbilityTask
{
	GENERATED_BODY()

public:
	/** Gets the hit result under cursor and returns the target data */
	UFUNCTION(BlueprintCallable, meta=( BlueprintInternalUseOnly = "TRUE", HidePin = "InOwningAbility", DefaultToSelf = "InOwningAbility",
		DisplayName = "Get Target Data Under Cursor"), Category = "Obsidian|GameplayTasks")
	static UOAbilityTask_TargetDataUnderCursor* CreateTargetDataUnderCursorProxy(UGameplayAbility* InOwningAbility);

	UPROPERTY(BlueprintAssignable)
	FCursorTargetDataSignature DataReceived;

protected:
	//~ Begin UGameplayTask Interface
	virtual void Activate() override;
	//~ End UGameplayTask Interface

private:
	void BroadcastTargetDataToServer();
	void ReceiveTargetDataFromClient();

	void OnTargetDataSetCallback(const FGameplayAbilityTargetDataHandle& InDataHandle, FGameplayTag InActivationTag);
};
