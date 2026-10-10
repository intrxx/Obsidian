// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CoreMinimal.h"

#include "ObsidianTypes/ObsidianCoreTypes.h"

#include "ObsidianAdvancedAttackNotify.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianAdvancedAttackNotify : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* InMeshComp, UAnimSequenceBase* InAnimation, float InTotalDuration, const FAnimNotifyEventReference& InEventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* InMeshComp, UAnimSequenceBase* InAnimation, const FAnimNotifyEventReference& InEventReference) override;

	virtual FString GetNotifyName_Implementation() const override;

protected:
	UPROPERTY(EditAnywhere, Category = Obsidian)
	EObsidianTraceType TraceType = EObsidianTraceType::SimpleLineTrace;

	UPROPERTY(EditAnywhere, Category = Obsidian)
	EObsidianTracedMeshType TracedMeshType = EObsidianTracedMeshType::CharacterMesh;

	/** The trace will only return one hit event to the specific actor hit. */
	UPROPERTY(EditAnywhere, Category = Obsidian)
	bool bAllowOneHitPerTrace = true;
};

