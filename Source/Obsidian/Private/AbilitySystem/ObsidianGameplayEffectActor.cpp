// Copyright 2024 Michał Ogiński

#include "AbilitySystem/ObsidianGameplayEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AObsidianGameplayEffectActor::AObsidianGameplayEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));
}

void AObsidianGameplayEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AObsidianGameplayEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> EffectClassToApply)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if(TargetASC == nullptr)
	{
		return;
	}

	checkf(EffectClassToApply, TEXT("No Gameplay Effect Class found in [%s]"), *GetNameSafe(this));
	
	FGameplayEffectContextHandle GEContextHandle = TargetASC->MakeEffectContext();
	GEContextHandle.AddSourceObject(this);
	
	const FGameplayEffectSpecHandle GESpecHandle = TargetASC->MakeOutgoingSpec(EffectClassToApply, EffectLevel, GEContextHandle);
		
	TargetASC->ApplyGameplayEffectSpecToSelf(*GESpecHandle.Data.Get());
}

void AObsidianGameplayEffectActor::OnOverlap(AActor* TargetActor)
{
}

void AObsidianGameplayEffectActor::OnEndOverlap(AActor* TargetActor)
{
}


