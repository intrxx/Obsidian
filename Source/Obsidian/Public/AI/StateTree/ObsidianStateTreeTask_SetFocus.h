// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "Tasks/StateTreeAITask.h"

#include "ObsidianStateTreeTask_SetFocus.generated.h"

class AAIController;

/**
 *
 */
USTRUCT()
struct FObsidianStateTreeTask_SetFocusInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AAIController> AIController = nullptr;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> FocusTarget = nullptr;
};

/**
 * Task for keeping the Focus of the AI Controller on the Target for as long as the State is active, the Focus is
 * cleared when the State is exited. Runs in the background, so it never completes the State.
 */
USTRUCT(meta = (DisplayName = "Set Focus", Category = "Obsidian"))
struct OBSIDIAN_API FObsidianStateTreeTask_SetFocus : public FStateTreeAITaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FObsidianStateTreeTask_SetFocusInstanceData;

public:
	FObsidianStateTreeTask_SetFocus();

	//~ Begin FStateTreeNodeBase Interface
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& InID, FStateTreeDataView InInstanceDataView, const IStateTreeBindingLookup& InBindingLookup,
		EStateTreeNodeFormatting InFormatting = EStateTreeNodeFormatting::Text) const override;
#endif
	//~ End FStateTreeNodeBase Interface

	//~ Begin FStateTreeTaskBase Interface
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& InContext, const FStateTreeTransitionResult& InTransition) const override;
	virtual void ExitState(FStateTreeExecutionContext& InContext, const FStateTreeTransitionResult& InTransition) const override;
	//~ End FStateTreeTaskBase Interface
};
