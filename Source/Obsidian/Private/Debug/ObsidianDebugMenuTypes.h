// Copyright 2026 out of sCope team - intrxx

#pragma once

#if WITH_OBSIDIAN_DEBUG_MENU

#include <CoreMinimal.h>
#include <AttributeSet.h>
#include <GameplayEffectTypes.h>
#include <UObject/SoftObjectPath.h>

class AObsidianPlayerController;
class UAbilitySystemComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogObsidianDebugMenu, Log, All);

/**
 * What the Debug Menu is currently pointed at, resolved every frame from the World and Player pickers.
 */
struct FObsidianDebugMenuContext
{
public:
	bool IsValid() const
	{
		return World != nullptr;
	}

	/** Most of the debug actions mutate gameplay state, so they need to be run on the World that has authority. */
	bool HasAuthority() const;

	AObsidianPlayerController* GetObsidianPC() const;
	UAbilitySystemComponent* GetPlayerASC() const;

	/** Logs the message and shows it in the status bar of the menu. */
	void Notify(const FString& Message) const;

	/** Runs the command through the local Player if there is one, so viewport commands like "stat" or "show" work. */
	void ExecConsoleCommand(const FString& Command) const;

public:
	UWorld* World = nullptr;
	APlayerController* PlayerController = nullptr;
	APawn* Pawn = nullptr;

	FString* StatusMessage = nullptr;
};

/**
 * Combo Box that can be fed with options that are rebuilt every frame, takes care of refreshing the widget.
 */
struct FObsidianDebugComboBox
{
public:
	/** Returns true if the user changed the selection this frame. InOutIndex will be INDEX_NONE if there are no options. */
	bool Draw(const TArray<FString>& NewOptions, int32& InOutIndex, const bool bSearchable = false);

private:
	TArray<FString> Options;
	int32 LastIndex = INDEX_NONE;
};

/**
 * Searchable picker of every native and Blueprint class derived from given class, Blueprint classes are loaded on demand.
 */
struct FObsidianDebugClassPicker
{
public:
	/** Draws the picker with the Refresh button. Returns true if the selection changed this frame. */
	bool Draw(const UClass* BaseClass);

	void Refresh(const UClass* BaseClass);

	/** Loads the selected class, will be nullptr if nothing is selected or the class cannot be used. */
	UClass* LoadSelectedClass() const;
	FString GetSelectedClassName() const;

	/** Paths of every gathered class, in the order they are listed in the picker. */
	TConstArrayView<FSoftClassPath> GetClassPaths() const
	{
		return ClassPaths;
	}

public:
	/**
	 * Optional filter of the gathered classes, needs to be set before the picker is drawn for the first time.
	 * Caution, classes need to be loaded to be filtered, so every gathered class is loaded when the filter is set.
	 */
	TFunction<bool(const UClass* Class)> ClassFilter;

	/**
	 * Optional provider of the names the classes are listed with, the name of the class is used when it is not set.
	 * The names are also what the list is searched by.
	 */
	TFunction<FString(const FSoftClassPath& ClassPath)> DisplayNameProvider;

private:
	FObsidianDebugComboBox ComboBox;
	TArray<FSoftClassPath> ClassPaths;
	TArray<FString> ClassNames;
	int32 SelectedIndex = 0;

	TWeakObjectPtr<const UClass> GatheredForClass;
};

namespace ObsidianDebugUI
{
	/** Scrollable root of the tab, every tab should be drawn between these. */
	void BeginTabContent();
	void EndTabContent();

	void Section(const FStringView& Title);

	/** Fixed width label, meant to be used as the first widget of the horizontal stack. */
	void Label(const FStringView& Text, const float Width = 150.0f);

	void WarningText(const FStringView& Text);

	/** Strips the "Default__" and "_C" decorations out of the class or CDO name. */
	FString GetCleanClassName(const UObject* ClassOrDefaultObject);
}

namespace ObsidianDebugGAS
{
	/**
	 * Applies the modifier with a dynamic instant Gameplay Effect, unlike setting the base value directly this runs
	 * PostGameplayEffectExecute on the Attribute Sets, so things like death or leveling up are handled.
	 */
	bool ApplyInstantAttributeMod(UAbilitySystemComponent* SourceASC, UAbilitySystemComponent* TargetASC,
		const FGameplayAttribute& Attribute, const EGameplayModOp::Type ModifierOp, const float Magnitude);

	/** Deals enough damage to kill the Target, InstigatorASC is credited for the kill (Target itself if left empty). */
	bool Kill(UAbilitySystemComponent* TargetASC, UAbilitySystemComponent* InstigatorASC = nullptr);

	/** Returns false if given ASC does not own the Attribute. */
	bool GetAttributeValue(const UAbilitySystemComponent* ASC, const FGameplayAttribute& Attribute, float& OutValue);
}

#endif // WITH_OBSIDIAN_DEBUG_MENU
