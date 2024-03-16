// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObsidianGameplayEffectActor.generated.h"

class UGameplayEffect;

UCLASS()
class OBSIDIAN_API AObsidianGameplayEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AObsidianGameplayEffectActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Obisdian|Effects")
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> EffectClassToApply);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Effects")
	float EffectLevel = 1.f;
};
