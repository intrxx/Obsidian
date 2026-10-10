// Copyright 2026 out of sCope team - intrxx

#pragma once

#if WITH_OBSIDIAN_DEBUG_MENU

#include "CoreMinimal.h"
#include "SlateIMWidgetBase.h"

#include "Debug/ObsidianDebugMenuTypes.h"

class FObsidianDebugMenuTab;

/**
 * SlateIM Debug Menu of Obsidian, toggled with the "obsidian.ToggleDebugMenu" console command. It can also be opened
 * straight on one of the tabs with "obsidian.OpenDebugMenuTab <TabName>".
 *
 * The menu is drawn in its own window and works on the World and Player chosen at the top of it, so in multiplayer PIE
 * it can be pointed at the server World to debug any of the connected Players.
 *
 * To add a new tab, derive from FObsidianDebugMenuTab and register it in the constructor.
 */
class FObsidianDebugMenu : public FSlateIMWindowBase
{
public:
	FObsidianDebugMenu();
	virtual ~FObsidianDebugMenu() override;

protected:
	//~ Start of FSlateIMWindowBase
	virtual void DrawWindow(float InDeltaTime) override;
	//~ End of FSlateIMWindowBase

private:
	template<typename TabType>
	void RegisterTab()
	{
		Tabs.Add(MakeUnique<TabType>());
	}

	/** Opens the menu with given tab active, handler of the "obsidian.OpenDebugMenuTab" console command. */
	void OpenTab(const TArray<FString>& InArgs);

	/** Draws the World and Player pickers and fills the Context with the picked ones. */
	void DrawTargetBar(FObsidianDebugMenuContext& OutContext);
	void DrawTab(FObsidianDebugMenuTab& InTab, const FObsidianDebugMenuContext& InContext) const;

private:
	TArray<TUniquePtr<FObsidianDebugMenuTab>> Tabs;

	FAutoConsoleCommand OpenTabCommand;
	FName PendingTabToActivate = NAME_None;

	FObsidianDebugComboBox WorldComboBox;
	FObsidianDebugComboBox PlayerComboBox;
	TWeakObjectPtr<UWorld> SelectedWorld;
	TWeakObjectPtr<APlayerController> SelectedPlayerController;

	FString StatusMessage;
};

#endif // WITH_OBSIDIAN_DEBUG_MENU
