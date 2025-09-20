// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianTypes/ObsidianCoreTypes.h"

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ObsidianAdvancedAttackNotify.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianAdvancedAttackNotify : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

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

