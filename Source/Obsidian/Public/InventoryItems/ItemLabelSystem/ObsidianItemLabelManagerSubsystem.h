// Copyright 2026 out of sCope - intrxx

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "ObsidianItemLabelManagerSubsystem.generated.h"

struct FObsidianItemInteractionFlags;

class UObsidianItemLabelComponent;
class UCanvasPanelSlot;
class UObsidianItemLabel;
class AObsidianPlayerController;
class UObsidianMainOverlay;

USTRUCT()
struct FObsidianItemLabelData
{
	GENERATED_BODY()

public:
	FObsidianItemLabelData(){}

	bool IsValid() const;

	void ResetLabelData();
public:
	/**
	 * Initialization
	 */

	/** Vector position which is adjusted by the ItemLabelGroundZOffset. */
	UPROPERTY()
	FVector LabelAdjustedWorldPosition = FVector::Zero();

	UPROPERTY()
	FGuid LabelID = FGuid();

	UPROPERTY()
	TObjectPtr<UObsidianItemLabelComponent> SourceLabelComponent;

	UPROPERTY()
	uint8 Priority = 8;

	/** Registration order, used as a stable tiebreaker so the solve order never depends on screen positions. */
	UPROPERTY()
	uint32 RegistrationIndex = 0;

	/**
	 * Dynamic
	 */

	/** Projected LabelAdjustedWorldPosition in the overlay's canvas space, refreshed every frame. */
	UPROPERTY()
	FVector2D LabelAnchorPosition = FVector2D::Zero();

	UPROPERTY()
	FVector2D LabelSize = FVector2D::Zero();

	UPROPERTY()
	TObjectPtr<UObsidianItemLabel> ItemLabelWidget;

	UPROPERTY()
	TObjectPtr<UCanvasPanelSlot> CanvasPanelSlot;

	/** Offset from the anchor the solver wants this frame. */
	UPROPERTY()
	FVector2D LabelSolvedPositionOffset = FVector2D::Zero();

	/** Offset from the anchor that is actually displayed, smoothed towards LabelSolvedPositionOffset. */
	UPROPERTY()
	FVector2D LabelDisplayedPositionOffset = FVector2D::Zero();

	/** Final center of the label in the overlay's canvas space. */
	UPROPERTY()
	FVector2D LabelSolvedPosition = FVector2D::Zero();

	UPROPERTY()
	uint8 bVisible:1 = false;

	/** Skips the smoothing for the next update, so freshly shown labels don't slide in from a stale offset. */
	UPROPERTY()
	uint8 bSnapToSolvedPosition:1 = true;
};

USTRUCT()
struct FObsidianLabelManagerLateTickFunction : public FTickFunction
{
	GENERATED_USTRUCT_BODY()

public:
	TObjectPtr<class UObsidianItemLabelManagerSubsystem> Target = nullptr;

public:
	virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread,
		const FGraphEventRef& MyCompletionGraphEvent) override;

	virtual FString DiagnosticMessage() override;
};

template<>
struct TStructOpsTypeTraits<FObsidianLabelManagerLateTickFunction> : public TStructOpsTypeTraitsBase2<FObsidianLabelManagerLateTickFunction>
{
	enum
	{
		WithCopy = false,
		WithPureVirtual = true
	};
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemLabelManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UObsidianItemLabelManagerSubsystem();

	void PostWorkTick(float DeltaTime);

	void InitializeItemLabelManager(UObsidianMainOverlay* InItemLabelOverlay, AObsidianPlayerController* InObsidianPC);

	FGuid RegisterItemLabel(UObsidianItemLabelComponent* SourceLabelComponent);
	void UnregisterItemLabel(const FGuid& LabelID);

	void ToggleItemLabelHighlight(const bool bHighlight);

	// ~ Start of USubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// ~ End of USubsystem

public:
	FObsidianLabelManagerLateTickFunction LateTickFunction;

protected:
	// ~ Start of UWorldSubsystem
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;
	// ~ End of UWorldSubsystem

	void UpdateLabels(float DeltaTime);
	void UpdateLabelAnchors(TArray<FObsidianItemLabelData*>& OutLabelsToSolve, FBox2D& OutViewportArea);
	void SolveLabelLayout(TArray<FObsidianItemLabelData*>& LabelsToSolve, const FBox2D& ViewportArea);

	bool ActivateLabel(FObsidianItemLabelData& LabelData);
	void DeactivateLabel(FObsidianItemLabelData& LabelData);

	UObsidianItemLabel* AcquireWidget(const FGuid& ForID);
	void ReleaseWidget(UObsidianItemLabel* LabelWidget);

	void HandleLabelHovered(const bool bEnter, const FGuid& LabelID);
	void HandleLabelPressed(const int32 PlayerIndex, const FObsidianItemInteractionFlags& InteractionFlags,
		const FGuid& LabelID);

private:
	UPROPERTY()
	TSubclassOf<UObsidianItemLabel> ItemLabelClass;

	UPROPERTY()
	TObjectPtr<UObsidianMainOverlay> MainOverlay;

	UPROPERTY()
	TWeakObjectPtr<AObsidianPlayerController> OwningPC;

	UPROPERTY()
	TMap<FGuid, FObsidianItemLabelData> ItemLabelsDataMap;

	UPROPERTY()
	TArray<UObsidianItemLabel*> LabelWidgetPool;

	float ItemLabelGroundZOffset = 0.0f;
	float LabelAdjustmentSmoothSpeed = 0.0f;

	uint32 NextRegistrationIndex = 0;

	bool bLabelOverlayVisible = true;
};
