// Copyright 2026 out of sCope team - intrxx

#include "Debug/ObsidianDebugMenuTypes.h"

#if WITH_OBSIDIAN_DEBUG_MENU

#include <AbilitySystemComponent.h>
#include <AbilitySystemGlobals.h>
#include <AssetRegistry/IAssetRegistry.h>
#include <Engine/Engine.h>
#include <GameFramework/Pawn.h>
#include <GameFramework/PlayerController.h>
#include <GameFramework/PlayerState.h>
#include <GameplayEffect.h>
#include <SlateIM.h>

#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Obsidian/ObsidianLogCategories.h"

// ~ FObsidianDebugMenuContext

bool FObsidianDebugMenuContext::HasAuthority() const
{
	return World && World->GetNetMode() != NM_Client;
}

AObsidianPlayerController* FObsidianDebugMenuContext::GetObsidianPC() const
{
	return Cast<AObsidianPlayerController>(PlayerController);
}

UAbilitySystemComponent* FObsidianDebugMenuContext::GetPlayerASC() const
{
	if (UAbilitySystemComponent* PawnASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn))
	{
		return PawnASC;
	}

	if (PlayerController)
	{
		return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerController->PlayerState);
	}
	return nullptr;
}

void FObsidianDebugMenuContext::Notify(const FString& Message) const
{
	UE_LOG(ObLogDebugMenu, Log, TEXT("%s"), *Message);

	if (StatusMessage)
	{
		*StatusMessage = Message;
	}
}

void FObsidianDebugMenuContext::ExecConsoleCommand(const FString& Command) const
{
	if (World == nullptr || GEngine == nullptr || Command.IsEmpty())
	{
		return;
	}

	if (APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(World))
	{
		LocalPlayerController->ConsoleCommand(Command);
	}
	else
	{
		GEngine->Exec(World, *Command);
	}

	Notify(FString::Printf(TEXT("Executed [%s]."), *Command));
}

// ~ FObsidianDebugComboBox

bool FObsidianDebugComboBox::Draw(const TArray<FString>& NewOptions, int32& InOutIndex, const bool bSearchable)
{
	if (NewOptions.IsEmpty())
	{
		Options.Reset();
		LastIndex = INDEX_NONE;
		InOutIndex = INDEX_NONE;

		SlateIM::VAlign(VAlign_Center);
		SlateIM::Text(TEXT("<none>"), {.Color = FStyleColors::AccentGray});
		return false;
	}

	InOutIndex = FMath::Clamp(InOutIndex, 0, NewOptions.Num() - 1);

	// SlateIM hands the new options to the Searchable Combo Box but never makes it refresh the list it shows (it only does
	// that when the search text changes), so it keeps listing the old options and picking any of them ends up as no
	// selection at all. Drawing a widget of another type in its place for a frame makes SlateIM create a fresh one.
	if (bSearchable && Options.IsEmpty() == false && Options != NewOptions)
	{
		Options = NewOptions;
		LastIndex = InOutIndex;
		SlateIM::ComboBox(Options, InOutIndex, {.bForceRefresh = true, .bSearchable = false});
		return false;
	}

	// The widget caches both the options and the selection, so it needs to be told when any of these changes from the outside.
	bool bForceRefresh = InOutIndex != LastIndex;
	if (Options != NewOptions)
	{
		Options = NewOptions;
		bForceRefresh = true;
	}

	const bool bSelectionChanged = SlateIM::ComboBox(Options, InOutIndex, {.bForceRefresh = bForceRefresh, .bSearchable = bSearchable});
	LastIndex = InOutIndex;
	return bSelectionChanged;
}

// ~ FObsidianDebugClassPicker

