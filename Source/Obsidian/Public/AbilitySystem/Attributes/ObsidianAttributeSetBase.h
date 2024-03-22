// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "ObsidianAttributeSetBase.generated.h"

class UObsidianAbilitySystemComponent;

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
		GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

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

	virtual void ClampAttribute(const float MaxAttribute, float& Attribute);

	/** Does not perform a check in the Get function so it is a good idea to check it before use */
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const;
};
