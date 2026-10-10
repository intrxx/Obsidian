// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "ObsidianTypes/ObsidianCoreTypes.h"

#include "ObsidianAbilitySystemFunctionLibrary.generated.h"

struct FGameplayTagContainer;
struct FGameplayEffectContextHandle;
class UObsidianAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianAbilitySystemFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (HidePin = "InWorldContextObject", DefaultToSelf = "InWorldContextObject"), Category = "ObsidianASCFunctionLibrary|Init")
	static void InitializeEnemyDefaultAttributesWithClass(const UObject* InWorldContextObject, UObsidianAbilitySystemComponent* InASC,
		const EObsidianEnemyClass InEnemyClass, const float InLevel, UObject* InSourceObject = nullptr);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "InWorldContextObject", DefaultToSelf = "InWorldContextObject"), Category = "ObsidianASCFunctionLibrary|Combat")
	static void GetAllCharactersWithinRadius(const UObject* InWorldContextObject, TArray<AActor*>& OutOverlappingActors,
		UClass* InActorClassFilter, const TArray<AActor*>& InActorsToIgnore, const float InRadius, const FVector& InSphereOrigin, const bool bInWithDebug);

	UFUNCTION(BlueprintPure, Category = "ObsidianASCFunctionLibrary|Combat")
	static bool IsBlockedAttack(const FGameplayEffectContextHandle& InEffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "ObsidianASCFunctionLibrary|Combat")
	static bool IsCriticalAttack(const FGameplayEffectContextHandle& InEffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "ObsidianASCFunctionLibrary|Combat")
	static void SetIsBlockedAttack(UPARAM(ref) FGameplayEffectContextHandle& InOutEffectContextHandle, const bool bInIsBlockedAttack);

	UFUNCTION(BlueprintCallable, Category = "ObsidianASCFunctionLibrary|Combat")
	static void SetIsCriticalAttack(UPARAM(ref) FGameplayEffectContextHandle& InOutEffectContextHandle, const bool bInIsCriticalAttack);

	UFUNCTION(BlueprintCallable, Category = "ObsidianASCFunctionLibrary")
	static void GetAllOwnedTagsFromActor(AActor* InActor, FGameplayTagContainer& OutTags);
};
