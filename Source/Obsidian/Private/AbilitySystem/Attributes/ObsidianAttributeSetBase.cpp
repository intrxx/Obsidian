// Copyright 2026 out of sCope team - intrxx

#include "AbilitySystem/Attributes/ObsidianAttributeSetBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/ObsidianCharacterBase.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"


void FObsidianEffectProperties::Reset()
{
	EffectContextHandle.Clear();
	SourceASC = nullptr;
	SourceAvatarActor = nullptr;
	SourceController = nullptr;
	SourceCharacter = nullptr;
	TargetASC = nullptr;
	TargetAvatarActor = nullptr;
	TargetController = nullptr;
	TargetCharacter = nullptr;
	Instigator = nullptr;
	EffectCauser = nullptr;
	bIsPlayerCharacter = false;
	bIsBoss = false;
}

UObsidianAttributeSetBase::UObsidianAttributeSetBase()
{
}

UWorld* UObsidianAttributeSetBase::GetWorld() const
{
	const UObject* Outer = GetOuter();
	check(Outer);

	return Outer->GetWorld();
}

UObsidianAbilitySystemComponent* UObsidianAttributeSetBase::GetObsidianAbilitySystemComponent() const
{
	return Cast<UObsidianAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

void UObsidianAttributeSetBase::SetEffectProperties(const FGameplayEffectModCallbackData& InData, FObsidianEffectProperties& OutProps) const
{
	// Source = causer of the effect, Target = target of the effect (owner of THIS Attribute Set)
	
	const FGameplayEffectContextHandle& EffectContextHandle = InData.EffectSpec.GetContext();
	OutProps.EffectContextHandle = EffectContextHandle;

	OutProps.Instigator = EffectContextHandle.GetOriginalInstigator();
	OutProps.EffectCauser = EffectContextHandle.GetEffectCauser();

	UAbilitySystemComponent* SourceASC = EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	OutProps.SourceASC = SourceASC;
	
	if(IsValid(SourceASC) && SourceASC->AbilityActorInfo.IsValid() && SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		AActor* SourceAvatarActor = nullptr;
		AController* SourceController = nullptr;
		
		SourceAvatarActor = SourceASC->AbilityActorInfo->AvatarActor.Get();
		OutProps.SourceAvatarActor = SourceAvatarActor;
		
		SourceController = SourceASC->AbilityActorInfo->PlayerController.Get();
		OutProps.SourceController = SourceController;
		
		if(SourceController == nullptr && SourceAvatarActor != nullptr)
		{
			if(const APawn* Pawn = Cast<APawn>(SourceAvatarActor))
			{
				SourceController = Pawn->GetController();
				OutProps.SourceController = SourceController;
			}
		}

		if(SourceController)
		{
			OutProps.SourceCharacter = Cast<AObsidianCharacterBase>(SourceController->GetCharacter());
		}
		else
		{
			OutProps.SourceCharacter = Cast<AObsidianCharacterBase>(SourceAvatarActor);
		}
	}
	
	if(InData.Target.AbilityActorInfo.IsValid() && InData.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		AActor* TargetAvatarActor = nullptr;
		AController* TargetController = nullptr;
		
		TargetAvatarActor = InData.Target.AbilityActorInfo->AvatarActor.Get();
		OutProps.TargetAvatarActor = TargetAvatarActor;
		
		TargetController = InData.Target.AbilityActorInfo->PlayerController.Get();
		OutProps.TargetController = TargetController;
		
		if(TargetController == nullptr && TargetAvatarActor != nullptr)
		{
			if(const APawn* Pawn = Cast<APawn>(TargetAvatarActor))
			{
				TargetController = Pawn->GetController();
				OutProps.TargetController = TargetController;
			}
		}
		
		if(TargetController)
		{
			OutProps.TargetCharacter = Cast<AObsidianCharacterBase>(TargetController->GetCharacter());
		}
		else
		{
			OutProps.TargetCharacter = Cast<AObsidianCharacterBase>(TargetAvatarActor);
		}

		OutProps.bIsPlayerCharacter = OutProps.TargetCharacter->ActorHasTag(ObsidianActorTags::Player);
		if(!OutProps.bIsPlayerCharacter)
		{
			OutProps.bIsBoss = OutProps.TargetCharacter->ActorHasTag(ObsidianActorTags::BossEnemy);
		}
		
		OutProps.bCanHitReact = OutProps.TargetCharacter->CanHitReact();

		OutProps.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetAvatarActor);
	}
}
