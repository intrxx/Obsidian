// Copyright 2024 out of sCope team - Michał Ogiński

#include "AbilitySystem/Data/ObsidianAbilitySet.h"

// ~ Core
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // ~ With Editor

// ~ Project
#include "AbilitySystem/Abilities/ObsidianGameplayAbility.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Obsidian/ObsidianGameModule.h"

void FObsidianAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if(Handle.IsValid())
	{
		GameplayAbilitySpecHandles.Add(Handle);
	}
}

void FObsidianAbilitySet_GrantedHandles::AddActiveGameplayEffectSpecHandle(const FActiveGameplayEffectHandle& Handle)
{
	if(Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FObsidianAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* AttributeSet)
{
	GrantedAttributeSets.Add(AttributeSet);
}

void FObsidianAbilitySet_GrantedHandles::TakeFromAbilitySystem(UObsidianAbilitySystemComponent* ObsidianASC)
{
	check(ObsidianASC);

	if(!ObsidianASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	for(const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if(Handle.IsValid())
		{
			ObsidianASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for(const FGameplayAbilitySpecHandle& Handle : GameplayAbilitySpecHandles)
	{
		if(Handle.IsValid())
		{
			ObsidianASC->ClearAbility(Handle);
		}
	}

	for(UAttributeSet* AttributeSet : GrantedAttributeSets)
	{ 
		if(AttributeSet)
		{
			ObsidianASC->RemoveSpawnedAttribute(AttributeSet);
		}
	}

	GameplayEffectHandles.Reset();
	GameplayAbilitySpecHandles.Reset();
	GrantedAttributeSets.Reset();
}

UObsidianAbilitySet::UObsidianAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UObsidianAbilitySet::GiveToAbilitySystem(UObsidianAbilitySystemComponent* ObsidianASC, FObsidianAbilitySet_GrantedHandles* GrantedHandles, UObject* SourceObject) const
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
		const FObsidianAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if(!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogObsidian, Error, TEXT("Granted Gameplay Ability [%d] on Ablity Set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		UObsidianGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UObsidianGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = ObsidianASC->GiveAbility(AbilitySpec);

		if(GrantedHandles)
		{
			GrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}
	
	TArray<FObsidianAbilitySet_GameplayEffect> LatentGameplayEffects;
	
	// Granting Gameplay Effects
	for(int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FObsidianAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if(!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogObsidian, Error, TEXT("Granted Gameplay Effect [%d] on Ablity Set [%s] is not valid."), EffectIndex, *GetNameSafe(this));
			continue;	
		}
		
		if(EffectToGrant.bIsDependentOnOtherAttributes == true)
		{
			LatentGameplayEffects.Add(EffectToGrant);
			continue;
		}

		const UGameplayEffect* EffectCDO = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		FGameplayEffectContextHandle ContextHandle = ObsidianASC->MakeEffectContext();
		ContextHandle.AddSourceObject(SourceObject);
		const FActiveGameplayEffectHandle GameplayEffectHandle = ObsidianASC->ApplyGameplayEffectToSelf(EffectCDO, EffectToGrant.EffectLevel, ContextHandle);

		if(GrantedHandles)
		{
			GrantedHandles->AddActiveGameplayEffectSpecHandle(GameplayEffectHandle);
		}
	}

	// Add Latent Gameplay Effects
	for(const FObsidianAbilitySet_GameplayEffect& Effect : LatentGameplayEffects)
	{
		const UGameplayEffect* EffectCDO = Effect.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		FGameplayEffectContextHandle ContextHandle = ObsidianASC->MakeEffectContext();
		ContextHandle.AddSourceObject(SourceObject);
		const FActiveGameplayEffectHandle GameplayEffectHandle = ObsidianASC->ApplyGameplayEffectToSelf(EffectCDO, Effect.EffectLevel, ContextHandle);

		if(GrantedHandles)
		{
			GrantedHandles->AddActiveGameplayEffectSpecHandle(GameplayEffectHandle);
		}
	}
	

	// Granting Attribute Sets.
	for(int32 SetIndex = 0; SetIndex < GrantedAttributeSets.Num(); ++SetIndex)
	{
		const FObsidianAbilitySet_AttributeSet& SetToGrant = GrantedAttributeSets[SetIndex];

		if(!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(LogObsidian, Error, TEXT("Granted Attribute Set [%d] on Ablity Set [%s] is not valid."), SetIndex, *GetNameSafe(this));
			continue;	
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(ObsidianASC->GetOwner(), SetToGrant.AttributeSet);
		ObsidianASC->AddAttributeSetSubobject(NewSet);

		if(GrantedHandles)
		{
			GrantedHandles->AddAttributeSet(NewSet);
		}
	}
}

void UObsidianAbilitySet::GiveToAbilitySystem(UObsidianAbilitySystemComponent* ObsidianASC,
	FObsidianAbilitySet_GrantedHandles* GrantedHandles, const float LevelOverride, UObject* SourceObject) const
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
		const FObsidianAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if(!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogObsidian, Error, TEXT("Granted Gameplay Ability [%d] on Ablity Set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		UObsidianGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UObsidianGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, LevelOverride);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = ObsidianASC->GiveAbility(AbilitySpec);

		if(GrantedHandles)
		{
			GrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}
	
	TArray<FObsidianAbilitySet_GameplayEffect> LatentGameplayEffects;
	
	// Granting Gameplay Effects
	for(int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FObsidianAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if(!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogObsidian, Error, TEXT("Granted Gameplay Effect [%d] on Ablity Set [%s] is not valid."), EffectIndex, *GetNameSafe(this));
			continue;	
		}
		
		if(EffectToGrant.bIsDependentOnOtherAttributes == true)
		{
			LatentGameplayEffects.Add(EffectToGrant);
			continue;
		}

		const UGameplayEffect* EffectCDO = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		FGameplayEffectContextHandle ContextHandle = ObsidianASC->MakeEffectContext();
		ContextHandle.AddSourceObject(SourceObject);
		const FActiveGameplayEffectHandle GameplayEffectHandle = ObsidianASC->ApplyGameplayEffectToSelf(EffectCDO, LevelOverride, ContextHandle);

		if(GrantedHandles)
		{
			GrantedHandles->AddActiveGameplayEffectSpecHandle(GameplayEffectHandle);
		}
	}

	// Add Latent Gameplay Effects
	for(const FObsidianAbilitySet_GameplayEffect& Effect : LatentGameplayEffects)
	{
		const UGameplayEffect* EffectCDO = Effect.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		FGameplayEffectContextHandle ContextHandle = ObsidianASC->MakeEffectContext();
		ContextHandle.AddSourceObject(SourceObject);
		const FActiveGameplayEffectHandle GameplayEffectHandle = ObsidianASC->ApplyGameplayEffectToSelf(EffectCDO, LevelOverride, ContextHandle);

		if(GrantedHandles)
		{
			GrantedHandles->AddActiveGameplayEffectSpecHandle(GameplayEffectHandle);
		}
	}
	

	// Granting Attribute Sets.
	for(int32 SetIndex = 0; SetIndex < GrantedAttributeSets.Num(); ++SetIndex)
	{
		const FObsidianAbilitySet_AttributeSet& SetToGrant = GrantedAttributeSets[SetIndex];

		if(!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(LogObsidian, Error, TEXT("Granted Attribute Set [%d] on Ablity Set [%s] is not valid."), SetIndex, *GetNameSafe(this));
			continue;	
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(ObsidianASC->GetOwner(), SetToGrant.AttributeSet);
		ObsidianASC->AddAttributeSetSubobject(NewSet);

		if(GrantedHandles)
		{
			GrantedHandles->AddAttributeSet(NewSet);
		}
	}
}

#if WITH_EDITOR
EDataValidationResult FObsidianAbilitySet_GameplayAbility::ValidateData(FDataValidationContext& Context, const int Index) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	if(Ability == nullptr)
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Abilty at index [%i] is null! \n"
			"Please set a valid Ability class or delete this index entry in the Granted Gameplay Abilities array"), Index));

		Context.AddError(ErrorMessage);
	}

	/*
	 * This is actually bad, I don't want to be forced to specify input tag for some abilties
	 *
	if(!InputTag.IsValid())
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Input Tag at index [%i] is invalid! \n"
			"Please set a valid Input Tag or delete this index entry in the Granted Gameplay Abilities array"), Index));

		Context.AddError(ErrorMessage);
	}
	*/

	return Result;
}

EDataValidationResult FObsidianAbilitySet_GameplayEffect::ValidateData(FDataValidationContext& Context, const int Index) const
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

EDataValidationResult FObsidianAbilitySet_AttributeSet::ValidateData(FDataValidationContext& Context, const int Index) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	if(AttributeSet == nullptr)
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Attribute Set at index [%i] is null! \n"
			"Please set a valid Attribute Set class or delete this index entry in the Granted Attribute Sets array"), Index));

		Context.AddError(ErrorMessage);
	}

	return Result;
}

EDataValidationResult UObsidianAbilitySet::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	unsigned int AbilityIndex = 0;
	for(const FObsidianAbilitySet_GameplayAbility& Ability : GrantedGameplayAbilities)
	{
		Result =  CombineDataValidationResults(Result, Ability.ValidateData(Context, AbilityIndex));
		AbilityIndex++;
	}
	
	unsigned int EffectIndex = 0;
	for(const FObsidianAbilitySet_GameplayEffect& Effect : GrantedGameplayEffects)
	{
		Result =  CombineDataValidationResults(Result, Effect.ValidateData(Context, EffectIndex));
		EffectIndex++;
	}

	unsigned int SetIndex = 0;
	for(const FObsidianAbilitySet_AttributeSet& Set : GrantedAttributeSets)
	{
		Result =  CombineDataValidationResults(Result, Set.ValidateData(Context, SetIndex));
		SetIndex++;
	}

	return Result;
}
#endif // ~ With Editor

