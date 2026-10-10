// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/ItemAffixes/ObsidianAffixAbilitySet.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "AbilitySystem/Abilities/ObsidianGameplayAbility.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Obsidian/ObsidianLogCategories.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // ~ WITH_EDITOR


#if WITH_EDITOR
// ~ FObsidianAffixAbilitySet_GameplayAbility
EDataValidationResult FObsidianAffixAbilitySet_GameplayAbility::ValidateData(FDataValidationContext& InContext, const int InIndex) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	if(Ability == nullptr)
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Abilty at index [%i] is null! \n"
			"Please set a valid Ability class or delete this index entry in the Granted Gameplay Abilities array"), InIndex));

		InContext.AddError(ErrorMessage);
	}
	
	return Result;
}

// ~ FObsidianAffixAbilitySet_GameplayEffect

EDataValidationResult FObsidianAffixAbilitySet_GameplayEffect::ValidateData(FDataValidationContext& InContext, const int InIndex) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	if(GameplayEffect == nullptr)
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Gameplay Effect at index [%i] is null! \n"
			"Please set a valid Gameplay Effect class or delete this index entry in the Granted Gameplay Effects array"), InIndex));

		InContext.AddError(ErrorMessage);
	}

	return Result;
}

// ~~ Start of UObsidianAffixAbilitySet
EDataValidationResult UObsidianAffixAbilitySet::IsDataValid(FDataValidationContext& InContext) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(InContext), EDataValidationResult::Valid);

	unsigned int AbilityIndex = 0;
	for(const FObsidianAffixAbilitySet_GameplayAbility& Ability : GrantedGameplayAbilities)
	{
		Result =  CombineDataValidationResults(Result, Ability.ValidateData(InContext, AbilityIndex));
		AbilityIndex++;
	}
	
	unsigned int EffectIndex = 0;
	for(const FObsidianAffixAbilitySet_GameplayEffect& Effect : GrantedGameplayEffects)
	{
		Result =  CombineDataValidationResults(Result, Effect.ValidateData(InContext, EffectIndex));
		EffectIndex++;
	}
	
	return Result;
}
// ~~ End of UObsidianAffixAbilitySet
#endif // ~ WITH_EDITOR

// ~ FObsidianAffixAbilitySet_GrantedHandles
void FObsidianAffixAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& InHandle)
{
	if (InHandle.IsValid())
	{
		GameplayAbilitySpecHandles.Add(InHandle);
	}
}

void FObsidianAffixAbilitySet_GrantedHandles::AddActiveGameplayEffectSpecHandle(const FActiveGameplayEffectHandle& InHandle)
{
	if (InHandle.IsValid())
	{
		GameplayEffectHandles.Add(InHandle);
	}
}

void FObsidianAffixAbilitySet_GrantedHandles::TakeFromAbilitySystem(UObsidianAbilitySystemComponent* InObsidianASC)
{
	check(InObsidianASC);

	if (!InObsidianASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			InObsidianASC->RemoveActiveGameplayEffect(Handle);
		}
	}
	
	for (const FGameplayAbilitySpecHandle& Handle : GameplayAbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			InObsidianASC->ClearAbility(Handle);
		}
	}

	GameplayEffectHandles.Reset();
	GameplayAbilitySpecHandles.Reset();
}
// ~ End of FObsidianAffixAbilitySet_GrantedHandles