bool FObsidianDebugClassPicker::Draw(const UClass* BaseClass)
{
	bool bSelectionChanged = false;
	if (GatheredForClass != BaseClass)
	{
		Refresh(BaseClass);
		bSelectionChanged = true;
	}

	SlateIM::BeginHorizontalStack();
	{
		SlateIM::MinWidth(320.0f);
		bSelectionChanged |= ComboBox.Draw(ClassNames, SelectedIndex, true);

		SlateIM::SetToolTip(TEXT("Gathers the classes again, use it after creating new Blueprints."));
		if (SlateIM::Button(TEXT("Refresh")))
		{
			Refresh(BaseClass);
			bSelectionChanged = true;
		}
	}
	SlateIM::EndHorizontalStack();

	return bSelectionChanged;
}

void FObsidianDebugClassPicker::Refresh(const UClass* BaseClass)
{
	const FString PreviouslySelectedClassName = GetSelectedClassName();

	ClassPaths.Reset();
	ClassNames.Reset();
	GatheredForClass = BaseClass;

	if (BaseClass == nullptr)
	{
		return;
	}

	TSet<FTopLevelAssetPath> DerivedClassPaths;
	IAssetRegistry::GetChecked().GetDerivedClassNames({BaseClass->GetClassPathName()}, TSet<FTopLevelAssetPath>(), DerivedClassPaths);

	for (const FTopLevelAssetPath& ClassPath : DerivedClassPaths)
	{
		const FString ClassName = ClassPath.GetAssetName().ToString();
		if (ClassName.StartsWith(TEXT("SKEL_")) || ClassName.StartsWith(TEXT("REINST_")) || ClassName.StartsWith(TEXT("TRASHCLASS_")))
		{
			continue;
		}

		// Classes that exist just for the automation tests are of no use in the game.
		if (ClassPath.GetPackageName() == TEXT("/Script/ObsidianTests"))
		{
			continue;
		}

		// Only the loaded classes can be validated here, the rest is validated once it gets loaded.
		if (const UClass* LoadedClass = FindObject<UClass>(ClassPath))
		{
			if (LoadedClass->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists))
			{
				continue;
			}
		}

		FSoftClassPath SoftClassPath(ClassPath.ToString());
		if (ClassFilter)
		{
			const UClass* ClassToFilter = SoftClassPath.TryLoadClass<UObject>();
			if (ClassToFilter == nullptr || ClassToFilter->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated) || ClassFilter(ClassToFilter) == false)
			{
				continue;
			}
		}

		ClassPaths.Add(MoveTemp(SoftClassPath));
	}

	ClassPaths.Sort([](const FSoftClassPath& A, const FSoftClassPath& B)
		{
			return A.GetAssetName() < B.GetAssetName();
		});

	ClassNames.Reserve(ClassPaths.Num());
	for (const FSoftClassPath& ClassPath : ClassPaths)
	{
		FString ClassName = DisplayNameProvider ? DisplayNameProvider(ClassPath) : ClassPath.GetAssetName();
		ClassName.RemoveFromEnd(TEXT("_C"));
		ClassNames.Add(MoveTemp(ClassName));
	}

	// Try to keep the selection in place.
	const int32 PreviouslySelectedIndex = ClassNames.IndexOfByKey(PreviouslySelectedClassName);
	SelectedIndex = PreviouslySelectedIndex != INDEX_NONE ? PreviouslySelectedIndex : 0;
}

UClass* FObsidianDebugClassPicker::LoadSelectedClass() const
{
	if (ClassPaths.IsValidIndex(SelectedIndex) == false)
	{
		return nullptr;
	}

	UClass* LoadedClass = ClassPaths[SelectedIndex].TryLoadClass<UObject>();
	if (LoadedClass == nullptr)
	{
		UE_LOG(ObLogDebugMenu, Warning, TEXT("Could not load class [%s]."), *ClassPaths[SelectedIndex].ToString());
		return nullptr;
	}

	const UClass* BaseClass = GatheredForClass.Get();
	if ((BaseClass && LoadedClass->IsChildOf(BaseClass) == false) || LoadedClass->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated))
	{
		UE_LOG(ObLogDebugMenu, Warning, TEXT("Class [%s] is abstract, deprecated or of a wrong type."), *GetNameSafe(LoadedClass));
		return nullptr;
	}

	return LoadedClass;
}

