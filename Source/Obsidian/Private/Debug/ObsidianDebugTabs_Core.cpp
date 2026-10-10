// Copyright 2026 out of sCope team - intrxx

#include "Debug/ObsidianDebugMenuTabs.h"

#if WITH_OBSIDIAN_DEBUG_MENU

#include "AbilitySystemComponent.h"
#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/WorldSettings.h"
#include "HAL/IConsoleManager.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/App.h"
#include "SlateIM.h"

#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Characters/Player/ObsidianPlayerState.h"


namespace ObsidianDebugCore
{
	struct FNamedCommand
	{
		const TCHAR* Name;
		const TCHAR* Command;
	};

	const FNamedCommand StatCommands[] =
	{
		{TEXT("FPS"), TEXT("stat fps")},
		{TEXT("Unit"), TEXT("stat unit")},
		{TEXT("Unit Graph"), TEXT("stat unitgraph")},
		{TEXT("Game"), TEXT("stat game")},
		{TEXT("GPU"), TEXT("stat gpu")},
		{TEXT("Scene Rendering"), TEXT("stat scenerendering")},
		{TEXT("AI"), TEXT("stat ai")},
		{TEXT("Hide All"), TEXT("stat none")}
	};

	const FNamedCommand ViewModeCommands[] =
	{
		{TEXT("Lit"), TEXT("viewmode lit")},
		{TEXT("Unlit"), TEXT("viewmode unlit")},
		{TEXT("Wireframe"), TEXT("viewmode wireframe")},
		{TEXT("Detail Lighting"), TEXT("viewmode lit_detaillighting")},
		{TEXT("Lighting Only"), TEXT("viewmode lightingonly")},
		{TEXT("Shader Complexity"), TEXT("viewmode shadercomplexity")}
	};

	const FNamedCommand ShowFlagCommands[] =
	{
		{TEXT("Collision"), TEXT("show Collision")},
		{TEXT("Navigation"), TEXT("show Navigation")},
		{TEXT("Bounds"), TEXT("show Bounds")},
		{TEXT("Post Processing"), TEXT("show PostProcessing")},
		{TEXT("Fog"), TEXT("show Fog")},
		{TEXT("Particles"), TEXT("show Particles")},
		{TEXT("Dynamic Shadows"), TEXT("show DynamicShadows")},
		{TEXT("Skeletal Meshes"), TEXT("show SkeletalMeshes")},
		{TEXT("Static Meshes"), TEXT("show StaticMeshes")}
	};

	const FNamedCommand ConsolePresets[] =
	{
		{TEXT("Toggle Gameplay Debugger"), TEXT("EnableGDT")},
		{TEXT("Show Debug Ability System"), TEXT("showdebug abilitysystem")},
		{TEXT("Ability System Next Category"), TEXT("AbilitySystem.Debug.NextCategory")},
		{TEXT("Show Debug AI"), TEXT("showdebug ai")},
		{TEXT("Hide Show Debug"), TEXT("showdebug reset")},
		{TEXT("Toggle Debug Camera"), TEXT("ToggleDebugCamera")},
		{TEXT("Dump Ticks"), TEXT("dumpticks")},
		{TEXT("Memory Report"), TEXT("memreport")}
	};

	constexpr int32 MaxCommandHistory = 12;

	void InfoRow(const FStringView& Label, const FStringView& Value)
	{
		SlateIM::HAlign(HAlign_Fill);
		SlateIM::BeginHorizontalStack();
		ObsidianDebugUI::Label(Label);

		// Value takes the rest of the row, otherwise the text that changes every frame wraps based on its previous size.
		SlateIM::Fill();
		SlateIM::HAlign(HAlign_Fill);
		SlateIM::VAlign(VAlign_Center);
		SlateIM::Text(Value);
		SlateIM::EndHorizontalStack();
	}

