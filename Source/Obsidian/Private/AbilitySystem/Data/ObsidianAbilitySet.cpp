// Copyright 2026 out of sCope team - intrxx

#include "AbilitySystem/Data/ObsidianAbilitySet.h"

#include "AbilitySystem/Abilities/ObsidianGameplayAbility.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Obsidian/ObsidianLogCategories.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // ~ With Editor


void FObsidianAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& InHandle)
{
	if(InHandle.IsValid())
	{
		GameplayAbilitySpecHandles.Add(InHandle);
	}
}

void FObsidianAbilitySet_GrantedHandles::AddActiveGameplayEffectSpecHandle(const FActiveGameplayEffectHandle& InHandle)
{
	if(InHandle.IsValid())
	{
		GameplayEffectHandles.Add(InHandle);
	}
}

void FObsidianAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* InAttributeSet)
{
	GrantedAttributeSets.Add(InAttributeSet);
}

void FObsidianAbilitySet_GrantedHandles::TakeFromAbilitySystem(UObsidianAbilitySystemComponent* InObsidianASC)
{
	check(InObsidianASC);

	if(!InObsidianASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	for(const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if(Handle.IsValid())
		{
			InObsidianASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for(const FGameplayAbilitySpecHandle& Handle : GameplayAbilitySpecHandles)
	{
		if(Handle.IsValid())
		{
			InObsidianASC->ClearAbility(Handle);
		}
	}

	for(UAttributeSet* AttributeSet : GrantedAttributeSets)
	{ 
		if(AttributeSet)
		{
			InObsidianASC->RemoveSpawnedAttribute(AttributeSet);
		}
	}

	GameplayEffectHandles.Reset();
	GameplayAbilitySpecHandles.Reset();
	GrantedAttributeSets.Reset();
}

UObsidianAbilitySet::UObsidianAbilitySet(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
}

void UObsidianAbilitySet::GiveToAbilitySystem(UObsidianAbilitySystemComponent* InObsidianASC, FObsidianAbilitySet_GrantedHandles* OutGrantedHandles, UObject* InSourceObject) const
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
		const FObsidianAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if(!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(ObLogAbilitySystem, Error, TEXT("Granted Gameplay Ability [%d] on Ablity Set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		UObsidianGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UObsidianGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = InSourceObject;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = InObsidianASC->GiveAbility(AbilitySpec);

		if(OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}
	
	TArray<FObsidianAbilitySet_GameplayEffect> LatentGameplayEffects;
	
	// Granting Gameplay Effects
	for(int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FObsidianAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if(!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(ObLogAbilitySystem, Error, TEXT("Granted Gameplay Effect [%d] on Ablity Set [%s] is not valid."), EffectIndex, *GetNameSafe(this));
			continue;	
		}
		
		if(EffectToGrant.bIsDependentOnOtherAttributes == true)
		{
			LatentGameplayEffects.Add(EffectToGrant);
			continue;
		}

		const UGameplayEffect* EffectCDO = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		FGameplayEffectContextHandle ContextHandle = InObsidianASC->MakeEffectContext();
		ContextHandle.AddSourceObject(InSourceObject);
		const FActiveGameplayEffectHandle GameplayEffectHandle = InObsidianASC->ApplyGameplayEffectToSelf(EffectCDO, EffectToGrant.EffectLevel, ContextHandle);

		if(OutGrantedHandles)
		{
			OutGrantedHandles->AddActiveGameplayEffectSpecHandle(GameplayEffectHandle);
		}
	}

	// Add Latent Gameplay Effects
	for(const FObsidianAbilitySet_GameplayEffect& Effect : LatentGameplayEffects)
	{
		const UGameplayEffect* EffectCDO = Effect.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		FGameplayEffectContextHandle ContextHandle = InObsidianASC->MakeEffectContext();
		ContextHandle.AddSourceObject(InSourceObject);
		const FActiveGameplayEffectHandle GameplayEffectHandle = InObsidianASC->ApplyGameplayEffectToSelf(EffectCDO, Effect.EffectLevel, ContextHandle);

		if(OutGrantedHandles)
		{
			OutGrantedHandles->AddActiveGameplayEffectSpecHandle(GameplayEffectHandle);
		}
	}
	

	// Granting Attribute Sets.
	for(int32 SetIndex = 0; SetIndex < GrantedAttributeSets.Num(); ++SetIndex)
	{
		const FObsidianAbilitySet_AttributeSet& SetToGrant = GrantedAttributeSets[SetIndex];

		if(!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(ObLogAbilitySystem, Error, TEXT("Granted Attribute Set [%d] on Ablity Set [%s] is not valid."), SetIndex, *GetNameSafe(this));
			continue;	
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(InObsidianASC->GetOwner(), SetToGrant.AttributeSet);
		InObsidianASC->AddAttributeSetSubobject(NewSet);

		if(OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}
}

void UObsidianAbilitySet::GiveToAbilitySystem(UObsidianAbilitySystemComponent* InObsidianASC,
	FObsidianAbilitySet_GrantedHandles* OutGrantedHandles, const float InLevelOverride, UObject* InSourceObject) const
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
		const FObsidianAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if(!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(ObLogAbilitySystem, Error, TEXT("Granted Gameplay Ability [%d] on Ablity Set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		UObsidianGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UObsidianGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, InLevelOverride);
		AbilitySpec.SourceObject = InSourceObject;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = InObsidianASC->GiveAbility(AbilitySpec);

		if(OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}
	
	TArray<FObsidianAbilitySet_GameplayEffect> LatentGameplayEffects;
	
	// Granting Gameplay Effects
	for(int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FObsidianAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if(!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(ObLogAbilitySystem, Error, TEXT("Granted Gameplay Effect [%d] on Ablity Set [%s] is not valid."), EffectIndex, *GetNameSafe(this));
			continue;	
		}
		
		if(EffectToGrant.bIsDependentOnOtherAttributes == true)
		{
			LatentGameplayEffects.Add(EffectToGrant);
			continue;
		}

		const UGameplayEffect* EffectCDO = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		FGameplayEffectContextHandle ContextHandle = InObsidianASC->MakeEffectContext();
		ContextHandle.AddSourceObject(InSourceObject);
		const FActiveGameplayEffectHandle GameplayEffectHandle = InObsidianASC->ApplyGameplayEffectToSelf(EffectCDO, InLevelOverride, ContextHandle);

		if(OutGrantedHandles)
		{
			OutGrantedHandles->AddActiveGameplayEffectSpecHandle(GameplayEffectHandle);
		}
	}

	// Add Latent Gameplay Effects
	for(const FObsidianAbilitySet_GameplayEffect& Effect : LatentGameplayEffects)
	{
		const UGameplayEffect* EffectCDO = Effect.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		FGameplayEffectContextHandle ContextHandle = InObsidianASC->MakeEffectContext();
		ContextHandle.AddSourceObject(InSourceObject);
		const FActiveGameplayEffectHandle GameplayEffectHandle = InObsidianASC->ApplyGameplayEffectToSelf(EffectCDO, InLevelOverride, ContextHandle);

		if(OutGrantedHandles)
		{
			OutGrantedHandles->AddActiveGameplayEffectSpecHandle(GameplayEffectHandle);
		}
	}
	

	// Granting Attribute Sets.
	for(int32 SetIndex = 0; SetIndex < GrantedAttributeSets.Num(); ++SetIndex)
	{
		const FObsidianAbilitySet_AttributeSet& SetToGrant = GrantedAttributeSets[SetIndex];

		if(!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(ObLogAbilitySystem, Error, TEXT("Granted Attribute Set [%d] on Ablity Set [%s] is not valid."), SetIndex, *GetNameSafe(this));
			continue;	
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(InObsidianASC->GetOwner(), SetToGrant.AttributeSet);
		InObsidianASC->AddAttributeSetSubobject(NewSet);

		if(OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}
}

#if WITH_EDITOR
EDataValidationResult FObsidianAbilitySet_GameplayAbility::ValidateData(FDataValidationContext& InContext, const int InIndex) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	if(Ability == nullptr)
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Abilty at index [%i] is null! \n"
			"Please set a valid Ability class or delete this index entry in the Granted Gameplay Abilities array"), InIndex));

		InContext.AddError(ErrorMessage);
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

EDataValidationResult FObsidianAbilitySet_GameplayEffect::ValidateData(FDataValidationContext& InContext, const int InIndex) const
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

EDataValidationResult FObsidianAbilitySet_AttributeSet::ValidateData(FDataValidationContext& InContext, const int InIndex) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	if(AttributeSet == nullptr)
	{
		Result = EDataValidationResult::Invalid;

		const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Attribute Set at index [%i] is null! \n"
			"Please set a valid Attribute Set class or delete this index entry in the Granted Attribute Sets array"), InIndex));

		InContext.AddError(ErrorMessage);
	}

	return Result;
}

EDataValidationResult UObsidianAbilitySet::IsDataValid(FDataValidationContext& InContext) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(InContext), EDataValidationResult::Valid);

	unsigned int AbilityIndex = 0;
	for(const FObsidianAbilitySet_GameplayAbility& Ability : GrantedGameplayAbilities)
	{
		Result =  CombineDataValidationResults(Result, Ability.ValidateData(InContext, AbilityIndex));
		AbilityIndex++;
	}
	
	unsigned int EffectIndex = 0;
	for(const FObsidianAbilitySet_GameplayEffect& Effect : GrantedGameplayEffects)
	{
		Result =  CombineDataValidationResults(Result, Effect.ValidateData(InContext, EffectIndex));
		EffectIndex++;
	}

	unsigned int SetIndex = 0;
	for(const FObsidianAbilitySet_AttributeSet& Set : GrantedAttributeSets)
	{
		Result =  CombineDataValidationResults(Result, Set.ValidateData(InContext, SetIndex));
		SetIndex++;
	}

	return Result;
}
#endif // ~ With Editor

