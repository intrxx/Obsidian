// Copyright 2024 out of sCope team - Michał Ogiński

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
	const FActiveGameplayEffectHandle ActiveGameplayEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*GESpecHandle.Data.Get());
	
	const bool bIsInfinite = GESpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if(bIsInfinite && InfiniteEffectRemovalPolicy == EObsidianEffectRemovalPolicy::RemovalOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveGameplayEffectHandle, TargetASC);
	}
}

void AObsidianGameplayEffectActor::ApplyMultipleEffectsToTarget(AActor* TargetActor,
	TArray<FObsidianGameplayEffectStack> MultipleGameplayEffectsToApply)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if(TargetASC == nullptr)
	{
		return;
	}

	for(const FObsidianGameplayEffectStack& Effect : MultipleGameplayEffectsToApply)
	{
		checkf(Effect.GameplayEffectClass, TEXT("No Gameplay Effect Class found in [%s]"), *GetNameSafe(this));
		
		FGameplayEffectContextHandle GEContextHandle = TargetASC->MakeEffectContext();
		GEContextHandle.AddSourceObject(this);

		const FGameplayEffectSpecHandle GESpecHandle = TargetASC->MakeOutgoingSpec(Effect.GameplayEffectClass, Effect.EffectLevel, GEContextHandle);
		const FActiveGameplayEffectHandle ActiveGameplayEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*GESpecHandle.Data.Get());

		if(Effect.bIsInfinite && Effect.EffectRemovalPolicy == EObsidianEffectRemovalPolicy::RemovalOnEndOverlap)
		{
			ActiveEffectHandles.Add(ActiveGameplayEffectHandle, TargetASC);
		}
	}
}

void AObsidianGameplayEffectActor::OnOverlap(AActor* TargetActor)
{
	if(InstantEffectApplicationPolicy == EObsidianEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if(DurationalEffectApplicationPolicy == EObsidianEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationalGameplayEffectClass);
	}

	if(InfiniteEffectApplicationPolicy == EObsidianEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}

	if(EffectToApply == EObsidianEffectToApply::MultipleEffects)
	{
		ApplyMultipleEffectsToTarget(TargetActor, MultipleGameplayEffects);
	}
}

void AObsidianGameplayEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if(InstantEffectApplicationPolicy == EObsidianEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if(DurationalEffectApplicationPolicy == EObsidianEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationalGameplayEffectClass);
	}

	if(InfiniteEffectApplicationPolicy == EObsidianEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
	
	if(InfiniteEffectRemovalPolicy == EObsidianEffectRemovalPolicy::RemovalOnEndOverlap)
	{
		RemoveEffectsFromActor(TargetActor);
	}

	if(EffectToApply == EObsidianEffectToApply::MultipleEffects && !ActiveEffectHandles.IsEmpty())
	{
		RemoveEffectsFromActor(TargetActor);
	}
}

void AObsidianGameplayEffectActor::RemoveEffectsFromActor(AActor* TargetActor)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if(!IsValid(TargetASC))
	{
		return;
	}

	TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		
	for(TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
	{
		if(TargetASC == HandlePair.Value)
		{
			TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
			HandlesToRemove.Add(HandlePair.Key);
		}
	}

	for(FActiveGameplayEffectHandle& Handle : HandlesToRemove)
	{
		ActiveEffectHandles.FindAndRemoveChecked(Handle);
	}
}


