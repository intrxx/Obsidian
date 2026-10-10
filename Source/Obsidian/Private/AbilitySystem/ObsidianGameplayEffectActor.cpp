// Copyright 2026 out of sCope team - intrxx

#include "AbilitySystem/ObsidianGameplayEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "ObsidianTypes/ObsidianCoreTypes.h"


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

void AObsidianGameplayEffectActor::ApplyEffectToTarget(AActor* InTargetActor, TSubclassOf<UGameplayEffect> InEffectClassToApply)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InTargetActor);

	if(TargetASC == nullptr)
	{
		return;
	}

	checkf(InEffectClassToApply, TEXT("No Gameplay Effect Class found in [%s]"), *GetNameSafe(this));
	
	FGameplayEffectContextHandle GEContextHandle = TargetASC->MakeEffectContext();
	GEContextHandle.AddSourceObject(this);
	
	const FGameplayEffectSpecHandle GESpecHandle = TargetASC->MakeOutgoingSpec(InEffectClassToApply, EffectLevel, GEContextHandle);
	const FActiveGameplayEffectHandle ActiveGameplayEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*GESpecHandle.Data.Get());
	
	const bool bIsInfinite = GESpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if(bIsInfinite && InfiniteEffectRemovalPolicy == EObsidianEffectRemovalPolicy::RemovalOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveGameplayEffectHandle, TargetASC);
	}
}

void AObsidianGameplayEffectActor::ApplyMultipleEffectsToTarget(AActor* InTargetActor,
	TArray<FObsidianGameplayEffectStack> InMultipleGameplayEffectsToApply)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InTargetActor);

	if(TargetASC == nullptr)
	{
		return;
	}

	for(const FObsidianGameplayEffectStack& Effect : InMultipleGameplayEffectsToApply)
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

void AObsidianGameplayEffectActor::OnOverlap(AActor* InTargetActor)
{
	if(!bApplyEffectToEnemies && InTargetActor->ActorHasTag(ObsidianActorTags::Enemy))
	{
		return;
	}
	
	if(InstantEffectApplicationPolicy == EObsidianEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(InTargetActor, InstantGameplayEffectClass);
		if(bDestroyOnEffectApplication)
		{
			Destroy();
		}
	}

	if(DurationalEffectApplicationPolicy == EObsidianEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(InTargetActor, DurationalGameplayEffectClass);
	}

	if(InfiniteEffectApplicationPolicy == EObsidianEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(InTargetActor, InfiniteGameplayEffectClass);
	}

	if(EffectToApply == EObsidianEffectToApply::MultipleEffects)
	{
		ApplyMultipleEffectsToTarget(InTargetActor, MultipleGameplayEffects);
	}
}

void AObsidianGameplayEffectActor::OnEndOverlap(AActor* InTargetActor)
{
	if(!bApplyEffectToEnemies && InTargetActor->ActorHasTag(ObsidianActorTags::Enemy))
	{
		return;
	}
	
	if(InstantEffectApplicationPolicy == EObsidianEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(InTargetActor, InstantGameplayEffectClass);
		if(bDestroyOnEffectApplication)
		{
			Destroy();
		}
	}

	if(DurationalEffectApplicationPolicy == EObsidianEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(InTargetActor, DurationalGameplayEffectClass);
	}

	if(InfiniteEffectApplicationPolicy == EObsidianEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(InTargetActor, InfiniteGameplayEffectClass);
	}
	
	if(InfiniteEffectRemovalPolicy == EObsidianEffectRemovalPolicy::RemovalOnEndOverlap)
	{
		RemoveEffectsFromActor(InTargetActor);
	}

	if(EffectToApply == EObsidianEffectToApply::MultipleEffects && !ActiveEffectHandles.IsEmpty())
	{
		RemoveEffectsFromActor(InTargetActor);
	}
}

void AObsidianGameplayEffectActor::RemoveEffectsFromActor(AActor* InTargetActor)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InTargetActor);

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


