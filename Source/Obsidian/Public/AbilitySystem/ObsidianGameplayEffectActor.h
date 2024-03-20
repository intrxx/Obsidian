// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObsidianGameplayEffectActor.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class EObsidianEffectToApply
{
	Instant,
	HasDuration,
	Infinite
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
	void OnOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Obisdian|Effects")
	void OnEndOverlap(AActor* TargetActor);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Effect", meta=(EditCondition="EffectToApply == EObsidianEffectToApply::Instant", EditConditionHides))
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Effect", meta=(EditCondition="EffectToApply == EObsidianEffectToApply::Instant", EditConditionHides))
	EObsidianEffectApplicationPolicy InstantEffectApplicationPolicy = EObsidianEffectApplicationPolicy::DoNotApply;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Effect", meta=(EditCondition="EffectToApply == EObsidianEffectToApply::HasDuration", EditConditionHides))
	TSubclassOf<UGameplayEffect> DurationalGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Effect", meta=(EditCondition="EffectToApply == EObsidianEffectToApply::HasDuration", EditConditionHides))
	EObsidianEffectApplicationPolicy DurationalEffectApplicationPolicy = EObsidianEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Effect", meta=(EditCondition="EffectToApply == EObsidianEffectToApply::Infinite", EditConditionHides))
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Effect", meta=(EditCondition="EffectToApply == EObsidianEffectToApply::Infinite", EditConditionHides))
	EObsidianEffectApplicationPolicy InfiniteEffectApplicationPolicy = EObsidianEffectApplicationPolicy::DoNotApply;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Effect", meta=(EditCondition="EffectToApply == EObsidianEffectToApply::Infinite", EditConditionHides))
	EObsidianEffectRemovalPolicy InfiniteEffectRemovalPolicy = EObsidianEffectRemovalPolicy::DoNotRemove;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Effect")
	float EffectLevel = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	bool bDestroyOnEffectRemoval = false;
};
