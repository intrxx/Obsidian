// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTags.h"

// ~ Project


#include "Kismet/BlueprintFunctionLibrary.h"
#include "ObsidianUIFunctionLibrary.generated.h"

class UObsidianInventoryItemsWidgetController;
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

	UFUNCTION(BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "ObsidianUIFunctionLibrary|WidgetControllers")
	static UObsidianInventoryItemsWidgetController* GetInventoryItemsWidgetController(const UObject* WorldContextObject);

	static FVector2D GetGameViewportSize();
	
	template<typename T>
	static T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
};

template <typename T>
T* UObsidianUIFunctionLibrary::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}