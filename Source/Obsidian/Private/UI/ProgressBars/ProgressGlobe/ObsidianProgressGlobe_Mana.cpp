// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobe_Mana.h"

// ~ Core
#include "CommonTextBlock.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetMathLibrary.h"

// ~ Project
#include "CharacterComponents/Attributes/ObsidianHeroAttributesComponent.h"
#include "UI/WidgetControllers/ObMainOverlayWidgetController.h"

void UObsidianProgressGlobe_Mana::HandleWidgetControllerSet()
{
	Super::HandleWidgetControllerSet();

	MainOverlayWidgetController->OnManaChangedDelegate.AddDynamic(this, &ThisClass::OnManaChanged);
	MainOverlayWidgetController->OnMaxManaChangedDelegate.AddDynamic(this, &ThisClass::OnMaxManaChanged);
	MainOverlayWidgetController->OnSpecialResourceChangedDelegate.AddDynamic(this, &ThisClass::OnSpecialResourceChanged);
	MainOverlayWidgetController->OnMaxSpecialResourceChangedDelegate.AddDynamic(this, &ThisClass::OnMaxSpecialResourceChanged);

	InitializeSpecialResourceVisuals();
}

void UObsidianProgressGlobe_Mana::InitializeSpecialResourceVisuals()
{
	const FObsidianSpecialResourceVisuals SpecialResourceVisuals = MainOverlayWidgetController->GetSpecialResourceVisuals();
	SpecialResourceAttributeName_TextBlock->SetText(SpecialResourceVisuals.SpecialResourceName);
	
	FProgressBarStyle Style;
	Style.BackgroundImage.TintColor = FSlateColor(FLinearColor::Transparent);
	Style.FillImage = SpecialResourceVisuals.SpecialResourceGlobeMaterial;
	SpecialResource_ProgressGlobe->SetWidgetStyle(Style);
}

void UObsidianProgressGlobe_Mana::OnManaChanged(float NewMana)
{
	ShouldGhostGlobeDecrease(NewMana, Mana, MaxMana);
	
	Mana = NewMana;

	const float ProgressBarPercent = UKismetMathLibrary::SafeDivide(Mana, MaxMana);
	Mana_ProgressGlobe->SetPercent(ProgressBarPercent);

	const int32 ManaFloored = FMath::FloorToInt(Mana);
	const int32 MaxManaFloored = FMath::FloorToInt(MaxMana);
	
	const FText AttributeText = FText::FromString(FString::Printf(TEXT("%d/%d"), ManaFloored, MaxManaFloored));
	ManaAttributeCount_TextBlock->SetText(AttributeText);
}

void UObsidianProgressGlobe_Mana::OnMaxManaChanged(float NewMaxMana)
{
	MaxMana = NewMaxMana;

	const float ProgressBarPercent = UKismetMathLibrary::SafeDivide(Mana, MaxMana);
	Mana_ProgressGlobe->SetPercent(ProgressBarPercent);

	const int32 ManaFloored = FMath::FloorToInt(Mana);
	const int32 MaxManaFloored = FMath::FloorToInt(MaxMana);
	
	const FText AttributeText = FText::FromString(FString::Printf(TEXT("%d/%d"), ManaFloored, MaxManaFloored));
	ManaAttributeCount_TextBlock->SetText(AttributeText);
}

void UObsidianProgressGlobe_Mana::OnSpecialResourceChanged(float NewSpecialResource)
{
	SpecialResource = NewSpecialResource;

	const float ProgressBarPercent = UKismetMathLibrary::SafeDivide(SpecialResource, MaxSpecialResource);
	SpecialResource_ProgressGlobe->SetPercent(ProgressBarPercent);
	
	const int32 SpecialResourceFloored = FMath::FloorToInt(SpecialResource);
	const int32 MaxSpecialResourceFloored = FMath::FloorToInt(MaxSpecialResource);
	
	const FText AttributeText = FText::FromString(FString::Printf(TEXT("%d/%d"), SpecialResourceFloored, MaxSpecialResourceFloored));
	SpecialResourceAttributeCount_TextBlock->SetText(AttributeText);
}

void UObsidianProgressGlobe_Mana::OnMaxSpecialResourceChanged(float NewMaxSpecialResource)
{
	MaxSpecialResource = NewMaxSpecialResource;

	const float ProgressBarPercent = UKismetMathLibrary::SafeDivide(SpecialResource, MaxSpecialResource);
	SpecialResource_ProgressGlobe->SetPercent(ProgressBarPercent);

	const int32 SpecialResourceFloored = FMath::FloorToInt(SpecialResource);
	const int32 MaxSpecialResourceFloored = FMath::FloorToInt(MaxSpecialResource);
	
	const FText AttributeText = FText::FromString(FString::Printf(TEXT("%d/%d"), SpecialResourceFloored, MaxSpecialResourceFloored));
	SpecialResourceAttributeCount_TextBlock->SetText(AttributeText);
}