UObsidianAffixAbilitySet::UObsidianAffixAbilitySet(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{}

void UObsidianAffixAbilitySet::GiveToAbilitySystem(UObsidianAbilitySystemComponent* InObsidianASC, const FGameplayTag& InAffixTag,
	const FObsidianActiveAffixValue& InAffixValue, FObsidianAffixAbilitySet_GrantedHandles* OutGrantedHandles, UObject* InSourceObject) const
{
	check(InObsidianASC);

	if(!InObsidianASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	// Granting Gameplay Abilities
	for(int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FObsidianAffixAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if(!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(ObLogAffixes, Error, TEXT("Granted Gameplay Ability [%d] on Ablity Set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		UObsidianGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UObsidianGameplayAbility>();

		checkf(InAffixValue.AffixValues.Num() == 1, TEXT("Affix that gives abilities should have one Affix Value!"));
		float AbilityLevel = InAffixValue.AffixValues[0];
		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityLevel);
		AbilitySpec.SourceObject = InSourceObject;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.OptionalInputTag);
		
		const FGameplayAbilitySpecHandle AbilitySpecHandle = InObsidianASC->GiveAbility(AbilitySpec);

		if(OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}
	
	// Granting Gameplay Effects
	for(int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FObsidianAffixAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if(!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(ObLogAffixes, Error, TEXT("Granted Gameplay Effect [%d] on Ability Set [%s] is not valid."), EffectIndex, *GetNameSafe(this));
			continue;	
		}
		
		FGameplayEffectContextHandle ContextHandle = InObsidianASC->MakeEffectContext();
		ContextHandle.AddSourceObject(InSourceObject);
		
		const FGameplayEffectSpecHandle SpecHandle = InObsidianASC->MakeOutgoingSpec(EffectToGrant.GameplayEffect, 1, ContextHandle);
		uint8 Index = 0;
		for (const float Value : InAffixValue.AffixValues)
		{
			const FGameplayTag TagPair = InAffixValue.AffixValuesIdentifiers[Index].AffixValueID;
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, TagPair, Value);
			++Index;
		}
		
		const FActiveGameplayEffectHandle ActiveSpecHandle = InObsidianASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		if(OutGrantedHandles)
		{
			OutGrantedHandles->AddActiveGameplayEffectSpecHandle(ActiveSpecHandle);
		}
	}
}

void UObsidianAffixAbilitySet::GiveItemAffixesToAbilitySystem(UObsidianAbilitySystemComponent* InObsidianASC, const TArray<FObsidianActiveItemAffix>& InItemAffixes,
	FObsidianAffixAbilitySet_GrantedHandles* OutGrantedHandles, UObject* InSourceObject) const
{
	check(InObsidianASC);

	if(!InObsidianASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	// Grant Batched Gameplay Effects
	checkf(GrantedGameplayEffects.Num() == 1, TEXT("This should give just one batched Gameplay Effect"));
	const FObsidianAffixAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[0];
	if(!IsValid(EffectToGrant.GameplayEffect))
	{
		UE_LOG(ObLogAffixes, Error, TEXT("Granted Gameplay Effect [0] on Ability Set [%s] is not valid."), *GetNameSafe(this));
		return;	
	}

	//NOTE(intrxx) IDK if that's safe, it seems to work both on server and client, but need extensive tests I guess
	FGameplayEffectContextHandle ContextHandle = InObsidianASC->MakeEffectContext();
	ContextHandle.AddSourceObject(InSourceObject);
	const UGameplayEffect* BaseGE = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
	UGameplayEffect* DynamicAffixGE = DuplicateObject<UGameplayEffect>(BaseGE, GetTransientPackage());
		
	for (const FObsidianActiveItemAffix& Affix : InItemAffixes)
	{
		for (int32 i = 0; i < Affix.CurrentAffixValue.AffixValuesIdentifiers.Num(); ++i)
		{
			FGameplayModifierInfo NewModifierInfo;
			NewModifierInfo.Attribute = Affix.CurrentAffixValue.AffixValuesIdentifiers[i].AttributeToModify;
			NewModifierInfo.ModifierOp = Affix.AffixValuesDefinition.ApplyingRule;
			
			 //TODO(intrxx) kind of temporary solution, idk how to solve it cleanly yet.
			float Value = Affix.CurrentAffixValue.AffixValues[i];
			if (NewModifierInfo.ModifierOp == EGameplayModOp::MultiplyAdditive || NewModifierInfo.ModifierOp == EGameplayModOp::MultiplyCompound)
			{
				Value = 1.0f + (Value / 100.0f);
			}
			NewModifierInfo.ModifierMagnitude = FScalableFloat(Value);
			
			DynamicAffixGE->Modifiers.Add(NewModifierInfo);
		}
	}
	const FActiveGameplayEffectHandle ActiveSpecHandle = InObsidianASC->ApplyGameplayEffectToSelf(DynamicAffixGE, 1.0f, ContextHandle);
		
	if(OutGrantedHandles)
	{
		OutGrantedHandles->AddActiveGameplayEffectSpecHandle(ActiveSpecHandle);
	}
}
