// Copyright 2026 out of sCope team - intrxx

#include "Debug/ObsidianDebugMenuTabs.h"

#if WITH_OBSIDIAN_DEBUG_MENU

#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "GameplayEffect.h"
#include "GameplayTagsManager.h"
#include "SlateIM.h"


namespace ObsidianDebugGASTab
{
	enum class EAbilityAction : uint8
	{
		None = 0,
		Activate,
		Cancel,
		Remove
	};

	/** Starts the table that takes the whole width of the tab and scrolls on its own if it gets too long. */
	void BeginSizedTable()
	{
		SlateIM::HAlign(HAlign_Fill);
		SlateIM::MinHeight(120.0f);
		SlateIM::MaxHeight(360.0f);
		SlateIM::BeginTable();
	}

	void TableTextCell(const FStringView& InText, const FSlateColor& InColor = FSlateColor::UseForeground())
	{
		if (SlateIM::NextTableCell())
		{
			SlateIM::Padding(FMargin(4.0f, 0.0f));
			SlateIM::VAlign(VAlign_Center);
			SlateIM::Text(InText, {.Color = InColor});
		}
	}
}

void FObsidianDebugTab_GAS::Draw(const FObsidianDebugMenuContext& InContext)
{
	UAbilitySystemComponent* ASC = DrawTargetPicker(InContext);
	if (ASC == nullptr)
	{
		ObsidianDebugUI::WarningText(TEXT("There is no Pawn with an Ability System Component in the chosen World."));
		return;
	}

	DrawAttributes(InContext, ASC);
	DrawAbilities(InContext, ASC);
	DrawEffects(InContext, ASC);
	DrawTags(InContext, ASC);
}

UAbilitySystemComponent* FObsidianDebugTab_GAS::DrawTargetPicker(const FObsidianDebugMenuContext& InContext)
{
	TArray<AActor*> Targets;
	TArray<FString> TargetNames;
	for (TActorIterator<APawn> It(InContext.World); It; ++It)
	{
		APawn* Pawn = *It;
		if (UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn) == nullptr)
		{
			continue;
		}

		// Pawn of the chosen Player is always the first option.
		if (Pawn == InContext.Pawn)
		{
			Targets.Insert(Pawn, 0);
			TargetNames.Insert(FString::Printf(TEXT("%s (chosen Player)"), *Pawn->GetName()), 0);
		}
		else
		{
			Targets.Add(Pawn);
			TargetNames.Add(Pawn->GetName());
		}
	}

	int32 TargetIndex = Targets.IndexOfByKey(TargetActor.Get());
	if (TargetIndex == INDEX_NONE)
	{
		TargetIndex = 0;
	}

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Target"));
		SlateIM::MinWidth(320.0f);
		TargetComboBox.Draw(TargetNames, TargetIndex, true);

		if (SlateIM::Button(TEXT("Use Player")))
		{
			TargetIndex = Targets.IndexOfByKey(InContext.Pawn);
		}
	}
	SlateIM::EndHorizontalStack();

	TargetActor = Targets.IsValidIndex(TargetIndex) ? Targets[TargetIndex] : nullptr;
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor.Get());
}

