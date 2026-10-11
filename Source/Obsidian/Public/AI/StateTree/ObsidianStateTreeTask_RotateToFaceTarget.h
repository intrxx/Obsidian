// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "Tasks/StateTreeAITask.h"

#include "ObsidianStateTreeTask_RotateToFaceTarget.generated.h"

class AAIController;

/**
 *
 */
USTRUCT()
struct FObsidianStateTreeTask_RotateToFaceTargetInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> AIController = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> Target = nullptr;

	/** Target the Focus was set on by this Task, it is the only Focus this Task is allowed to clear. */
	UPROPERTY()
	TObjectPtr<AActor> FocusActorSet = nullptr;
};

/**
 * Task for rotating the Pawn of the AI Controller to face the Target, done by setting the Focus on it for as long as
 * the Task is running. Succeeds once the Pawn faces the Target with given Precision, fails if there is no Target.
 */
USTRUCT(meta = (DisplayName = "Rotate To Face Target", Category = "Obsidian"))
struct OBSIDIAN_API FObsidianStateTreeTask_RotateToFaceTarget : public FStateTreeAITaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FObsidianStateTreeTask_RotateToFaceTargetInstanceData;

public:
	//~ Begin FStateTreeNodeBase Interface
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& InID, FStateTreeDataView InInstanceDataView, const IStateTreeBindingLookup& InBindingLookup,
		EStateTreeNodeFormatting InFormatting = EStateTreeNodeFormatting::Text) const override;
#endif
	//~ End FStateTreeNodeBase Interface

	//~ Begin FStateTreeTaskBase Interface
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& InContext, const FStateTreeTransitionResult& InTransition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& InContext, const float InDeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& InContext, const FStateTreeTransitionResult& InTransition) const override;
	//~ End FStateTreeTaskBase Interface

	/** Success condition precision in degrees. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"), Category = "Obsidian")
	float Precision = 10.0f;

private:
	bool IsFacingLocation(const AAIController& InAIController, const FVector& InLocation) const;

	void CleanUp(FInstanceDataType& InOutInstanceData) const;
};
