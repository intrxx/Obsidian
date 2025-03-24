// Copyright 2024 out of sCope team - Michał Ogiński


#include "InventoryItems/Debugging/GameplayDebuggerCategory_Equipment.h"

FGameplayDebuggerCategory_Equipment::FGameplayDebuggerCategory_Equipment()
{
}

TSharedRef<FGameplayDebuggerCategory> FGameplayDebuggerCategory_Equipment::MakeInstance()
{
	return MakeShareable(new FGameplayDebuggerCategory_Equipment());
}
