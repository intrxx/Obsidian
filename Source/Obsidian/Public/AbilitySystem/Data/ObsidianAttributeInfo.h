// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "ObsidianAttributeInfo.generated.h"

USTRUCT(BlueprintType)
struct FOAttributeInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute Tag")
	FGameplayTag AttributeTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute Name")
	FText AttributeName = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute Description")
	FText AttributeDescription = FText();

	UPROPERTY(BlueprintReadOnly)
	float AttributeValue = 0.f;
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianAttributeInfo : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	FOAttributeInfo FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogIfNotFound = false) const;
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute Info")
	TArray<FOAttributeInfo> AttributeInformation;
};
