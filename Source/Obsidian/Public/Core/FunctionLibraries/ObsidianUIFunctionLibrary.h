// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>
#include <GameplayTags.h>


#include <Kismet/BlueprintFunctionLibrary.h>
#include "ObsidianUIFunctionLibrary.generated.h"

struct FObsidianEnemyWidgetControllerParams;

class UObCharacterSelectionWidgetController;
class UObInventoryItemsWidgetController;
class UObsidianEnemyOverlayWidgetController;
class UObCharacterStatusWidgetController;
class UObMainOverlayWidgetController;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianUIFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "ObsidianUIFunctionLibrary|WidgetControllers")
	static UObMainOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "ObsidianUIFunctionLibrary|WidgetControllers")
	static UObCharacterStatusWidgetController* GetCharacterStatusWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "ObsidianUIFunctionLibrary|WidgetControllers")
	static UObInventoryItemsWidgetController* GetInventoryItemsWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "ObsidianUIFunctionLibrary|WidgetControllers")
	static UObCharacterSelectionWidgetController* GetCharacterSelectionWidgetController(const UObject* WorldContextObject);

	static FVector2D GetGameViewportSize();
	
	template<typename T>
	static T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
};

template <typename T>
T* UObsidianUIFunctionLibrary::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}