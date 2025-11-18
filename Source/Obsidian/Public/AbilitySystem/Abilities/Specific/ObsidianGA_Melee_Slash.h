// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include "AbilitySystem/Abilities/ObsidianGameplayAbility_Melee.h"
#include "ObsidianGA_Melee_Slash.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianGA_Melee_Slash : public UObsidianGameplayAbility_Melee
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void FireSlash();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Slash")
	TObjectPtr<UNiagaraSystem> SlashNiagaraSystem;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Slash")
	float SlashWidth = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Slash")
	float SlashHeight = 200.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Slash")
	float SlashTraceCapsuleRadius = 20.0f;

	/** The offset of the slash trace from the Hero Owner. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Slash")
	float SlashStartOffset = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Debug")
	bool bDrawSlashDebug = false;
};
