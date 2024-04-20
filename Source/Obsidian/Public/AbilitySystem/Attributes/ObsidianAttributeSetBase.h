// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"

#include "ObsidianAttributeSetBase.generated.h"

class AObsidianCharacterBase;
class UObsidianAbilitySystemComponent;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT()
struct FObsidianEffectProperties
{
	GENERATED_BODY()

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
