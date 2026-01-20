// Copyright 2026 out of sCope team - intrxx

#include "Animation/Notifies/ObsidianAdvancedAttackNotify.h"

#include "CharacterComponents/ObsidianAdvancedCombatComponent.h"
#include "Obsidian/ObsidianGameModule.h"

void UObsidianAdvancedAttackNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if(MeshComp == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("MeshComp is invalid on ObsidianAdvancedAttackNotify."));
		return;
	}

	if(const AActor* Owner = MeshComp->GetOwner())
	{
		UObsidianAdvancedCombatComponent* ObsidianAdvancedCombatComp = UObsidianAdvancedCombatComponent::FindAdvancedCombatComponent(Owner);
		if(!IsValid(ObsidianAdvancedCombatComp))
		{
			UE_LOG(LogObsidian, Error, TEXT("ObsidianAdvancedCombatComponent is invalid for [%s]."), *GetNameSafe(Owner));
			return;
		}
		FObsidianAdvancedTraceParams TraceParams;
		TraceParams.TraceType = TraceType;
		TraceParams.TracedMeshType = TracedMeshType;
		TraceParams.bAllowOneHitPerTrace = bAllowOneHitPerTrace;
		
		ObsidianAdvancedCombatComp->StartTrace(TraceParams);
	}
}

void UObsidianAdvancedAttackNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if(MeshComp == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("MeshComp is invalid on ObsidianAdvancedAttackNotify."));
		return;
	}

	if(const AActor* Owner = MeshComp->GetOwner())
	{
		UObsidianAdvancedCombatComponent* ObsidianAdvancedCombatComp = UObsidianAdvancedCombatComponent::FindAdvancedCombatComponent(Owner);
		if(!IsValid(ObsidianAdvancedCombatComp))
		{
			UE_LOG(LogObsidian, Error, TEXT("ObsidianAdvancedCombatComponent is invalid for [%s]."), *GetNameSafe(Owner));
			return;
		}
		ObsidianAdvancedCombatComp->StopTrace();
	}
}

FString UObsidianAdvancedAttackNotify::GetNotifyName_Implementation() const
{
	return FString("Advanced Combat Trace Notify");
}
