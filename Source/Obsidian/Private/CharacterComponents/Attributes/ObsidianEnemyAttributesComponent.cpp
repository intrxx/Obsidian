// Copyright 2024 out of sCope team - Michał Ogiński

#include "CharacterComponents/Attributes/ObsidianEnemyAttributesComponent.h"
#include "AbilitySystem/ObsidianAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianEnemyAttributeSet.h"
#include "Obsidian/Obsidian.h"

UObsidianEnemyAttributesComponent::UObsidianEnemyAttributesComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	EnemyAttributeSet = nullptr;
}

void UObsidianEnemyAttributesComponent::InitializeWithAbilitySystem(UObsidianAbilitySystemComponent* InASC, AActor* Owner)
{
	AActor* CurrentOwner = GetOwner();
	check(CurrentOwner);
	
	Super::InitializeWithAbilitySystem(InASC, CurrentOwner);

	EnemyAttributeSet = AbilitySystemComponent->GetSet<UObsidianEnemyAttributeSet>();
	if (!EnemyAttributeSet)
	{
		UE_LOG(LogObsidian, Error, TEXT("ObsidianEnemyAttributesComponent: Cannot initialize Attributes Component for owner [%s] with NULL Enemy Set set on the Ability System."), *GetNameSafe(Owner));
		return;
	}

	EnemySpecificAttributeChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetEnemySpecificAttributeAttribute()).AddUObject(this, &ThisClass::EnemySpecificAttributeChanged);
}

void UObsidianEnemyAttributesComponent::UninitializeFromAbilitySystem()
{
	EnemySpecificAttributeChangedDelegateHandle.Reset();
	
	EnemyAttributeSet = nullptr;
	
	Super::UninitializeFromAbilitySystem();
}

void UObsidianEnemyAttributesComponent::EnemySpecificAttributeChanged(const FOnAttributeChangeData& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Implement Enemy Specific Attribute Changed!"));
}

float UObsidianEnemyAttributesComponent::GetEnemySpecificAttribute() const
{
	return (EnemyAttributeSet ? EnemyAttributeSet->GetEnemySpecificAttribute() : 0.0f);
}

FGameplayAttribute UObsidianEnemyAttributesComponent::GetEnemySpecificAttributeAttribute() const
{
	return (EnemyAttributeSet ? EnemyAttributeSet->GetEnemySpecificAttributeAttribute() : nullptr);
}

