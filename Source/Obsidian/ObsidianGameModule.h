// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogObsidian, Log, All);

/**
 * 
 */
class FObsidianGameModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};