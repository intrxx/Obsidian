// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "ObsidianAffixList.generated.h"

class UGameplayEffect;

USTRUCT()
struct FObsidianAffixClass
{
	GENERATED_BODY()

public:
	TArray<FObsidianDynamicItemAffix> GetAllAffixesUpToQuality(const int32 InUpToTreasureQuality) const;
	TArray<FObsidianDynamicItemAffix> GetAllAffixesUpToQualityForCategory(const int32 InUpToTreasureQuality,
		const FGameplayTag& InForCategory, const FGameplayTag& InForBaseType) const;

#if WITH_EDITOR
	EDataValidationResult ValidateData(FDataValidationContext& InContext, const FName InClassName, const int InIndex) const;
#endif
	
public:
	//TODO(intrxx) Currently not used for anything, keep or delete?
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FName AffixClassName;

	/** Types of Affixes added to this class. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	EObsidianAffixType AffixClassType = EObsidianAffixType::None;

	/** List of actual Item Affixes. */
	UPROPERTY(EditDefaultsOnly, Meta = (TitleProperty = "Affix: {AffixTag}"), Category = "Obsidian")
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
	UObsidianAffixList(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	virtual void PostInitProperties() override;
	virtual void PostLoad() override;
	
	TConstArrayView<FObsidianAffixClass> ReadAllAffixClasses() const;
	
	virtual void PreSave(FObjectPreSaveContext InSaveContext) override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& InPropertyChangedEvent) override;
	virtual EDataValidationResult IsDataValid(FDataValidationContext& InContext) const override;
#endif
	
protected:
	UPROPERTY(EditDefaultsOnly, Meta = (TitleProperty = "AffixClassName"), Category = "Obsidian")
	TArray<FObsidianAffixClass> AffixClasses;
};
