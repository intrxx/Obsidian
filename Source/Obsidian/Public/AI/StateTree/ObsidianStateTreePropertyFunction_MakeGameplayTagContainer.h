// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StateTreePropertyFunctionBase.h"

#include "ObsidianStateTreePropertyFunction_MakeGameplayTagContainer.generated.h"

USTRUCT()
struct FObsidianStateTreePropertyFunction_MakeGameplayTagContainerInstanceData
{
	GENERATED_BODY()

	/** Tags to put into the container. Each element can be bound separately, invalid tags are skipped. */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	TArray<FGameplayTag> Tags;

	UPROPERTY(EditAnywhere, Category = "Output")
	FGameplayTagContainer Result;
};

/**
 * Property function that makes a Gameplay Tag Container from a list of Gameplay Tags.
 * Can be used on any Gameplay Tag Container property of a State Tree node.
 */
USTRUCT(meta = (DisplayName = "Make Gameplay Tag Container", Category = "Obsidian"))
struct OBSIDIAN_API FObsidianStateTreePropertyFunction_MakeGameplayTagContainer : public FStateTreePropertyFunctionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FObsidianStateTreePropertyFunction_MakeGameplayTagContainerInstanceData;

public:
	//~ Begin FStateTreeNodeBase Interface
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

#if WITH_EDITOR
	virtual FText GetDescription(const FGuid& InID, FStateTreeDataView InInstanceDataView, const IStateTreeBindingLookup& InBindingLookup,
		EStateTreeNodeFormatting InFormatting = EStateTreeNodeFormatting::Text) const override;
#endif
	//~ End FStateTreeNodeBase Interface

	//~ Begin FStateTreePropertyFunctionBase Interface
	virtual void Execute(FStateTreeExecutionContext& InContext) const override;
	//~ End FStateTreePropertyFunctionBase Interface
};
