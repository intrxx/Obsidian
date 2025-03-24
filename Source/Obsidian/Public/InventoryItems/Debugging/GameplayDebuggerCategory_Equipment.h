// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"

#if WITH_GAMEPLAY_DEBUGGER_MENU
#include "GameplayDebuggerCategory.h"

/**
 * 
 */
class FGameplayDebuggerCategory_Equipment : public FGameplayDebuggerCategory
{

public:
	OBSIDIAN_API FGameplayDebuggerCategory_Equipment();


	OBSIDIAN_API static TSharedRef<FGameplayDebuggerCategory> MakeInstance();
};

#endif // WITH_GAMEPLAY_DEBUGGER_MENU
