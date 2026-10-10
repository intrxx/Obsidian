// Copyright 2026 out of sCope team - intrxx

#include "Debug/ObsidianDebugMenu.h"

#if WITH_OBSIDIAN_DEBUG_MENU

#include <Engine/Engine.h>
#include <GameFramework/PlayerController.h>
#include <GameFramework/PlayerState.h>
#include <SlateIM.h>

#include "Debug/ObsidianDebugMenuTabs.h"
#include "Obsidian/ObsidianLogCategories.h"

namespace ObsidianDebugMenu
{
	const FVector2f DefaultWindowSize(900.0f, 760.0f);

	FString GetNetModeString(const ENetMode NetMode)
	{
		switch (NetMode)
		{
			case NM_Standalone:
				return TEXT("Standalone");
			case NM_DedicatedServer:
				return TEXT("Dedicated Server");
			case NM_ListenServer:
				return TEXT("Listen Server");
			case NM_Client:
				return TEXT("Client");
			default:
				return TEXT("Unknown");
		}
	}
}

FObsidianDebugMenu::FObsidianDebugMenu()
	: FSlateIMWindowBase(TEXT("Obsidian Debug Menu"), ObsidianDebugMenu::DefaultWindowSize, TEXT("obsidian.ToggleDebugMenu"),
		TEXT("Toggles the Obsidian Debug Menu window."))
	, OpenTabCommand(TEXT("obsidian.OpenDebugMenuTab"),
		TEXT("Opens the Obsidian Debug Menu with given tab active. Usage: obsidian.OpenDebugMenuTab <TabName>, e.g. Items, GAS or AI."),
		FConsoleCommandWithArgsDelegate::CreateRaw(this, &FObsidianDebugMenu::OpenTab))
{
	// Core
	RegisterTab<FObsidianDebugTab_Game>();
	RegisterTab<FObsidianDebugTab_Player>();
	RegisterTab<FObsidianDebugTab_Rendering>();
	RegisterTab<FObsidianDebugTab_Console>();

	// Obsidian
	RegisterTab<FObsidianDebugTab_Items>();
	RegisterTab<FObsidianDebugTab_GAS>();
	RegisterTab<FObsidianDebugTab_AI>();
}

FObsidianDebugMenu::~FObsidianDebugMenu() = default;

void FObsidianDebugMenu::OpenTab(const TArray<FString>& Args)
{
	const FName TabName = Args.IsEmpty() ? NAME_None : FName(*Args[0]);
	const bool bTabExists = Tabs.ContainsByPredicate([TabName](const TUniquePtr<FObsidianDebugMenuTab>& Tab)
		{
			return Tab->GetTabName() == TabName;
		});

	if (bTabExists == false)
	{
		FString TabNames;
		for (const TUniquePtr<FObsidianDebugMenuTab>& Tab : Tabs)
		{
			TabNames += FString::Printf(TEXT(" %s"), *Tab->GetTabName().ToString());
		}
		UE_LOG(ObLogDebugMenu, Warning, TEXT("Usage: obsidian.OpenDebugMenuTab <TabName>. Available tabs:%s"), *TabNames);
		return;
	}

	// Tab can only be activated while the menu is being drawn.
	PendingTabToActivate = TabName;
	EnableWidget();
}

void FObsidianDebugMenu::DrawWindow(float DeltaTime)
{
	FObsidianDebugMenuContext Context;
	Context.StatusMessage = &StatusMessage;

	// Window root lays out its content vertically, the tabs take all the space left by the target and status bars.
	DrawTargetBar(Context);

	if (Context.IsValid())
	{
		for (const TUniquePtr<FObsidianDebugMenuTab>& Tab : Tabs)
		{
			Tab->Tick(Context, DeltaTime);
		}
	}

	SlateIM::Maximize();
	SlateIM::BeginTabGroup(TEXT("ObsidianDebugMenuTabs"));
	SlateIM::BeginTabStack();
	for (const TUniquePtr<FObsidianDebugMenuTab>& Tab : Tabs)
	{
		if (SlateIM::BeginTab(Tab->GetTabName()))
		{
			DrawTab(*Tab, Context);
		}
		SlateIM::EndTab();
	}
	SlateIM::EndTabStack();
	SlateIM::EndTabGroup();

	if (PendingTabToActivate.IsNone() == false)
	{
		SlateIM::ActivateTab(PendingTabToActivate);
		PendingTabToActivate = NAME_None;
	}

	SlateIM::HAlign(HAlign_Fill);
	SlateIM::Padding(FMargin(8.0f, 4.0f));
	SlateIM::Text(StatusMessage.IsEmpty() ? TEXT("Ready.") : *StatusMessage, {.Color = FStyleColors::AccentGray});
}

