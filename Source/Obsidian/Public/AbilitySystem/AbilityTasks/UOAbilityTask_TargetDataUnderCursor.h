// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "Abilities/Tasks/AbilityTask.h"
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
	UFUNCTION(BlueprintCallable, meta=( BlueprintInternalUseOnly = "TRUE", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility",
		DisplayName = "Get Target Data Under Cursor"), Category = "Obsidian|GameplayTasks")
	static UOAbilityTask_TargetDataUnderCursor* CreateTargetDataUnderCursorProxy(UGameplayAbility* OwningAbility);

	UPROPERTY(BlueprintAssignable)
	FCursorTargetDataSignature DataReceived;

private:
	virtual void Activate() override;

	void BroadcastTargetDataToServer();
	void ReceiveTargetDataFromClient();
	
	void OnTargetDataSetCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);
};