	template<int32 Count>
	void CommandButtons(const FObsidianDebugMenuContext& Context, const FNamedCommand (&Commands)[Count])
	{
		SlateIM::HAlign(HAlign_Fill);
		SlateIM::BeginHorizontalWrap();
		for (const FNamedCommand& NamedCommand : Commands)
		{
			SlateIM::SetToolTip(NamedCommand.Command);
			if (SlateIM::Button(NamedCommand.Name))
			{
				Context.ExecConsoleCommand(NamedCommand.Command);
			}
		}
		SlateIM::EndHorizontalWrap();
	}

	FString GetAttributeString(const UAbilitySystemComponent* ASC, const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute)
	{
		float Value = 0.0f;
		float MaxValue = 0.0f;
		if (ObsidianDebugGAS::GetAttributeValue(ASC, Attribute, Value) && ObsidianDebugGAS::GetAttributeValue(ASC, MaxAttribute, MaxValue))
		{
			return FString::Printf(TEXT("%.0f / %.0f"), Value, MaxValue);
		}
		return TEXT("-");
	}
}

// ~ FObsidianDebugTab_Game

void FObsidianDebugTab_Game::Draw(const FObsidianDebugMenuContext& Context)
{
	using namespace ObsidianDebugCore;

	UWorld* World = Context.World;

	ObsidianDebugUI::Section(TEXT("World"));
	{
		const float FrameTime = static_cast<float>(FApp::GetDeltaTime());
		SmoothedFrameTime = SmoothedFrameTime <= 0.0f ? FrameTime : FMath::Lerp(SmoothedFrameTime, FrameTime, 0.05f);

		InfoRow(TEXT("Map"), UWorld::RemovePIEPrefix(World->GetMapName()));
		InfoRow(TEXT("Time"), FString::Printf(TEXT("%.1f s"), World->GetTimeSeconds()));
		InfoRow(TEXT("Players"), FString::FromInt(World->GetNumPlayerControllers()));
		InfoRow(TEXT("Frame"), FString::Printf(TEXT("%.2f ms (%.0f FPS)"), SmoothedFrameTime * 1000.0f,
			SmoothedFrameTime > 0.0f ? 1.0f / SmoothedFrameTime : 0.0f));
	}

	ObsidianDebugUI::Section(TEXT("Time"));
	if (AWorldSettings* WorldSettings = World->GetWorldSettings())
	{
		SlateIM::BeginHorizontalStack();
		{
			ObsidianDebugUI::Label(TEXT("Time Dilation"));

			float TimeDilation = WorldSettings->TimeDilation;
			SlateIM::MinWidth(260.0f);
			SlateIM::VAlign(VAlign_Center);
			if (SlateIM::Slider(TimeDilation, {.Min = 0.05f, .Max = 4.0f, .Step = 0.05f}))
			{
				WorldSettings->SetTimeDilation(TimeDilation);
			}

			SlateIM::Padding(FMargin(8.0f, 0.0f));
			SlateIM::VAlign(VAlign_Center);
			SlateIM::Text(FString::Printf(TEXT("%.2fx"), WorldSettings->TimeDilation));
		}
		SlateIM::EndHorizontalStack();

		SlateIM::BeginHorizontalStack();
		{
			ObsidianDebugUI::Label(TEXT("Presets"));
			for (const float Preset : {0.1f, 0.25f, 0.5f, 1.0f, 2.0f, 4.0f})
			{
				if (SlateIM::Button(FString::Printf(TEXT("%.2gx"), Preset)))
				{
					WorldSettings->SetTimeDilation(Preset);
				}
			}
		}
		SlateIM::EndHorizontalStack();

		bool bPaused = World->IsPaused();
		if (SlateIM::CheckBox(bPaused, {.Label = TEXT("Pause Game")}))
		{
			UGameplayStatics::SetGamePaused(World, bPaused);
		}
	}

	ObsidianDebugUI::Section(TEXT("Level"));
	{
		if (bGatheredMaps == false)
		{
			GatherMaps();
		}

		SlateIM::BeginHorizontalStack();
		{
			ObsidianDebugUI::Label(TEXT("Open Level"));
			SlateIM::MinWidth(320.0f);
			MapComboBox.Draw(MapNames, SelectedMapIndex, true);

			if (SlateIM::Button(TEXT("Open")) && MapPackageNames.IsValidIndex(SelectedMapIndex))
			{
				Context.Notify(FString::Printf(TEXT("Opening level [%s]."), *MapPackageNames[SelectedMapIndex]));
				UGameplayStatics::OpenLevel(World, FName(*MapPackageNames[SelectedMapIndex]));
			}
			if (SlateIM::Button(TEXT("Refresh")))
			{
				GatherMaps();
			}
		}
		SlateIM::EndHorizontalStack();

		if (SlateIM::Button(TEXT("Restart Current Level")))
		{
			UGameplayStatics::OpenLevel(World, FName(*UGameplayStatics::GetCurrentLevelName(World)));
		}
	}

	ObsidianDebugUI::Section(TEXT("Memory"));
	if (SlateIM::Button(TEXT("Collect Garbage")) && GEngine)
	{
		GEngine->ForceGarbageCollection(true);
		Context.Notify(TEXT("Requested full garbage collection."));
	}
}