void FObsidianDebugMenu::DrawTargetBar(FObsidianDebugMenuContext& OutContext)
{
	// Worlds
	TArray<UWorld*> Worlds;
	TArray<FString> WorldNames;
	if (GEngine)
	{
		for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
		{
			UWorld* World = WorldContext.World();
			if (World == nullptr || (WorldContext.WorldType != EWorldType::Game && WorldContext.WorldType != EWorldType::PIE))
			{
				continue;
			}

			FString WorldName = FString::Printf(TEXT("%s [%s]"), *UWorld::RemovePIEPrefix(World->GetMapName()),
				*ObsidianDebugMenu::GetNetModeString(World->GetNetMode()));
			if (WorldContext.WorldType == EWorldType::PIE)
			{
				WorldName += FString::Printf(TEXT(" PIE %d"), WorldContext.PIEInstance);
			}

			Worlds.Add(World);
			WorldNames.Add(MoveTemp(WorldName));
		}
	}

	int32 WorldIndex = Worlds.IndexOfByKey(SelectedWorld.Get());
	if (WorldIndex == INDEX_NONE)
	{
		// Prefer the World with authority as most of the debug actions need it.
		WorldIndex = Worlds.IndexOfByPredicate([](const UWorld* World)
			{
				return World->GetNetMode() != NM_Client;
			});
	}

	SlateIM::Padding(FMargin(8.0f, 6.0f));
	SlateIM::BeginHorizontalStack();
	{
		ObsidianDebugUI::Label(TEXT("World"), 45.0f);
		SlateIM::MinWidth(260.0f);
		WorldComboBox.Draw(WorldNames, WorldIndex);

		UWorld* World = Worlds.IsValidIndex(WorldIndex) ? Worlds[WorldIndex] : nullptr;
		SelectedWorld = World;
		OutContext.World = World;

		// Players
		TArray<APlayerController*> PlayerControllers;
		TArray<FString> PlayerNames;
		if (World)
		{
			for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
			{
				APlayerController* PlayerController = It->Get();
				if (PlayerController == nullptr)
				{
					continue;
				}

				FString PlayerName = PlayerController->PlayerState ? PlayerController->PlayerState->GetPlayerName() : FString();
				if (PlayerName.IsEmpty())
				{
					PlayerName = PlayerController->GetName();
				}
				if (PlayerController->IsLocalController())
				{
					PlayerName += TEXT(" (local)");
				}

				PlayerControllers.Add(PlayerController);
				PlayerNames.Add(MoveTemp(PlayerName));
			}
		}

		int32 PlayerIndex = PlayerControllers.IndexOfByKey(SelectedPlayerController.Get());
		if (PlayerIndex == INDEX_NONE)
		{
			PlayerIndex = PlayerControllers.IndexOfByPredicate([](const APlayerController* PlayerController)
				{
					return PlayerController->IsLocalController();
				});
		}

		SlateIM::Padding(FMargin(16.0f, 0.0f, 0.0f, 0.0f));
		ObsidianDebugUI::Label(TEXT("Player"), 50.0f);
		SlateIM::MinWidth(220.0f);
		PlayerComboBox.Draw(PlayerNames, PlayerIndex);

		APlayerController* PlayerController = PlayerControllers.IsValidIndex(PlayerIndex) ? PlayerControllers[PlayerIndex] : nullptr;
		SelectedPlayerController = PlayerController;
		OutContext.PlayerController = PlayerController;
		OutContext.Pawn = PlayerController ? PlayerController->GetPawn() : nullptr;
	}
	SlateIM::EndHorizontalStack();
}

void FObsidianDebugMenu::DrawTab(FObsidianDebugMenuTab& Tab, const FObsidianDebugMenuContext& Context) const
{
	ObsidianDebugUI::BeginTabContent();
	if (Context.IsValid() == false)
	{
		ObsidianDebugUI::WarningText(TEXT("There is no game World running, start the game or PIE session to use the Debug Menu."));
	}
	else
	{
		const bool bMissingAuthority = Tab.RequiresAuthority() && Context.HasAuthority() == false;
		if (bMissingAuthority)
		{
			ObsidianDebugUI::WarningText(TEXT("This tab changes gameplay state so it needs authority, choose the server World above to use it."));
			SlateIM::BeginDisabledState();
		}

		Tab.Draw(Context);

		if (bMissingAuthority)
		{
			SlateIM::EndDisabledState();
		}
	}
	ObsidianDebugUI::EndTabContent();
}

#endif // WITH_OBSIDIAN_DEBUG_MENU
