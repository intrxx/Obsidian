// Copyright 2026 out of sCope - intrxx

#pragma once

#include <CoreMinimal.h>


#include <Subsystems/WorldSubsystem.h>
#include "ObsidianItemLabelManagerSubsystem.generated.h"

struct FObsidianItemInteractionFlags;
class UObsidianItemLabelComponent;
class ItemLabelClass;
class UCanvasPanelSlot;
class AObsidianDroppableItem;
class UObsidianItemLabel;
class AObsidianPlayerController;
class UObsidianItemLabelOverlay;

DECLARE_LOG_CATEGORY_EXTERN(LogItemLabelManager, Log, All);

USTRUCT()
struct FObsidianItemLabelData
{
	GENERATED_BODY()
	
public:
	FObsidianItemLabelData(){}

	bool IsValid() const;
	
public:
	/**
	 * Initialization
	 */

	/** Vector position which is adjusted by the ItemLabelGroundZOffset. */
	UPROPERTY()
	FVector LabelAdjustedWorldPosition = FVector::Zero();

	UPROPERTY()
	FVector2D LabelAnchorPosition = FVector2D::Zero();

	UPROPERTY()
	FGuid LabelID = FGuid();

	UPROPERTY()
	FVector2D LabelSize = FVector2D::Zero();

	UPROPERTY()
	TObjectPtr<UObsidianItemLabel> ItemLabelWidget;

	UPROPERTY()
	TObjectPtr<UCanvasPanelSlot> CanvasPanelSlot;

	UPROPERTY()
	TObjectPtr<UObsidianItemLabelComponent> SourceLabelComponent;

	UPROPERTY()
	uint8 Priority = 8;

	/**
	 * Dynamic
	 */
	
	UPROPERTY()
	FVector2D LabelSolvedPositionOffset = FVector2D::Zero();
	
	UPROPERTY()
	FVector2D LabelSolvedPosition = FVector2D::Zero();
	
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

	FGuid RegisterItemLabel(UObsidianItemLabelComponent* SourceLabelComponent);
	void UnregisterItemLabel(const FGuid& LabelID);

	void ToggleItemLabelHighlight(const bool bHighlight);
	
	// ~ Start of USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// ~ End of USubsystem

protected:
	void UpdateLabelAnchors(float DeltaTime);
	void SolveLabelLayout();

	bool IsOutsideCurrentViewport(const FVector2D& ViewportPosition);
	
	static bool CheckVerticalOverlap(const FObsidianItemLabelData& LabelA, const FObsidianItemLabelData& LabelB);
	static bool CheckHorizontalOverlap(const FObsidianItemLabelData& LabelA, const FObsidianItemLabelData& LabelB);
	
	void HandleViewportResize(FViewport* Viewport, uint32 /** unused */);

	UObsidianItemLabel* AcquireWidget(const FGuid& ForID);
	void ReleaseWidget(UObsidianItemLabel* LabelWidget);

	void HandleLabelHovered(const bool bEnter, const FGuid& LabelID);
	void HandleLabelPressed(const int32 PlayerIndex, const FObsidianItemInteractionFlags& InteractionFlags,
		const FGuid& LabelID);
	
private:
	UPROPERTY()
	TSubclassOf<UObsidianItemLabel> ItemLabelClass;
	
	UPROPERTY()
	TObjectPtr<UObsidianItemLabelOverlay> ItemLabelOverlay;

	UPROPERTY()
	TWeakObjectPtr<AObsidianPlayerController> OwningPC;

	UPROPERTY()
	TMap<FGuid, FObsidianItemLabelData> ItemLabelsDataMap;

	UPROPERTY()
	TArray<UObsidianItemLabel*> LabelWidgetPool;

	float ItemLabelGroundZOffset = 0.0f;
	float LabelAdjustmentSmoothSpeed = 0.0f;

	FDelegateHandle OnViewportResizeDelegateHandle;

	bool bLayoutDirty = false;

	bool bLabelOverlayVisible = true;
};