void FObsidianDebugTab_GAS::DrawAttributes(const FObsidianDebugMenuContext& InContext, UAbilitySystemComponent* InASC)
{
	using namespace ObsidianDebugGASTab;

	ObsidianDebugUI::Section(TEXT("Attributes"));

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Filter"));
		SlateIM::MinWidth(320.0f);
		SlateIM::EditableText(AttributeFilter, TEXT("Filter the attributes by name..."));
	}
	SlateIM::EndHorizontalStack();

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Amount"));
		SlateIM::SetToolTip(TEXT("Amount used by the [+], [-] and [Set] buttons of the attributes."));
		SlateIM::MinWidth(120.0f);
		SlateIM::SpinBox(AttributeAmount);

		SlateIM::Padding(FMargin(8.0f, 0.0f));
		SlateIM::VAlign(VAlign_Center);
		SlateIM::SetToolTip(TEXT("Changes the attribute with an instant Gameplay Effect, so the Attribute Set reacts to it the same way it does in"
			" the game (clamping, death, leveling up). Otherwise the base value is set directly."));
		SlateIM::CheckBox(bApplyAsInstantEffect, {.Label = TEXT("Apply As Instant Effect")});
	}
	SlateIM::EndHorizontalStack();

	TArray<FGameplayAttribute> Attributes;
	InASC->GetAllAttributes(Attributes);

	FGameplayAttribute AttributeToModify;
	EGameplayModOp::Type ModifierOp = EGameplayModOp::Additive;
	float ModifierMagnitude = 0.0f;

	BeginSizedTable();
	SlateIM::BeginTableHeader();
	SlateIM::InitialTableColumnWidth(380.0f);
	SlateIM::AddTableColumn(TEXT("Attribute"), TEXT("Attribute"));
	SlateIM::InitialTableColumnWidth(110.0f);
	SlateIM::AddTableColumn(TEXT("Base"), TEXT("Base"));
	SlateIM::InitialTableColumnWidth(110.0f);
	SlateIM::AddTableColumn(TEXT("Current"), TEXT("Current"));
	SlateIM::AddTableColumn(TEXT("Modify"), TEXT("Modify"));
	SlateIM::EndTableHeader();
	SlateIM::BeginTableBody();
	for (const FGameplayAttribute& Attribute : Attributes)
	{
		if (Attribute.IsValid() == false)
		{
			continue;
		}

		const FString AttributeName = FString::Printf(TEXT("%s.%s"), *GetNameSafe(Attribute.GetAttributeSetClass()), *Attribute.GetName());
		if (AttributeFilter.IsEmpty() == false && AttributeName.Contains(AttributeFilter) == false)
		{
			continue;
		}

		TableTextCell(AttributeName);
		TableTextCell(FString::SanitizeFloat(InASC->GetNumericAttributeBase(Attribute)));
		TableTextCell(FString::SanitizeFloat(InASC->GetNumericAttribute(Attribute)));

		if (SlateIM::NextTableCell())
		{
			SlateIM::BeginHorizontalStack();
			if (SlateIM::Button(TEXT("+")))
			{
				AttributeToModify = Attribute;
				ModifierOp = EGameplayModOp::Additive;
				ModifierMagnitude = AttributeAmount;
			}
			if (SlateIM::Button(TEXT("-")))
			{
				AttributeToModify = Attribute;
				ModifierOp = EGameplayModOp::Additive;
				ModifierMagnitude = -AttributeAmount;
			}
			if (SlateIM::Button(TEXT("Set")))
			{
				AttributeToModify = Attribute;
				ModifierOp = EGameplayModOp::Override;
				ModifierMagnitude = AttributeAmount;
			}
			SlateIM::EndHorizontalStack();
		}
	}
	SlateIM::EndTableBody();
	SlateIM::EndTable();

	if (AttributeToModify.IsValid())
	{
		if (bApplyAsInstantEffect)
		{
			ObsidianDebugGAS::ApplyInstantAttributeMod(InASC, InASC, AttributeToModify, ModifierOp, ModifierMagnitude);
		}
		else
		{
			const float NewBaseValue = ModifierOp == EGameplayModOp::Override ? ModifierMagnitude :
				InASC->GetNumericAttributeBase(AttributeToModify) + ModifierMagnitude;
			InASC->SetNumericAttributeBase(AttributeToModify, NewBaseValue);
		}

		InContext.Notify(FString::Printf(TEXT("%s [%s] of [%s] %s [%s]."), ModifierOp == EGameplayModOp::Override ? TEXT("Set") : TEXT("Changed"),
			*AttributeToModify.GetName(), *GetNameSafe(InASC->GetAvatarActor()), ModifierOp == EGameplayModOp::Override ? TEXT("to") : TEXT("by"),
			*FString::SanitizeFloat(ModifierMagnitude)));
	}

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Attribute Set"));
		AttributeSetPicker.Draw(UAttributeSet::StaticClass());

		SlateIM::SetToolTip(TEXT("Adds a new Attribute Set to the target, attributes of it are left with their default values."));
		if (SlateIM::Button(TEXT("Add Attribute Set")))
		{
			const UClass* AttributeSetClass = AttributeSetPicker.LoadSelectedClass();
			const bool bAlreadyOwned = InASC->GetSpawnedAttributes().ContainsByPredicate([AttributeSetClass](const UAttributeSet* InAttributeSet)
				{
					return InAttributeSet && InAttributeSet->GetClass() == AttributeSetClass;
				});

			if (AttributeSetClass == nullptr || bAlreadyOwned)
			{
				InContext.Notify(TEXT("Could not add the Attribute Set, it is invalid or the target already owns it."));
			}
			else
			{
				UAttributeSet* NewAttributeSet = NewObject<UAttributeSet>(InASC->GetOwnerActor(), AttributeSetClass);
				InASC->AddSpawnedAttribute(NewAttributeSet);
				InASC->ForceReplication();

				InContext.Notify(FString::Printf(TEXT("Added [%s] to [%s]."), *GetNameSafe(AttributeSetClass), *GetNameSafe(InASC->GetAvatarActor())));
			}
		}
	}
	SlateIM::EndHorizontalStack();
}

