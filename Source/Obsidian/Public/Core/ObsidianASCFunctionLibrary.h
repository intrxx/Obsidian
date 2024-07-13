// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ObsidianTypes/Character/ObsidianEnemyType.h"
#include "ObsidianASCFunctionLibrary.generated.h"

struct FGameplayEffectContextHandle;
class UObsidianAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianASCFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "ObsidianASCFunctionLibrary|Init")
	static void InitializeEnemyDefaultAttributesWithClass(const UObject* WorldContextObject, UObsidianAbilitySystemComponent* ASC, const EObsidianEnemyClass EnemyClass, const float Level, UObject* SourceObject = nullptr);

	UFUNCTION(BlueprintPure, Category = "ObsidianASCFunctionLibrary|Combat")
	static bool IsBlockedAttack(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "ObsidianASCFunctionLibrary|Combat")
	static bool IsCriticalAttack(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "ObsidianASCFunctionLibrary|Combat")
	static void SetIsBlockedAttack(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsBlockedAttack);

	UFUNCTION(BlueprintCallable, Category = "ObsidianASCFunctionLibrary|Combat")
	static void SetIsCriticalAttack(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsCriticalAttack);
};
