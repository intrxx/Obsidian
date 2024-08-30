// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/AbilityTasks/UOAbilityTask_TargetDataUnderCursor.h"

#include "AbilitySystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Obsidian/Obsidian.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"

UOAbilityTask_TargetDataUnderCursor* UOAbilityTask_TargetDataUnderCursor::CreateTargetDataUnderCursorProxy(UGameplayAbility* OwningAbility)
{
	UOAbilityTask_TargetDataUnderCursor* AbilityTaskObj = NewAbilityTask<UOAbilityTask_TargetDataUnderCursor>(OwningAbility);
	
	return AbilityTaskObj;
}

void UOAbilityTask_TargetDataUnderCursor::Activate()
{
	if (Ability == nullptr)
	{
		return;
	}
	
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if(bIsLocallyControlled)
	{
		BroadcastTargetDataToServer();	
	}
	else
	{
		ReceiveTargetDataFromClient();
	}
}

void UOAbilityTask_TargetDataUnderCursor::BroadcastTargetDataToServer()
{
	FScopedPredictionWindow ScopedPredictionWindow(AbilitySystemComponent.Get());
	
	const APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	if(PC == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("PC is null on [%s]"), *GetNameSafe(this));
		return;
	}
	
	FHitResult HitResult;
	PC->GetHitResultUnderCursor(Obsidian_TraceChannel_PlayerCursorTrace, false, HitResult);
	
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = HitResult;
	
	FGameplayAbilityTargetDataHandle DataHandle;
	DataHandle.Add(Data);
	
	AbilitySystemComponent->ServerSetReplicatedTargetData(GetAbilitySpecHandle(),GetActivationPredictionKey(),
		DataHandle, FGameplayTag(), AbilitySystemComponent->ScopedPredictionKey);

	if(ShouldBroadcastAbilityTaskDelegates())
	{
		DataReceived.Broadcast(DataHandle);
	}
}

void UOAbilityTask_TargetDataUnderCursor::ReceiveTargetDataFromClient()
{
	// Bind the function to receive target data when the server gets it
	const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
	const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
	AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &ThisClass::OnTargetDataSetCallback);

	// In the case when the date got to the server before we reach this point, we can call the delegate manually 
	const bool bCalledDelegate = AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
	if(!bCalledDelegate)
	{
		SetWaitingOnRemotePlayerData();
	}
}

void UOAbilityTask_TargetDataUnderCursor::OnTargetDataSetCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	if(ShouldBroadcastAbilityTaskDelegates())
	{
		DataReceived.Broadcast(DataHandle);
	}
}