void FObsidianDebugTab_GAS::DrawAbilities(const FObsidianDebugMenuContext& InContext, UAbilitySystemComponent* InASC)
{
	using namespace ObsidianDebugGASTab;

	ObsidianDebugUI::Section(TEXT("Abilities"));

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Ability"));
		AbilityPicker.Draw(UGameplayAbility::StaticClass());
	}
	SlateIM::EndHorizontalStack();

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Level"));
		SlateIM::MinWidth(120.0f);
		SlateIM::SpinBox(AbilityLevel, {.Min = 1, .Max = 100});
	}
	SlateIM::EndHorizontalStack();

	SlateIM::BeginHorizontalStack();
	{
		if (InputTags.IsEmpty())
		{
			GatherInputTags();
		}

		ObsidianDebugUI::Label(TEXT("Input Tag"));
		SlateIM::SetToolTip(TEXT("Input that activates the ability, the same one that is set in the Ability Sets."));
		SlateIM::MinWidth(320.0f);
		InputTagComboBox.Draw(InputTagNames, InputTagIndex);
	}
	SlateIM::EndHorizontalStack();

	if (SlateIM::Button(TEXT("Grant Ability")))
	{
		if (const TSubclassOf<UGameplayAbility> AbilityClass = AbilityPicker.LoadSelectedClass())
		{
			FGameplayAbilitySpec AbilitySpec(AbilityClass, AbilityLevel);
			if (InputTags.IsValidIndex(InputTagIndex) && InputTags[InputTagIndex].IsValid())
			{
				AbilitySpec.GetDynamicSpecSourceTags().AddTag(InputTags[InputTagIndex]);
			}

			InASC->GiveAbility(AbilitySpec);
			InContext.Notify(FString::Printf(TEXT("Granted [%s] to [%s]."), *ObsidianDebugUI::GetCleanClassName(AbilityClass.Get()),
				*GetNameSafe(InASC->GetAvatarActor())));
		}
		else
		{
			InContext.Notify(TEXT("Could not grant the ability, chosen class is invalid."));
		}
	}

	// Abilities cannot be changed while iterating them, so the action is performed after the table is drawn.
	FGameplayAbilitySpecHandle AbilityHandle;
	EAbilityAction AbilityAction = EAbilityAction::None;

	BeginSizedTable();
	SlateIM::BeginTableHeader();
	SlateIM::InitialTableColumnWidth(300.0f);
	SlateIM::AddTableColumn(TEXT("Ability"), TEXT("Granted Ability"));
	SlateIM::InitialTableColumnWidth(60.0f);
	SlateIM::AddTableColumn(TEXT("Level"), TEXT("Level"));
	SlateIM::InitialTableColumnWidth(200.0f);
	SlateIM::AddTableColumn(TEXT("Tags"), TEXT("Dynamic Tags"));
	SlateIM::InitialTableColumnWidth(70.0f);
	SlateIM::AddTableColumn(TEXT("State"), TEXT("State"));
	SlateIM::AddTableColumn(TEXT("Actions"), TEXT("Actions"));
	SlateIM::EndTableHeader();
	SlateIM::BeginTableBody();
	for (const FGameplayAbilitySpec& AbilitySpec : InASC->GetActivatableAbilities())
	{
		const bool bActive = AbilitySpec.IsActive();

		TableTextCell(ObsidianDebugUI::GetCleanClassName(AbilitySpec.Ability));
		TableTextCell(FString::FromInt(AbilitySpec.Level));
		TableTextCell(AbilitySpec.GetDynamicSpecSourceTags().ToStringSimple());
		TableTextCell(bActive ? TEXT("Active") : TEXT("-"), bActive ? FSlateColor(FStyleColors::AccentGreen) : FSlateColor::UseForeground());

		if (SlateIM::NextTableCell())
		{
			SlateIM::BeginHorizontalStack();
			if (SlateIM::Button(TEXT("Activate")))
			{
				AbilityHandle = AbilitySpec.Handle;
				AbilityAction = EAbilityAction::Activate;
			}
			if (SlateIM::Button(TEXT("Cancel"), {.bEnabled = bActive}))
			{
				AbilityHandle = AbilitySpec.Handle;
				AbilityAction = EAbilityAction::Cancel;
			}
			if (SlateIM::Button(TEXT("Remove")))
			{
				AbilityHandle = AbilitySpec.Handle;
				AbilityAction = EAbilityAction::Remove;
			}
			SlateIM::EndHorizontalStack();
		}
	}
	SlateIM::EndTableBody();
	SlateIM::EndTable();

	switch (AbilityAction)
	{
		case EAbilityAction::Activate:
			{
				const bool bActivated = InASC->TryActivateAbility(AbilityHandle);
				InContext.Notify(bActivated ? TEXT("Activated the ability.") : TEXT("Could not activate the ability, see the Ability System log."));
			} break;
		case EAbilityAction::Cancel:
			{
				InASC->CancelAbilityHandle(AbilityHandle);
				InContext.Notify(TEXT("Canceled the ability."));
			} break;
		case EAbilityAction::Remove:
			{
				InASC->ClearAbility(AbilityHandle);
				InContext.Notify(TEXT("Removed the ability."));
			} break;
		default:
			{} break;
	}
}

