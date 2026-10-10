// Copyright 2026 out of sCope team - intrxx

#include "Animation/Notifies/ObsidianAdvancedAttackNotify.h"

#include "CharacterComponents/ObsidianAdvancedCombatComponent.h"
#include "Obsidian/ObsidianLogCategories.h"


void UObsidianAdvancedAttackNotify::NotifyBegin(USkeletalMeshComponent* InMeshComp, UAnimSequenceBase* InAnimation,
                                                float InTotalDuration, const FAnimNotifyEventReference& InEventReference)
{
	Super::NotifyBegin(InMeshComp, InAnimation, InTotalDuration, InEventReference);

	if(InMeshComp == nullptr)
	{
		UE_LOG(ObLogCombat, Error, TEXT("MeshComp is invalid on ObsidianAdvancedAttackNotify."));
		return;
	}

	if(const AActor* Owner = InMeshComp->GetOwner())
	{
		UObsidianAdvancedCombatComponent* ObsidianAdvancedCombatComp = UObsidianAdvancedCombatComponent::FindAdvancedCombatComponent(Owner);
		if(!IsValid(ObsidianAdvancedCombatComp))
		{
			UE_LOG(ObLogCombat, Error, TEXT("ObsidianAdvancedCombatComponent is invalid for [%s]."), *GetNameSafe(Owner));
			return;
		}
		FObsidianAdvancedTraceParams TraceParams;
		TraceParams.TraceType = TraceType;
		TraceParams.TracedMeshType = TracedMeshType;
		TraceParams.bAllowOneHitPerTrace = bAllowOneHitPerTrace;
		
		ObsidianAdvancedCombatComp->StartTrace(TraceParams);
	}
}

void UObsidianAdvancedAttackNotify::NotifyEnd(USkeletalMeshComponent* InMeshComp, UAnimSequenceBase* InAnimation,
	const FAnimNotifyEventReference& InEventReference)
{
	Super::NotifyEnd(InMeshComp, InAnimation, InEventReference);

	if(InMeshComp == nullptr)
	{
		UE_LOG(ObLogCombat, Error, TEXT("MeshComp is invalid on ObsidianAdvancedAttackNotify."));
		return;
	}

	if(const AActor* Owner = InMeshComp->GetOwner())
	{
		UObsidianAdvancedCombatComponent* ObsidianAdvancedCombatComp = UObsidianAdvancedCombatComponent::FindAdvancedCombatComponent(Owner);
		if(!IsValid(ObsidianAdvancedCombatComp))
		{
			UE_LOG(ObLogCombat, Error, TEXT("ObsidianAdvancedCombatComponent is invalid for [%s]."), *GetNameSafe(Owner));
			return;
		}
		ObsidianAdvancedCombatComp->StopTrace();
	}
}

FString UObsidianAdvancedAttackNotify::GetNotifyName_Implementation() const
{
	return FString("Advanced Combat Trace Notify");
}
