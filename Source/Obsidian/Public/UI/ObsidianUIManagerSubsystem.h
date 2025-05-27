// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "GameUIManagerSubsystem.h"
#include "ObsidianUIManagerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianUIManagerSubsystem : public UGameUIManagerSubsystem
{
	GENERATED_BODY()

public:
	UObsidianUIManagerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
	bool Tick(float DeltaTime);
	void SyncRootLayoutVisibilityToShowHUD();

	FTSTicker::FDelegateHandle TickHandle;
};
