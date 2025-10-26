// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ItemAffixes/ObsidianAffixAbilitySet.h"

#include <AbilitySystemBlueprintLibrary.h>
#if WITH_EDITOR
#include <Misc/DataValidation.h>
#endif // ~ WITH_EDITOR

#include "AbilitySystem/Abilities/ObsidianGameplayAbility.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/ObsidianAbilitySystemEffectTypes.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "Obsidian/ObsidianGameModule.h"

#if WITH_EDITOR
// ~ FObsidianAffixAbilitySet_GameplayAbility
EDataValidationResult FObsidianAffixAbilitySet_GameplayAbility::ValidateData(FDataValidationContext& Context, const int Index) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	if(Ability == nullptr)
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Abilty at index [%i] is null! \n"
			"Please set a valid Ability class or delete this index entry in the Granted Gameplay Abilities array"), Index));

		Context.AddError(ErrorMessage);
	}
	
	return Result;
}

// ~ FObsidianAffixAbilitySet_GameplayEffect

EDataValidationResult FObsidianAffixAbilitySet_GameplayEffect::ValidateData(FDataValidationContext& Context, const int Index) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	if(GameplayEffect == nullptr)
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Gameplay Effect at index [%i] is null! \n"
			"Please set a valid Gameplay Effect class or delete this index entry in the Granted Gameplay Effects array"), Index));

		Context.AddError(ErrorMessage);
	}

	return Result;
}

// ~~ Start of UObsidianAffixAbilitySet
EDataValidationResult UObsidianAffixAbilitySet::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	unsigned int AbilityIndex = 0;
	for(const FObsidianAffixAbilitySet_GameplayAbility& Ability : GrantedGameplayAbilities)
	{
		Result =  CombineDataValidationResults(Result, Ability.ValidateData(Context, AbilityIndex));
		AbilityIndex++;
	}
	
	unsigned int EffectIndex = 0;
	for(const FObsidianAffixAbilitySet_GameplayEffect& Effect : GrantedGameplayEffects)
	{
		Result =  CombineDataValidationResults(Result, Effect.ValidateData(Context, EffectIndex));
		EffectIndex++;
	}
	
	return Result;
}
// ~~ End of UObsidianAffixAbilitySet
#endif // ~ WITH_EDITOR

// ~ FObsidianAffixAbilitySet_GrantedHandles
void FObsidianAffixAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayAbilitySpecHandles.Add(Handle);
	}
}

void FObsidianAffixAbilitySet_GrantedHandles::AddActiveGameplayEffectSpecHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FObsidianAffixAbilitySet_GrantedHandles::TakeFromAbilitySystem(UObsidianAbilitySystemComponent* ObsidianASC)
{
	check(ObsidianASC);

	if (!ObsidianASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			ObsidianASC->RemoveActiveGameplayEffect(Handle);
		}
	}
	
	for (const FGameplayAbilitySpecHandle& Handle : GameplayAbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			ObsidianASC->ClearAbility(Handle);
		}
	}

	GameplayEffectHandles.Reset();
	GameplayAbilitySpecHandles.Reset();
}
// ~ End of FObsidianAffixAbilitySet_GrantedHandles