void FObsidianDebugTab_GAS::DrawEffects(const FObsidianDebugMenuContext& InContext, UAbilitySystemComponent* InASC)
{
	using namespace ObsidianDebugGASTab;

	ObsidianDebugUI::Section(TEXT("Effects"));

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Effect"));
		EffectPicker.Draw(UGameplayEffect::StaticClass());
	}
	SlateIM::EndHorizontalStack();

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Level"));
		SlateIM::MinWidth(120.0f);
		SlateIM::SpinBox(EffectLevel, {.Min = 0.0f});
	}
	SlateIM::EndHorizontalStack();

	if (SlateIM::Button(TEXT("Apply Effect")))
	{
		if (const TSubclassOf<UGameplayEffect> EffectClass = EffectPicker.LoadSelectedClass())
		{
			InASC->ApplyGameplayEffectToSelf(EffectClass.GetDefaultObject(), EffectLevel, InASC->MakeEffectContext());
			InContext.Notify(FString::Printf(TEXT("Applied [%s] to [%s]."), *ObsidianDebugUI::GetCleanClassName(EffectClass.Get()),
				*GetNameSafe(InASC->GetAvatarActor())));
		}
		else
		{
			InContext.Notify(TEXT("Could not apply the effect, chosen class is invalid."));
		}
	}

	FActiveGameplayEffectHandle EffectToRemove;
	const float WorldTime = static_cast<float>(InContext.World->GetTimeSeconds());

	BeginSizedTable();
	SlateIM::BeginTableHeader();
	SlateIM::InitialTableColumnWidth(340.0f);
	SlateIM::AddTableColumn(TEXT("Effect"), TEXT("Active Effect"));
	SlateIM::InitialTableColumnWidth(60.0f);
	SlateIM::AddTableColumn(TEXT("Level"), TEXT("Level"));
	SlateIM::InitialTableColumnWidth(60.0f);
	SlateIM::AddTableColumn(TEXT("Stacks"), TEXT("Stacks"));
	SlateIM::InitialTableColumnWidth(140.0f);
	SlateIM::AddTableColumn(TEXT("TimeLeft"), TEXT("Time Left"));
	SlateIM::AddTableColumn(TEXT("Actions"), TEXT("Actions"));
	SlateIM::EndTableHeader();
	SlateIM::BeginTableBody();
	for (const FActiveGameplayEffectHandle& EffectHandle : InASC->GetActiveEffects(FGameplayEffectQuery()))
	{
		const FActiveGameplayEffect* ActiveEffect = InASC->GetActiveGameplayEffect(EffectHandle);
		if (ActiveEffect == nullptr)
		{
			continue;
		}

		const float Duration = ActiveEffect->GetDuration();
		const FString TimeLeft = Duration < 0.0f ? FString(TEXT("Infinite")) :
			FString::Printf(TEXT("%.1f / %.1f s"), ActiveEffect->GetTimeRemaining(WorldTime), Duration);

		TableTextCell(ObsidianDebugUI::GetCleanClassName(ActiveEffect->Spec.Def));
		TableTextCell(FString::SanitizeFloat(ActiveEffect->Spec.GetLevel()));
		TableTextCell(FString::FromInt(InASC->GetCurrentStackCount(EffectHandle)));
		TableTextCell(TimeLeft);

		if (SlateIM::NextTableCell())
		{
			if (SlateIM::Button(TEXT("Remove")))
			{
				EffectToRemove = EffectHandle;
			}
		}
	}
	SlateIM::EndTableBody();
	SlateIM::EndTable();

	if (EffectToRemove.IsValid())
	{
		InASC->RemoveActiveGameplayEffect(EffectToRemove);
		InContext.Notify(TEXT("Removed the effect."));
	}
}