void FObsidianDebugTab_Game::GatherMaps()
{
	MapPackageNames.Reset();
	MapNames.Reset();
	bGatheredMaps = true;

	TArray<FAssetData> MapAssets;
	IAssetRegistry::GetChecked().GetAssetsByClass(UWorld::StaticClass()->GetClassPathName(), MapAssets);

	for (const FAssetData& MapAsset : MapAssets)
	{
		const FString PackageName = MapAsset.PackageName.ToString();
		if (PackageName.StartsWith(TEXT("/Game/")))
		{
			MapPackageNames.Add(PackageName);
		}
	}

	MapPackageNames.Sort([](const FString& A, const FString& B)
		{
			return FPaths::GetBaseFilename(A) < FPaths::GetBaseFilename(B);
		});

	for (const FString& PackageName : MapPackageNames)
	{
		MapNames.Add(FPaths::GetBaseFilename(PackageName));
	}
}

// ~ FObsidianDebugTab_Player

void FObsidianDebugTab_Player::Tick(const FObsidianDebugMenuContext& Context, const float DeltaTime)
{
	if (bKeepResourcesFull && Context.HasAuthority())
	{
		RestoreResources(Context);
	}
}

void FObsidianDebugTab_Player::Draw(const FObsidianDebugMenuContext& Context)
{
	using namespace ObsidianDebugCore;

	APawn* Pawn = Context.Pawn;
	UAbilitySystemComponent* ASC = Context.GetPlayerASC();
	const AObsidianPlayerController* ObsidianPC = Context.GetObsidianPC();
	if (Pawn == nullptr || ASC == nullptr)
	{
		ObsidianDebugUI::WarningText(TEXT("Chosen Player has no Pawn with an Ability System Component."));
		return;
	}

	ObsidianDebugUI::Section(TEXT("Info"));
	{
		const AObsidianPlayerState* ObsidianPS = ObsidianPC ? ObsidianPC->GetObsidianPlayerState() : nullptr;

		InfoRow(TEXT("Pawn"), Pawn->GetName());
		InfoRow(TEXT("Location"), Pawn->GetActorLocation().ToCompactString());
		InfoRow(TEXT("Hero Level"), ObsidianPS ? FString::FromInt(ObsidianPS->GetHeroLevel()) : FString(TEXT("-")));
		InfoRow(TEXT("Health"), GetAttributeString(ASC, UObsidianCommonAttributeSet::GetHealthAttribute(),
			UObsidianCommonAttributeSet::GetMaxHealthAttribute()));
		InfoRow(TEXT("Energy Shield"), GetAttributeString(ASC, UObsidianCommonAttributeSet::GetEnergyShieldAttribute(),
			UObsidianCommonAttributeSet::GetMaxEnergyShieldAttribute()));
		InfoRow(TEXT("Mana"), GetAttributeString(ASC, UObsidianHeroAttributeSet::GetManaAttribute(),
			UObsidianHeroAttributeSet::GetMaxManaAttribute()));
		InfoRow(TEXT("Stamina"), GetAttributeString(ASC, UObsidianHeroAttributeSet::GetStaminaAttribute(),
			UObsidianHeroAttributeSet::GetMaxStaminaAttribute()));
		InfoRow(TEXT("Experience"), GetAttributeString(ASC, UObsidianHeroAttributeSet::GetExperienceAttribute(),
			UObsidianHeroAttributeSet::GetMaxExperienceAttribute()));
	}

	ObsidianDebugUI::Section(TEXT("Resources"));
	{
		SlateIM::BeginHorizontalStack();
		{
			SlateIM::SetToolTip(TEXT("Sets Health, Energy Shield, Mana and Stamina to their max values."));
			if (SlateIM::Button(TEXT("Restore Resources")))
			{
				RestoreResources(Context);
				Context.Notify(TEXT("Restored the resources of the Player."));
			}

			SlateIM::SetToolTip(TEXT("Deals lethal damage to the Player, running the regular death flow."));
			if (SlateIM::Button(TEXT("Kill")))
			{
				const bool bKilled = ObsidianDebugGAS::Kill(ASC);
				Context.Notify(bKilled ? TEXT("Killed the Player.") : TEXT("Could not kill the Player, it has no Common Attribute Set."));
			}
		}
		SlateIM::EndHorizontalStack();

		SlateIM::SetToolTip(TEXT("Restores the resources every frame, works only while the Debug Menu is opened."));
		SlateIM::CheckBox(bKeepResourcesFull, {.Label = TEXT("Keep Resources Full")});
	}

	ObsidianDebugUI::Section(TEXT("Progression"));
	{
		SlateIM::BeginHorizontalStack();
		{
			ObsidianDebugUI::Label(TEXT("Experience"));
			SlateIM::MinWidth(120.0f);
			SlateIM::SpinBox(ExperienceToAdd, {.Min = 0.0f});

			if (SlateIM::Button(TEXT("Add")))
			{
				ObsidianDebugGAS::ApplyInstantAttributeMod(ASC, ASC, UObsidianHeroAttributeSet::GetExperienceAttribute(),
					EGameplayModOp::Additive, ExperienceToAdd);
				Context.Notify(FString::Printf(TEXT("Added [%.0f] Experience to the Player."), ExperienceToAdd));
			}

			SlateIM::SetToolTip(TEXT("Adds just enough Experience for the Player to level up."));
			if (SlateIM::Button(TEXT("Level Up")))
			{
				float Experience = 0.0f;
				float MaxExperience = 0.0f;
				if (ObsidianDebugGAS::GetAttributeValue(ASC, UObsidianHeroAttributeSet::GetExperienceAttribute(), Experience)
					&& ObsidianDebugGAS::GetAttributeValue(ASC, UObsidianHeroAttributeSet::GetMaxExperienceAttribute(), MaxExperience))
				{
					ObsidianDebugGAS::ApplyInstantAttributeMod(ASC, ASC, UObsidianHeroAttributeSet::GetExperienceAttribute(),
						EGameplayModOp::Additive, FMath::Max(MaxExperience - Experience, 0.0f) + 1.0f);
					Context.Notify(TEXT("Leveled up the Player."));
				}
			}
		}
		SlateIM::EndHorizontalStack();
	}

	ObsidianDebugUI::Section(TEXT("Teleport"));
	{
		SlateIM::BeginHorizontalStack();
		{
			ObsidianDebugUI::Label(TEXT("Location"));
			SlateIM::MinWidth(100.0f);
			SlateIM::SpinBox(TeleportLocation.X);
			SlateIM::MinWidth(100.0f);
			SlateIM::SpinBox(TeleportLocation.Y);
			SlateIM::MinWidth(100.0f);
			SlateIM::SpinBox(TeleportLocation.Z);

			if (SlateIM::Button(TEXT("Teleport")))
			{
				Pawn->TeleportTo(TeleportLocation, Pawn->GetActorRotation());
			}
			if (SlateIM::Button(TEXT("Use Current")))
			{
				TeleportLocation = Pawn->GetActorLocation();
			}
		}
		SlateIM::EndHorizontalStack();

		SlateIM::BeginHorizontalStack();
		{
			ObsidianDebugUI::Label(TEXT("Saved Location"));
			if (SlateIM::Button(TEXT("Save")))
			{
				SavedLocation = Pawn->GetActorLocation();
				bHasSavedLocation = true;
			}
			if (SlateIM::Button(TEXT("Return"), {.bEnabled = bHasSavedLocation}))
			{
				Pawn->TeleportTo(SavedLocation, Pawn->GetActorRotation());
			}
			if (bHasSavedLocation)
			{
				SlateIM::Padding(FMargin(8.0f, 0.0f));
				SlateIM::Fill();
				SlateIM::HAlign(HAlign_Fill);
				SlateIM::VAlign(VAlign_Center);
				SlateIM::Text(SavedLocation.ToCompactString());
			}
		}
		SlateIM::EndHorizontalStack();
	}

	// Stash is opened through the local UI so it makes no sense for remote Players.
	if (ObsidianPC && ObsidianPC->IsLocalController())
	{
		ObsidianDebugUI::Section(TEXT("Player Stash"));
		SlateIM::BeginHorizontalStack();
		if (SlateIM::Button(TEXT("Open Stash")))
		{
			ObsidianPC->TogglePlayerStash(true);
		}
		if (SlateIM::Button(TEXT("Close Stash")))
		{
			ObsidianPC->TogglePlayerStash(false);
		}
		SlateIM::EndHorizontalStack();
	}
}

