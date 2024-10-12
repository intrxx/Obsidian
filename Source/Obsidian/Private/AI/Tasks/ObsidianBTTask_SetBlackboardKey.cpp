// Copyright 2024 out of sCope team - Michał Ogiński


#include "AI/Tasks/ObsidianBTTask_SetBlackboardKey.h"

#include "BehaviorTree/BlackboardComponent.h"

UObsidianBTTask_SetBlackboardKey::UObsidianBTTask_SetBlackboardKey()
{
	NodeName = FString("Set Blackboard Key");
	INIT_TASK_NODE_NOTIFY_FLAGS();
}

EBTNodeResult::Type UObsidianBTTask_SetBlackboardKey::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();

	if(BlackboardComponent == nullptr)
	{
		UE_VLOG(OwnerComp.GetOwner(), LogBehaviorTree, Error, TEXT("UObsidianBTTask_SetBlackboardKey::ExecuteTask failed since BlackboardComponent is missing."));
		return EBTNodeResult::Failed;
	}

	if(SetAsType == ESetAsType::AsBool)
	{
		BlackboardComponent->SetValueAsBool(KeyToSet_Selector.SelectedKeyName, BoolToSet);
		return EBTNodeResult::Succeeded;
	}
	if(SetAsType == ESetAsType::AsObject)
	{
		BlackboardComponent->SetValueAsObject(KeyToSet_Selector.SelectedKeyName, ObjectToSet);
		return EBTNodeResult::Succeeded;
	}
	if(SetAsType == ESetAsType::AsFloat)
	{
		BlackboardComponent->SetValueAsFloat(KeyToSet_Selector.SelectedKeyName, FloatToSet);
		return EBTNodeResult::Succeeded;
	}
	if(SetAsType == ESetAsType::AsVector)
	{
		BlackboardComponent->SetValueAsVector(KeyToSet_Selector.SelectedKeyName, VectorToSet);
		return EBTNodeResult::Succeeded;
	}
	if(SetAsType == ESetAsType::AsInt)
	{
		BlackboardComponent->SetValueAsInt(KeyToSet_Selector.SelectedKeyName, IntToSet);
		return EBTNodeResult::Succeeded;
	}
	if(SetAsType == ESetAsType::AsRotator)
	{
		BlackboardComponent->SetValueAsRotator(KeyToSet_Selector.SelectedKeyName, RotatorToSet);
		return EBTNodeResult::Succeeded;
	}
	if(SetAsType == ESetAsType::AsString)
	{
		BlackboardComponent->SetValueAsString(KeyToSet_Selector.SelectedKeyName, StringToSet);
		return EBTNodeResult::Succeeded;
	}
	if(SetAsType == ESetAsType::AsName)
	{
		BlackboardComponent->SetValueAsName(KeyToSet_Selector.SelectedKeyName, NameToSet);
		return EBTNodeResult::Succeeded;
	}
	if(SetAsType == ESetAsType::AsClass)
	{
		BlackboardComponent->SetValueAsClass(KeyToSet_Selector.SelectedKeyName, ClassToSet);
		return EBTNodeResult::Succeeded;
	}
	
	return EBTNodeResult::Failed;
}

FString UObsidianBTTask_SetBlackboardKey::GetStaticDescription() const
{
	FString FinalMessage = FString::Printf(TEXT("Ket to Set: [%s] \n"), *KeyToSet_Selector.SelectedKeyName.ToString());
		
	if(SetAsType == ESetAsType::AsBool)
	{
		return FinalMessage += BoolToSet ? FString::Printf(TEXT("Set as True")) : FString::Printf(TEXT("Set as False"));
	}
	if(SetAsType == ESetAsType::AsObject)
	{
		return FinalMessage += FString::Printf(TEXT("Object to set: [%s]"), *GetNameSafe(ObjectToSet));
	}
	if(SetAsType == ESetAsType::AsFloat)
	{
		return FinalMessage += FString::Printf(TEXT("Float to set: [%f]"), FloatToSet);
	}
	if(SetAsType == ESetAsType::AsVector)
	{
		return FinalMessage += FString::Printf(TEXT("Vector to set: [X: %f,Y: %f,Z: %f]"), VectorToSet.X, VectorToSet.Y, VectorToSet.Z);
	}
	if(SetAsType == ESetAsType::AsInt)
	{
		return FinalMessage += FString::Printf(TEXT("Int to set: [%d]"), IntToSet);
	}
	if(SetAsType == ESetAsType::AsRotator)
	{
		return FinalMessage += FString::Printf(TEXT("Rotator to set: [Pitch: %f,Roll: %f,Yaw: %f]"), RotatorToSet.Pitch, RotatorToSet.Roll, RotatorToSet.Yaw);
	}
	if(SetAsType == ESetAsType::AsString)
	{
		return FinalMessage += FString::Printf(TEXT("String to set: [%s]"), *StringToSet);
	}
	if(SetAsType == ESetAsType::AsName)
	{
		return FinalMessage += FString::Printf(TEXT("Name to set: [%s]"), *NameToSet.ToString());
	}
	if(SetAsType == ESetAsType::AsClass)
	{
		return FinalMessage += FString::Printf(TEXT("Class to set: [%s]"), *GetNameSafe(ClassToSet));
	}

	return FinalMessage;
}