void FObsidianDebugTab_GAS::DrawTags(const FObsidianDebugMenuContext& InContext, UAbilitySystemComponent* InASC)
{
	ObsidianDebugUI::Section(TEXT("Tags"));

	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("Loose Tag"));
		SlateIM::MinWidth(320.0f);
		SlateIM::EditableText(TagInput, TEXT("Full name of the Gameplay Tag..."));

		const bool bAddTag = SlateIM::Button(TEXT("Add"));
		const bool bRemoveTag = SlateIM::Button(TEXT("Remove"));
		if (bAddTag || bRemoveTag)
		{
			const FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName(*TagInput.TrimStartAndEnd()), false);
			if (Tag.IsValid() == false)
			{
				InContext.Notify(FString::Printf(TEXT("[%s] is not a registered Gameplay Tag."), *TagInput));
			}
			else if (bAddTag)
			{
				InASC->AddLooseGameplayTag(Tag);
				InContext.Notify(FString::Printf(TEXT("Added loose tag [%s]."), *Tag.ToString()));
			}
			else
			{
				InASC->RemoveLooseGameplayTag(Tag);
				InContext.Notify(FString::Printf(TEXT("Removed loose tag [%s]."), *Tag.ToString()));
			}
		}
	}
	SlateIM::EndHorizontalStack();

	FGameplayTagContainer OwnedTags;
	InASC->GetOwnedGameplayTags(OwnedTags);

	if (OwnedTags.IsEmpty())
	{
		SlateIM::Text(TEXT("Target owns no Gameplay Tags."), {.Color = FStyleColors::AccentGray});
	}

	for (const FGameplayTag& OwnedTag : OwnedTags)
	{
		SlateIM::Text(FString::Printf(TEXT("%s (x%d)"), *OwnedTag.ToString(), InASC->GetGameplayTagCount(OwnedTag)));
	}
}

void FObsidianDebugTab_GAS::GatherInputTags()
{
	InputTags.Reset();
	InputTagNames.Reset();

	// First option leaves the ability without the input.
	InputTags.Add(FGameplayTag::EmptyTag);
	InputTagNames.Add(TEXT("None"));

	const FGameplayTag InputRootTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Input.Ability")), false);
	if (InputRootTag.IsValid() == false)
	{
		return;
	}

	const FGameplayTagContainer AbilityInputTags = UGameplayTagsManager::Get().RequestGameplayTagChildren(InputRootTag);
	for (const FGameplayTag& InputTag : AbilityInputTags)
	{
		InputTags.Add(InputTag);
		InputTagNames.Add(InputTag.ToString());
	}
}

#endif // WITH_OBSIDIAN_DEBUG_MENU
