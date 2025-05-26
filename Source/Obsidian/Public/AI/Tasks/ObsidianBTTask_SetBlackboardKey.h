// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "BehaviorTree/BTTaskNode.h"
#include "ObsidianBTTask_SetBlackboardKey.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ESetAsType : uint8
{
	None UMETA(DisplayName = "None"),
	AsObject UMETA(DisplayName = "As Object"),
	AsBool UMETA(DisplayName = "As Bool"),
	AsFloat UMETA(DisplayName = "As Float"),
	AsVector UMETA(DisplayName = "As Vector"),
	AsRotator UMETA(DisplayName = "As Rotator"),
	AsName UMETA(DisplayName = "As Name"),
	AsString UMETA(DisplayName = "As String"),
	AsInt UMETA(DisplayName = "As Int"),
	AsClass UMETA(DisplayName = "As Class"),
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianBTTask_SetBlackboardKey : public UBTTaskNode
{
	GENERATED_BODY()

	UObsidianBTTask_SetBlackboardKey();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESetAsType SetAsType = ESetAsType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(DisplayName = "Key To Set"), Category = "Key")
	FBlackboardKeySelector KeyToSet_Selector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="SetAsType == ESetAsType::AsObject", EditConditionHides), Category = "Key")
	UObject* ObjectToSet = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="SetAsType == ESetAsType::AsClass", EditConditionHides), Category = "Key")
	UClass* ClassToSet = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="SetAsType == ESetAsType::AsBool", EditConditionHides), Category = "Key")
	bool BoolToSet = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="SetAsType == ESetAsType::AsFloat", EditConditionHides), Category = "Key")
	float FloatToSet = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="SetAsType == ESetAsType::AsInt", EditConditionHides), Category = "Key")
	int32 IntToSet = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="SetAsType == ESetAsType::AsVector", EditConditionHides), Category = "Key")
	FVector VectorToSet = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="SetAsType == ESetAsType::AsRotator", EditConditionHides), Category = "Key")
	FRotator RotatorToSet = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="SetAsType == ESetAsType::AsName", EditConditionHides), Category = "Key")
	FName NameToSet = FName("");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="SetAsType == ESetAsType::AsString", EditConditionHides), Category = "Key")
	FString StringToSet = FString("");
};
