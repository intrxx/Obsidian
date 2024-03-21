// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "ObsidianGameplayEffectActor.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EObsidianEffectToApply
{
	Instant,
	HasDuration,
	Infinite,
	MultipleEffects
};

UENUM(BlueprintType)
enum class EObsidianEffectApplicationPolicy
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

UENUM(BlueprintType)
enum class EObsidianEffectRemovalPolicy
{
	RemovalOnEndOverlap,
	DoNotRemove
};

USTRUCT(BlueprintType)
struct FObsidianGameplayEffectStack
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	bool bIsInfinite = false;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditDefaultsOnly)
	EObsidianEffectApplicationPolicy EffectApplicationPolicy = EObsidianEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditDefaultsOnly, meta=(EditCondition="bIsInfinite == true", EditConditionHides))
	EObsidianEffectRemovalPolicy EffectRemovalPolicy = EObsidianEffectRemovalPolicy::DoNotRemove;

	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.f;
};

UCLASS()
class OBSIDIAN_API AObsidianGameplayEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AObsidianGameplayEffectActor();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	EObsidianEffectToApply EffectToApply;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Obisdian|Effects")
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> EffectClassToApply);

	UFUNCTION(BlueprintCallable, Category = "Obisdian|Effects")
	void ApplyMultipleEffectsToTarget(AActor* TargetActor, TArray<FObsidianGameplayEffectStack> MultipleGameplayEffectsToApply);

	UFUNCTION(BlueprintCallable, Category = "Obisdian|Effects")
	void OnOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Obisdian|Effects")
	void OnEndOverlap(AActor* TargetActor);
	
protected:
	/** Instant Gameplay Effect */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Effect", meta=(EditCondition="EffectToApply == EObsidianEffectToApply::Instant", EditConditionHides))
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Effect", meta=(EditCondition="EffectToApply == EObsidianEffectToApply::Instant", EditConditionHides))
	EObsidianEffectApplicationPolicy InstantEffectApplicationPolicy = EObsidianEffectApplicationPolicy::DoNotApply;
	
	/** Durational Gameplay Effect */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Effect", meta=(EditCondition="EffectToApply == EObsidianEffectToApply::HasDuration", EditConditionHides))
	TSubclassOf<UGameplayEffect> DurationalGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Effect", meta=(EditCondition="EffectToApply == EObsidianEffectToApply::HasDuration", EditConditionHides))
	EObsidianEffectApplicationPolicy DurationalEffectApplicationPolicy = EObsidianEffectApplicationPolicy::DoNotApply;

	/** Infinite Gameplay Effect */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Effect", meta=(EditCondition="EffectToApply == EObsidianEffectToApply::Infinite", EditConditionHides))
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Effect", meta=(EditCondition="EffectToApply == EObsidianEffectToApply::Infinite", EditConditionHides))
	EObsidianEffectApplicationPolicy InfiniteEffectApplicationPolicy = EObsidianEffectApplicationPolicy::DoNotApply;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Effect", meta=(EditCondition="EffectToApply == EObsidianEffectToApply::Infinite", EditConditionHides))
	EObsidianEffectRemovalPolicy InfiniteEffectRemovalPolicy = EObsidianEffectRemovalPolicy::DoNotRemove;

	/** Multiple Gameplay Effects */
	UPROPERTY(EditAnywhere, Category = "Obsidian|Effect", meta=(EditCondition="EffectToApply == EObsidianEffectToApply::MultipleEffects", EditConditionHides))
	TArray<FObsidianGameplayEffectStack> MultipleGameplayEffects;
	
	/** Level of applied effect */
	UPROPERTY(EditAnywhere, Category = "Obsidian|Effect", meta=(EditCondition="EffectToApply != EObsidianEffectToApply::MultipleEffects", EditConditionHides))
	float EffectLevel = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	bool bDestroyOnEffectRemoval = false;

private:
	void RemoveEffectsFromActor(AActor* TargetActor);
	
private:
	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;
};
