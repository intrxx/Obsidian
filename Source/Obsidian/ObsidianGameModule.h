// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FObsidianGameModule : public FDefaultGameModuleImpl
{
public:
	//~ Begin IModuleInterface Interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface Interface
};