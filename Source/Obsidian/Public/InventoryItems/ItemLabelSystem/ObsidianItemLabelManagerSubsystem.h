// Copyright 2026 out of sCope - intrxx

#pragma once

#include <CoreMinimal.h>


#include <Subsystems/WorldSubsystem.h>
#include "ObsidianItemLabelManagerSubsystem.generated.h"

class UCanvasPanelSlot;
class AObsidianDroppableItem;
class UObsidianItemLabel;
class AObsidianPlayerController;
class UObsidianItemLabelOverlay;

DECLARE_LOG_CATEGORY_EXTERN(LogItemLabelManager, Log, All);

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

	/** Vector position which is adjusted by the ItemLabelGroundZOffset. */
	UPROPERTY()
	FVector LabelAdjustedWorldPosition = FVector::Zero();
	
	UPROPERTY()
	FVector2D LabelAnchorPosition = FVector2D::Zero();

	UPROPERTY()
	FVector2D LabelSolvedPositionOffset = FVector2D::Zero();
	
	UPROPERTY()
	FVector2D LabelSolvedPosition = FVector2D::Zero();

	UPROPERTY()
	FVector2D LabelSize = FVector2D::Zero();

	UPROPERTY()
	uint8 Priority = 8;

	UPROPERTY()
	uint8 bVisible:1 = false;
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
	void UnregisterItemLabel(UObsidianItemLabel* ItemLabel);

	void ToggleItemLabelHighlight(const bool bHighlight);
	
	// ~ Start of USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// ~ End of USubsystem

protected:
	void UpdateLabelAnchors();

	void ClusterLabels();
	void SolveLabelLayout();
	void SolveVerticalCluster();

	static bool CheckVerticalOverlap(const FObsidianItemLabelData& LabelA, const FObsidianItemLabelData& LabelB);
	static bool CheckHorizontalOverlap(const FObsidianItemLabelData& LabelA, const FObsidianItemLabelData& LabelB);

	void HandleViewportResize(FViewport* Viewport, uint32 /** unused */);
	
private:
	UPROPERTY()
	TObjectPtr<UObsidianItemLabelOverlay> ItemLabelOverlay;

	UPROPERTY()
	TWeakObjectPtr<AObsidianPlayerController> OwningPC;

	UPROPERTY()
	TArray<FObsidianItemLabelData> ItemLabelsData;

	float ItemLabelGroundZOffset = 0.0f;

	FDelegateHandle OnViewportResizeDelegateHandle;

	bool bLayoutDirty = false;

	bool bLabelOverlayVisible = true;
};
