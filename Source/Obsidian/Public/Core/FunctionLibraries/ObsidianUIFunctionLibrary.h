// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayTags.h"
#include "Kismet/BlueprintFunctionLibrary.h"

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
	UFUNCTION(BlueprintPure, meta = (HidePin = "InWorldContextObject", DefaultToSelf = "InWorldContextObject"), Category = "ObsidianUIFunctionLibrary|WidgetControllers")
	static UObMainOverlayWidgetController* GetOverlayWidgetController(const UObject* InWorldContextObject);

	UFUNCTION(BlueprintPure, meta = (HidePin = "InWorldContextObject", DefaultToSelf = "InWorldContextObject"), Category = "ObsidianUIFunctionLibrary|WidgetControllers")
	static UObCharacterStatusWidgetController* GetCharacterStatusWidgetController(const UObject* InWorldContextObject);

	UFUNCTION(BlueprintPure, meta = (HidePin = "InWorldContextObject", DefaultToSelf = "InWorldContextObject"), Category = "ObsidianUIFunctionLibrary|WidgetControllers")
	static UObInventoryItemsWidgetController* GetInventoryItemsWidgetController(const UObject* InWorldContextObject);

	UFUNCTION(BlueprintPure, meta = (HidePin = "InWorldContextObject", DefaultToSelf = "InWorldContextObject"), Category = "ObsidianUIFunctionLibrary|WidgetControllers")
	static UObCharacterSelectionWidgetController* GetCharacterSelectionWidgetController(const UObject* InWorldContextObject);

	static FVector2D GetGameViewportSize();
	
	template<typename T>
	static T* GetDataTableRowByTag(UDataTable* InDataTable, const FGameplayTag& InTag);
};

template <typename T>
T* UObsidianUIFunctionLibrary::GetDataTableRowByTag(UDataTable* InDataTable, const FGameplayTag& InTag)
{
	return InDataTable->FindRow<T>(InTag.GetTagName(), TEXT(""));
}