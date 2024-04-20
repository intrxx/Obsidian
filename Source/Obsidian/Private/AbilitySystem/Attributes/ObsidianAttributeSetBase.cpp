// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Attributes/ObsidianAttributeSetBase.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Characters/ObsidianCharacterBase.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemBlueprintLibrary.h"

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

void UObsidianAttributeSetBase::SetEffectProperties(const FGameplayEffectModCallbackData& Data,
	FObsidianEffectProperties& Props) const
{
	// Source = causer of the effect, Target = target of the effect (owner of THIS Attribute Set)
	
	FGameplayEffectContextHandle EffectContextHandle = Data.EffectSpec.GetContext();
	Props.EffectContextHandle = EffectContextHandle;

	UAbilitySystemComponent* SourceASC = EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	Props.SourceASC = SourceASC;
	
	if(IsValid(SourceASC) && SourceASC->AbilityActorInfo.IsValid() && SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		AActor* SourceAvatarActor = nullptr;
		AController* SourceController = nullptr;
		
		SourceAvatarActor = SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceAvatarActor = SourceAvatarActor;
		
		SourceController = SourceASC->AbilityActorInfo->PlayerController.Get();
		Props.SourceController = SourceController;
		
		if(SourceController == nullptr && SourceAvatarActor != nullptr)
		{
			if(const APawn* Pawn = Cast<APawn>(SourceAvatarActor))
			{
				SourceController = Pawn->GetController();
				Props.SourceController = SourceController;
			}
		}

		if(SourceController)
		{
			Props.SourceCharacter = Cast<AObsidianCharacterBase>(SourceController->GetCharacter());
		}
		else
		{
			Props.SourceCharacter = Cast<AObsidianCharacterBase>(SourceAvatarActor);
		}
	}
	
	if(Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		AActor* TargetAvatarActor = nullptr;
		AController* TargetController = nullptr;
		
		TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetAvatarActor = TargetAvatarActor;
		
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetController = TargetController;
		
		if(TargetController == nullptr && TargetAvatarActor != nullptr)
		{
			if(const APawn* Pawn = Cast<APawn>(TargetAvatarActor))
			{
				TargetController = Pawn->GetController();
				Props.TargetController = TargetController;
			}
		}
		
		if(TargetController)
		{
			Props.TargetCharacter = Cast<AObsidianCharacterBase>(TargetController->GetCharacter());
			
		}
		else
		{
			Props.TargetCharacter = Cast<AObsidianCharacterBase>(TargetAvatarActor);
		}

		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetAvatarActor);
	}
}