void FObsidianDebugTab_Player::RestoreResources(const FObsidianDebugMenuContext& Context)
{
	UAbilitySystemComponent* ASC = Context.GetPlayerASC();
	if (ASC == nullptr)
	{
		return;
	}

	const TPair<FGameplayAttribute, FGameplayAttribute> ResourcesWithMax[] =
	{
		{UObsidianCommonAttributeSet::GetHealthAttribute(), UObsidianCommonAttributeSet::GetMaxHealthAttribute()},
		{UObsidianCommonAttributeSet::GetEnergyShieldAttribute(), UObsidianCommonAttributeSet::GetMaxEnergyShieldAttribute()},
		{UObsidianHeroAttributeSet::GetManaAttribute(), UObsidianHeroAttributeSet::GetMaxManaAttribute()},
		{UObsidianHeroAttributeSet::GetStaminaAttribute(), UObsidianHeroAttributeSet::GetMaxStaminaAttribute()}
	};

	for (const TPair<FGameplayAttribute, FGameplayAttribute>& ResourceWithMax : ResourcesWithMax)
	{
		float Value = 0.0f;
		float MaxValue = 0.0f;
		if (ObsidianDebugGAS::GetAttributeValue(ASC, ResourceWithMax.Key, Value)
			&& ObsidianDebugGAS::GetAttributeValue(ASC, ResourceWithMax.Value, MaxValue) && Value < MaxValue)
		{
			ASC->SetNumericAttributeBase(ResourceWithMax.Key, MaxValue);
		}
	}
}

