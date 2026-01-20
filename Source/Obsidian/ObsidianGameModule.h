// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project

DECLARE_LOG_CATEGORY_EXTERN(LogObsidian, Log, All);

/**
 * 
 */
class FObsidianGameModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};