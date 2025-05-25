// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"

// ~ Project


#include "AttributeSet.h"
#include "ObsidianAttributeSetBase.generated.h"

class AObsidianCharacterBase;
class UObsidianAbilitySystemComponent;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/** 
 * Delegate used to broadcast attribute events, some of these parameters may be null on clients: 
 * @param EffectInstigator	The original instigating actor for this event
 * @param EffectCauser		The physical actor that caused the change
 * @param EffectSpec		The full effect spec for this change
 * @param EffectMagnitude	The raw magnitude, this is before clamping
 * @param OldValue			The value of the attribute before it was changed
 * @param NewValue			The value after it was changed
*/
DECLARE_MULTICAST_DELEGATE_SixParams(FObsidianAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/, const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/, float /*OldValue*/, float /*NewValue*/);

USTRUCT()
struct FObsidianEffectProperties
{
	GENERATED_BODY()
public:
	void Reset();
	
public:
	FObsidianEffectProperties(){}
	
	UPROPERTY()
	FGameplayEffectContextHandle EffectContextHandle;
	
	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;
	
	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY()
	AController* SourceController = nullptr;

	UPROPERTY()
	AObsidianCharacterBase* SourceCharacter = nullptr;
	
	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;

	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY()
	AController* TargetController = nullptr;

	UPROPERTY()
	AObsidianCharacterBase* TargetCharacter = nullptr;

	UPROPERTY()
	AActor* Instigator = nullptr;

	UPROPERTY()
	AActor* EffectCauser = nullptr;

	UPROPERTY()
	bool bIsPlayerCharacter = false;

	UPROPERTY()
	bool bIsBoss = false;

	UPROPERTY()
	bool bCanHitReact = false;
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:
	UObsidianAttributeSetBase();

	virtual UWorld* GetWorld() const override;
	
	/** Does not perform a check in the Get function, so it is a good idea to check it before use */
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const;

protected:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, /** OUT */ FObsidianEffectProperties& Props) const;
};
