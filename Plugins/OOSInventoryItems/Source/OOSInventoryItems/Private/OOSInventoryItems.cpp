// Copyright Epic Games, Inc. All Rights Reserved.

#include "OOSInventoryItems.h"

#define LOCTEXT_NAMESPACE "FOOSInventoryItemsModule"

DEFINE_LOG_CATEGORY(LogOOSEquipment);
DEFINE_LOG_CATEGORY(LogOOSInventory);

void FOOSInventoryItemsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FOOSInventoryItemsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FOOSInventoryItemsModule, OOSInventoryItems)