UObsidianAffixAbilitySet::UObsidianAffixAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void UObsidianAffixAbilitySet::GiveToAbilitySystem(UObsidianAbilitySystemComponent* ObsidianASC, const FGameplayTag& AffixTag,
	const FObsidianActiveAffixValue& AffixValue, FObsidianAffixAbilitySet_GrantedHandles* GrantedHandles, UObject* SourceObject) const
{
	check(ObsidianASC);

	if(!ObsidianASC->IsOwnerActorAuthoritative())
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
			UE_LOG(LogObsidian, Error, TEXT("Granted Gameplay Ability [%d] on Ablity Set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		UObsidianGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UObsidianGameplayAbility>();

		checkf(AffixValue.AffixValues.Num() == 1, TEXT("Affix that gives abilities should have one Affix Value!"));
		float AbilityLevel = AffixValue.AffixValues[0];
		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.OptionalInputTag);
		
		const FGameplayAbilitySpecHandle AbilitySpecHandle = ObsidianASC->GiveAbility(AbilitySpec);

		if(GrantedHandles)
		{
			GrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}
	
	// Granting Gameplay Effects
	for(int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FObsidianAffixAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if(!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogObsidian, Error, TEXT("Granted Gameplay Effect [%d] on Ability Set [%s] is not valid."), EffectIndex, *GetNameSafe(this));
			continue;	
		}
		
		FGameplayEffectContextHandle ContextHandle = ObsidianASC->MakeEffectContext();
		ContextHandle.AddSourceObject(SourceObject);
		
		const FGameplayEffectSpecHandle SpecHandle = ObsidianASC->MakeOutgoingSpec(EffectToGrant.GameplayEffect, 1, ContextHandle);
		uint8 Index = 0;
		for (const float Value : AffixValue.AffixValues)
		{
			const FGameplayTag TagPair = AffixValue.AffixValuesIdentifiers[Index].AffixValueID;
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, TagPair, Value);
			++Index;
		}
		
		const FActiveGameplayEffectHandle ActiveSpecHandle = ObsidianASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		if(GrantedHandles)
		{
			GrantedHandles->AddActiveGameplayEffectSpecHandle(ActiveSpecHandle);
		}
	}
}

void UObsidianAffixAbilitySet::GiveItemAffixesToAbilitySystem(UObsidianAbilitySystemComponent* ObsidianASC, const TArray<FObsidianActiveItemAffix>& ItemAffixes,
	FObsidianAffixAbilitySet_GrantedHandles* GrantedHandles, UObject* SourceObject) const
{
	check(ObsidianASC);

	if(!ObsidianASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	// Do not grant abilities with this function, it is made for Affix batching
	ensureMsgf(GrantedGameplayAbilities.IsEmpty(), TEXT("This function should only be used to Grant Gameplay Effects, Gameplay Abilities will not be granted!"));

	// Grant Batched Gameplay Effects
	checkf(GrantedGameplayEffects.Num() == 1, TEXT("This should give just one batched Gameplay Effect"));
	const FObsidianAffixAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[0];
	if(!IsValid(EffectToGrant.GameplayEffect))
	{
		UE_LOG(LogObsidian, Error, TEXT("Granted Gameplay Effect [0] on Ability Set [%s] is not valid."), *GetNameSafe(this));
		return;	
	}

	//NOTE(intrxx) IDK if that's safe, it seems to work both on server and client, but need extensive tests I guess
	FGameplayEffectContextHandle ContextHandle = ObsidianASC->MakeEffectContext();
	ContextHandle.AddSourceObject(SourceObject);
	const UGameplayEffect* BaseGE = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
	UGameplayEffect* DynamicAffixGE = DuplicateObject<UGameplayEffect>(BaseGE, GetTransientPackage());
		
	for (const FObsidianActiveItemAffix& Affix : ItemAffixes)
	{
		for (int32 i = 0; i < Affix.CurrentAffixValue.AffixValuesIdentifiers.Num(); ++i)
		{
			FGameplayModifierInfo NewModifierInfo;
			NewModifierInfo.Attribute = Affix.CurrentAffixValue.AffixValuesIdentifiers[i].AttributeToModify;
			NewModifierInfo.ModifierOp = EGameplayModOp::AddFinal; //TODO(intrxx) don't want to AddFinal for all, need to take it from ItemAffix as well
			NewModifierInfo.ModifierMagnitude = FScalableFloat(Affix.CurrentAffixValue.AffixValues[i]);
			DynamicAffixGE->Modifiers.Add(NewModifierInfo);
		}
	}
	const FActiveGameplayEffectHandle ActiveSpecHandle = ObsidianASC->ApplyGameplayEffectToSelf(DynamicAffixGE, 1.0f, ContextHandle);
		
	if(GrantedHandles)
	{
		GrantedHandles->AddActiveGameplayEffectSpecHandle(ActiveSpecHandle);
	}
}
