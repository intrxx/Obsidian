// Copyright 2026 out of sCope - intrxx

#pragma once

#include <CoreMinimal.h>


#include "Subsystems/WorldSubsystem.h"
#include "ObsidianItemLabelManagerSubsystem.generated.h"

class AObsidianPlayerController;
class UObsidianItemLabelOverlay;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemLabelManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UObsidianItemLabelManagerSubsystem();

	void InitializeItemLabelManager(UObsidianItemLabelOverlay* InItemLabelOverlay);
	
	// ~ Start of USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// ~ End of USubsystem

private:
	UPROPERTY()
	TObjectPtr<UObsidianItemLabelOverlay> ItemLabelOverlay;
};
