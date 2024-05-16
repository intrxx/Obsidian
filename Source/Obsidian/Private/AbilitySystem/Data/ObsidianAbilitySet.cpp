// Copyright 2024 out of sCope team - Michał Ogiński


#include "AbilitySystem/Data/ObsidianAbilitySet.h"
#include "AbilitySystem/Abilities/ObsidianGameplayAbility.h"

#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "Obsidian/Obsidian.h"

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

void UObsidianAbilitySet::GiveToAbilitySystem(UObsidianAbilitySystemComponent* ObsidianASC,
	FObsidianAbilitySet_GrantedHandles* GrantedHandles, UObject* SourceObject) const
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
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

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

