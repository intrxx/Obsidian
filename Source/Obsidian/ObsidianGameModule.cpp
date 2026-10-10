// Copyright 2026 out of sCope team - intrxx

#include "ObsidianGamemodule.h"

// ~ Core
#include "Modules/ModuleManager.h"
#if WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebugger.h"

// ~ Project
#include "InventoryItems/Debugging/GameplayDebuggerCategory_InventoryItems.h"
#include "InventoryItems/Debugging/GameplayDebuggerCategory_Equipment.h"
#include "InventoryItems/Debugging/GameplayDebuggerCategory_PlayerStash.h"
#endif
#if WITH_OBSIDIAN_DEBUG_MENU
#include "Debug/ObsidianDebugMenu.h"
#endif

DEFINE_LOG_CATEGORY(LogObsidian);

IMPLEMENT_PRIMARY_GAME_MODULE(FObsidianGameModule, Obsidian, "Obsidian");

#if WITH_OBSIDIAN_DEBUG_MENU
namespace ObsidianGameModule
{
	/** Registers the obsidian.ToggleDebugMenu console command for as long as it is alive. */
	static TUniquePtr<FObsidianDebugMenu> DebugMenu;
}
#endif

void FObsidianGameModule::StartupModule()
{
#if WITH_OBSIDIAN_DEBUG_MENU
	if (IsRunningDedicatedServer() == false && IsRunningCommandlet() == false)
	{
		ObsidianGameModule::DebugMenu = MakeUnique<FObsidianDebugMenu>();
	}
#endif

#if WITH_GAMEPLAY_DEBUGGER
	IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
	
	GameplayDebuggerModule.RegisterCategory("Inventory", IGameplayDebugger::FOnGetCategory::CreateStatic(&FGameplayDebuggerCategory_InventoryItems::MakeInstance));
	GameplayDebuggerModule.NotifyCategoriesChanged();
	GameplayDebuggerModule.RegisterCategory("Equipment", IGameplayDebugger::FOnGetCategory::CreateStatic(&FGameplayDebuggerCategory_Equipment::MakeInstance));
	GameplayDebuggerModule.NotifyCategoriesChanged();
	GameplayDebuggerModule.RegisterCategory("PlayerStash", IGameplayDebugger::FOnGetCategory::CreateStatic(&FGameplayDebuggerCategory_PlayerStash::MakeInstance));
	GameplayDebuggerModule.NotifyCategoriesChanged();
#endif
}

void FObsidianGameModule::ShutdownModule()
{
#if WITH_OBSIDIAN_DEBUG_MENU
	ObsidianGameModule::DebugMenu.Reset();
#endif

#if WITH_GAMEPLAY_DEBUGGER
	if(IGameplayDebugger::IsAvailable())
	{
		IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
		GameplayDebuggerModule.UnregisterCategory("Inventory");
		GameplayDebuggerModule.NotifyCategoriesChanged();
		GameplayDebuggerModule.UnregisterCategory("Equipment");
		GameplayDebuggerModule.NotifyCategoriesChanged();
		GameplayDebuggerModule.UnregisterCategory("PlayerStash");
		GameplayDebuggerModule.NotifyCategoriesChanged();
	}
#endif
}