// ~ FObsidianDebugTab_Rendering

void FObsidianDebugTab_Rendering::Draw(const FObsidianDebugMenuContext& Context)
{
	using namespace ObsidianDebugCore;

	ObsidianDebugUI::Section(TEXT("Stats"));
	CommandButtons(Context, StatCommands);

	ObsidianDebugUI::Section(TEXT("View Mode"));
	CommandButtons(Context, ViewModeCommands);

	ObsidianDebugUI::Section(TEXT("Show Flags (toggles)"));
	CommandButtons(Context, ShowFlagCommands);

	ObsidianDebugUI::Section(TEXT("Settings"));
	{
		if (IConsoleVariable* ScreenPercentageCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage")))
		{
			SlateIM::BeginHorizontalStack();
			ObsidianDebugUI::Label(TEXT("Screen Percentage"));

			// Non-positive value means that the default of the project is used.
			float ScreenPercentage = ScreenPercentageCVar->GetFloat() > 0.0f ? ScreenPercentageCVar->GetFloat() : 100.0f;
			SlateIM::MinWidth(260.0f);
			SlateIM::VAlign(VAlign_Center);
			if (SlateIM::Slider(ScreenPercentage, {.Min = 25.0f, .Max = 200.0f, .Step = 5.0f}))
			{
				ScreenPercentageCVar->Set(ScreenPercentage, ECVF_SetByConsole);
			}

			SlateIM::Padding(FMargin(8.0f, 0.0f));
			SlateIM::VAlign(VAlign_Center);
			SlateIM::Text(FString::Printf(TEXT("%.0f%%"), ScreenPercentage));
			SlateIM::EndHorizontalStack();
		}

		if (IConsoleVariable* MaxFPSCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("t.MaxFPS")))
		{
			SlateIM::BeginHorizontalStack();
			ObsidianDebugUI::Label(TEXT("Max FPS (0 = unlimited)"));

			float MaxFPS = MaxFPSCVar->GetFloat();
			SlateIM::MinWidth(120.0f);
			if (SlateIM::SpinBox(MaxFPS, {.Min = 0.0f, .Max = 1000.0f}))
			{
				MaxFPSCVar->Set(MaxFPS, ECVF_SetByConsole);
			}
			SlateIM::EndHorizontalStack();
		}

		if (IConsoleVariable* VSyncCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.VSync")))
		{
			bool bVSync = VSyncCVar->GetInt() != 0;
			if (SlateIM::CheckBox(bVSync, {.Label = TEXT("VSync")}))
			{
				VSyncCVar->Set(bVSync ? 1 : 0, ECVF_SetByConsole);
			}
		}
	}
}

// ~ FObsidianDebugTab_Console

void FObsidianDebugTab_Console::Draw(const FObsidianDebugMenuContext& Context)
{
	using namespace ObsidianDebugCore;

	ObsidianDebugUI::Section(TEXT("Command"));
	{
		SlateIM::HAlign(HAlign_Fill);
		SlateIM::BeginHorizontalStack();
		{
			SlateIM::Fill();
			SlateIM::HAlign(HAlign_Fill);
			SlateIM::EditableText(CommandInput, {.HintText = TEXT("Console command to run...")});

			if (SlateIM::Button(TEXT("Run"), {.bEnabled = CommandInput.IsEmpty() == false}))
			{
				RunCommand(Context, CommandInput);
			}
		}
		SlateIM::EndHorizontalStack();
	}

	ObsidianDebugUI::Section(TEXT("Presets"));
	CommandButtons(Context, ConsolePresets);

	ObsidianDebugUI::Section(TEXT("History"));
	{
		if (CommandHistory.IsEmpty())
		{
			SlateIM::Text(TEXT("No commands were run yet."), {.Color = FStyleColors::AccentGray});
		}

		// Running the command reorders the history, so it cannot be done while iterating it.
		FString CommandToRun;
		for (const FString& Command : CommandHistory)
		{
			SlateIM::HAlign(HAlign_Left);
			if (SlateIM::Button(Command))
			{
				CommandToRun = Command;
			}
		}

		if (CommandToRun.IsEmpty() == false)
		{
			RunCommand(Context, CommandToRun);
		}
	}
}

void FObsidianDebugTab_Console::RunCommand(const FObsidianDebugMenuContext& Context, const FString& Command)
{
	// Command might be a reference to the history entry that is about to be moved.
	const FString CommandCopy = Command;

	Context.ExecConsoleCommand(CommandCopy);

	CommandHistory.Remove(CommandCopy);
	CommandHistory.Insert(CommandCopy, 0);
	CommandHistory.SetNum(FMath::Min(CommandHistory.Num(), ObsidianDebugCore::MaxCommandHistory));
}

#endif // WITH_OBSIDIAN_DEBUG_MENU
