// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTags.h"
#include "ObsidianUIFunctionLibrary.generated.h"

class UObsidianEnemyOverlayWidgetController;
struct FObsidianEnemyWidgetControllerParams;
class UOCharacterStatusWidgetController;
class UMainOverlayWidgetController;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianUIFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "ObsidianUIFunctionLibrary|WidgetControllers")
	static UMainOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "ObsidianUIFunctionLibrary|WidgetControllers")
	static UOCharacterStatusWidgetController* GetCharacterStatusWidgetController(const UObject* WorldContextObject);
	
	template<typename T>
	static T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
};

template <typename T>
T* UObsidianUIFunctionLibrary::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}