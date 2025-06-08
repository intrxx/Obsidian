// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogOOSEquipment, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogOOSInventory, Log, All);

class FOOSInventoryItemsModule : public IModuleInterface
{
	
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
