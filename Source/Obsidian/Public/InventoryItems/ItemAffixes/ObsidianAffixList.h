// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project
#include "ObsidianTypes/ObsidianItemTypes.h"

#include "Engine/DataAsset.h"
#include "ObsidianAffixList.generated.h"

class UGameplayEffect;

USTRUCT()
struct FObsidianAffixClass
{
	GENERATED_BODY()

public:
	TArray<FObsidianDynamicItemAffix> GetAllAffixesUpToQuality(const int32 UpToTreasureQuality) const;
	TArray<FObsidianDynamicItemAffix> GetAllAffixesUpToQualityForCategory(const int32 UpToTreasureQuality, const FGameplayTag& ForCategory) const;
	
public:
	//TODO(intrxx) Currently not used for anything, keep or delete?
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FName AffixClassName;

	/** Types of Affixes added to this class. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	EObsidianAffixType AffixClassType = EObsidianAffixType::None;

	/** List of actual Item Affixes. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<FObsidianDynamicItemAffix> ItemAffixList;
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianAffixList : public UDataAsset
{
	GENERATED_BODY()

public:
	UObsidianAffixList(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostInitProperties() override;
	virtual void PostLoad() override;
	
	TArray<FObsidianAffixClass> GetAllAffixClasses() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<FObsidianAffixClass> AffixClasses;
};
