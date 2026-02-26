// Copyright 2026 out of sCope - intrxx

#pragma once

#include <CoreMinimal.h>


#include <Subsystems/WorldSubsystem.h>
#include "ObsidianItemLabelManagerSubsystem.generated.h"

class AObsidianItemLabelSystemLateTickActor;
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

	void ResetLabelData();
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
class OBSIDIAN_API UObsidianItemLabelManagerSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UObsidianItemLabelManagerSubsystem();

	virtual void Tick(float DeltaTime) override;
	void PostWorkTick(float DeltaTime);
	
	virtual TStatId GetStatId() const override;
	
	void InitializeItemLabelManager(UObsidianItemLabelOverlay* InItemLabelOverlay, AObsidianPlayerController* InObsidianPC);

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
	void UpdateLabelAnchors(float DeltaTime);
	void SolveLabelLayout_1();
	void SolveLabelLayout_2();

	bool IsOutsideCurrentViewport(const FVector2D& ViewportPosition);
	
	static bool CheckVerticalOverlap(const FObsidianItemLabelData& LabelA, const FObsidianItemLabelData& LabelB);
	static bool CheckHorizontalOverlap(const FObsidianItemLabelData& LabelA, const FObsidianItemLabelData& LabelB);
	
	void HandleViewportResize(FViewport* Viewport, uint32 /** unused */);

	UObsidianItemLabel* AcquireWidget(const FGuid& ForID);
	void ReleaseWidget(UObsidianItemLabel* LabelWidget);

	void HandleLabelHovered(const bool bEnter, const FGuid& LabelID);
	void HandleLabelPressed(const int32 PlayerIndex, const FObsidianItemInteractionFlags& InteractionFlags,
		const FGuid& LabelID);

	void MakeLayoutDirty();
	void MakeLayoutClean();
	
private:
	UPROPERTY()
	TSubclassOf<UObsidianItemLabel> ItemLabelClass;

	// ~ Debug
	UPROPERTY()
	TSubclassOf<UUserWidget> ItemLabelHelperTLClass;
	UPROPERTY()
	TSubclassOf<UUserWidget> ItemLabelHelperBRClass;
	// ~ End of Debug
	
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

struct FDeterministicRadialEnumerator
{
	
public:
	FDeterministicRadialEnumerator(const float InStep, const int32 InMaxRings, const int32 InBaseSamples = 8)
		: Step(InStep)
		, MaxRings(InMaxRings)
		, BaseSamples(InBaseSamples)
	{}

	void Reset()
	{
		Ring = 0;
		AngleIndex = 0;
		bFirst = true;
		SamplesThisRing = 1;
	}

	bool Next(FVector2D& OutOffset)
	{
		if (bFirst)
		{
			bFirst = false;
			OutOffset = FVector2D::ZeroVector;
			return true;
		}

		if (Ring >= MaxRings)
		{
			return false;
		}

		if (AngleIndex >= SamplesThisRing)
		{
			Ring++;
			AngleIndex = 0;
			SamplesThisRing = BaseSamples + Ring * 6;
		}

		const float Radius = Ring * Step;
		const float Angle = (2.f * PI * AngleIndex) / SamplesThisRing;

		float SinA, CosA;
		FMath::SinCos(&SinA, &CosA, Angle);

		FVector2D Offset(CosA * Radius, SinA * Radius);
		
		Offset.X = FMath::RoundToFloat(Offset.X);
		Offset.Y = FMath::RoundToFloat(Offset.Y);

		OutOffset = Offset;

		AngleIndex++;
		return true;
	}

private:
	float Step;
	int32 MaxRings;
	int32 BaseSamples;

	int32 Ring = 0;
	int32 AngleIndex = 0;
	int32 SamplesThisRing = 1;
	bool bFirst = true;
};