FString FObsidianDebugClassPicker::GetSelectedClassName() const
{
	return ClassNames.IsValidIndex(SelectedIndex) ? ClassNames[SelectedIndex] : FString();
}

// ~ ObsidianDebugUI

void ObsidianDebugUI::BeginTabContent()
{
	SlateIM::Fill();
	SlateIM::HAlign(HAlign_Fill);
	SlateIM::VAlign(VAlign_Fill);
	SlateIM::Padding(FMargin(8.0f));
	SlateIM::BeginScrollBox();
}

void ObsidianDebugUI::EndTabContent()
{
	SlateIM::EndScrollBox();
}

void ObsidianDebugUI::Section(const FStringView& Title)
{
	SlateIM::Padding(FMargin(0.0f, 12.0f, 0.0f, 4.0f));
	SlateIM::Text(Title, {.Color = FStyleColors::AccentBlue});
}

void ObsidianDebugUI::Label(const FStringView& Text, const float Width)
{
	SlateIM::VAlign(VAlign_Center);
	SlateIM::MinWidth(Width);
	SlateIM::Text(Text);
}

void ObsidianDebugUI::WarningText(const FStringView& Text)
{
	SlateIM::Text(Text, {.Color = FStyleColors::Warning});
}

FString ObsidianDebugUI::GetCleanClassName(const UObject* ClassOrDefaultObject)
{
	FString Name = GetNameSafe(ClassOrDefaultObject);
	Name.RemoveFromStart(TEXT("Default__"));
	Name.RemoveFromEnd(TEXT("_C"));
	return Name;
}

// ~ ObsidianDebugGAS

bool ObsidianDebugGAS::ApplyInstantAttributeMod(UAbilitySystemComponent* SourceASC, UAbilitySystemComponent* TargetASC,
	const FGameplayAttribute& Attribute, const EGameplayModOp::Type ModifierOp, const float Magnitude)
{
	if (TargetASC == nullptr || Attribute.IsValid() == false || TargetASC->HasAttributeSetForAttribute(Attribute) == false)
	{
		return false;
	}

	if (SourceASC == nullptr)
	{
		SourceASC = TargetASC;
	}

	UGameplayEffect* InstantEffect = NewObject<UGameplayEffect>(GetTransientPackage());
	InstantEffect->DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo& ModifierInfo = InstantEffect->Modifiers.AddDefaulted_GetRef();
	ModifierInfo.Attribute = Attribute;
	ModifierInfo.ModifierOp = ModifierOp;
	ModifierInfo.ModifierMagnitude = FScalableFloat(Magnitude);

	SourceASC->ApplyGameplayEffectToTarget(InstantEffect, TargetASC, 1.0f, SourceASC->MakeEffectContext());
	return true;
}

bool ObsidianDebugGAS::Kill(UAbilitySystemComponent* TargetASC, UAbilitySystemComponent* InstigatorASC)
{
	float Health = 0.0f;
	if (GetAttributeValue(TargetASC, UObsidianCommonAttributeSet::GetHealthAttribute(), Health) == false)
	{
		return false;
	}

	float EnergyShield = 0.0f;
	GetAttributeValue(TargetASC, UObsidianCommonAttributeSet::GetEnergyShieldAttribute(), EnergyShield);

	// Damage needs to go through Incoming Damage for the regular death flow to be run.
	return ApplyInstantAttributeMod(InstigatorASC, TargetASC, UObsidianCommonAttributeSet::GetIncomingDamageAttribute(),
		EGameplayModOp::Override, Health + EnergyShield + 1.0f);
}

bool ObsidianDebugGAS::GetAttributeValue(const UAbilitySystemComponent* ASC, const FGameplayAttribute& Attribute, float& OutValue)
{
	if (ASC == nullptr || Attribute.IsValid() == false || ASC->HasAttributeSetForAttribute(Attribute) == false)
	{
		return false;
	}

	OutValue = ASC->GetNumericAttribute(Attribute);
	return true;
}

#endif // WITH_OBSIDIAN_DEBUG_MENU
