// Copyright 2026 out of sCope - intrxx

#pragma once

#include <CoreMinimal.h>


#include "Subsystems/WorldSubsystem.h"
#include "ObsidianItemLabelManagerSubsystem.generated.h"

class UCanvasPanelSlot;
class AObsidianDroppableItem;
class UObsidianItemLabel;
class AObsidianPlayerController;
class UObsidianItemLabelOverlay;

USTRUCT()
struct FObsidianItemLabelInfo
{
	GENERATED_BODY()
	
public:
	bool IsValid() const;
	
public:
	UPROPERTY()
	TObjectPtr<UObsidianItemLabel> ItemLabelWidget;

	UPROPERTY()
	TObjectPtr<AObsidianDroppableItem> OwningItemActor;
	
	uint8 Priority = 8;
};

USTRUCT()
struct FObsidianItemLabelData
{
	GENERATED_BODY()
	
public:
	FObsidianItemLabelData(){}
	FObsidianItemLabelData(const FObsidianItemLabelInfo& ItemLabelInfo);

	bool IsValid() const;
	
public:
	UPROPERTY()
	TObjectPtr<UObsidianItemLabel> ItemLabelWidget;

	UPROPERTY()
	TObjectPtr<UCanvasPanelSlot> CanvasPanelSlot;

	UPROPERTY()
	TObjectPtr<AObsidianDroppableItem> OwningItemActor;
	
	uint8 Priority = 8;
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemLabelManagerSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UObsidianItemLabelManagerSubsystem();

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	
	void InitializeItemLabelManager(UObsidianItemLabelOverlay* InItemLabelOverlay, AObsidianPlayerController* InObsidianPC);

	void RegisterItemLabel(const FObsidianItemLabelInfo& ItemLabelInfo);
	
	// ~ Start of USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// ~ End of USubsystem

protected:
	void UpdateLabelAnchors();
	
private:
	UPROPERTY()
	TObjectPtr<UObsidianItemLabelOverlay> ItemLabelOverlay;

	UPROPERTY()
	TWeakObjectPtr<AObsidianPlayerController> OwningPC;

	UPROPERTY()
	TArray<FObsidianItemLabelData> ItemLabelsData;
};
