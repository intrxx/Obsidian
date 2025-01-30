// Copyright 2024 out of sCope team - Michał Ogiński


#include "ObsidianGamemodule.h"
#include "Modules/ModuleManager.h"

#if WITH_GAMEPLAY_DEBUGGER
#include "GameplayDebugger.h"
#include "InventoryItems/Debugging/GameplayDebuggerCategory_InventoryItems.h"
#endif

DEFINE_LOG_CATEGORY(LogObsidian);

IMPLEMENT_PRIMARY_GAME_MODULE(FObsidianGameModule, Obsidian, "Obsidian");

void FObsidianGameModule::StartupModule()
{
#if WITH_GAMEPLAY_DEBUGGER
	IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
	GameplayDebuggerModule.RegisterCategory("Inventory", IGameplayDebugger::FOnGetCategory::CreateStatic(&FGameplayDebuggerCategory_InventoryItems::MakeInstance));
	GameplayDebuggerModule.NotifyCategoriesChanged();
#endif
}

void FObsidianGameModule::ShutdownModule()
{
#if WITH_GAMEPLAY_DEBUGGER
	if(IGameplayDebugger::IsAvailable())
	{
		IGameplayDebugger& GameplayDebuggerModule = IGameplayDebugger::Get();
		GameplayDebuggerModule.UnregisterCategory("Inventory");
		GameplayDebuggerModule.NotifyCategoriesChanged();
	}
#endif
}
