// Copyright 2024 Michał Ogiński


#include "AbilitySystem/ObsidianGameplayEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "Characters/ObsidianCharacterBase.h"
#include "Components/SphereComponent.h"

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

void AObsidianGameplayEffectActor::ApplyEffectToTarget(AActor* Target, TSubclassOf<UGameplayEffect> EffectClassToApply)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);

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


