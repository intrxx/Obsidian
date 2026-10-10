// Copyright 2026 out of sCope - intrxx

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "ObsidianItemLabelComponent.generated.h"

USTRUCT()
struct FObsidianLabelInitializationData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FText ItemName = FText();

	// + Label Icon in the feature

	/** Priority can range from 0 to 8. */
	UPROPERTY()
	uint8 Priority = 8;
};

struct FObsidianItemInteractionFlags;
class UObsidianItemLabel;
class AObsidianDroppableItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OBSIDIAN_API UObsidianItemLabelComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UObsidianItemLabelComponent(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	void SetItemOwner(AObsidianDroppableItem* InOwningItemActor);

	FVector GetOwningItemActorLocation() const;
	FObsidianLabelInitializationData GetLabelInitializationData() const;
	
	void RegisterLabelComponent();

	void HandleLabelMouseHover(const bool bInMouseEnter);
	void HandleLabelMouseButtonDown(const int32 InPlayerIndex, const FObsidianItemInteractionFlags& InInteractionFlags);

protected:
	virtual void EndPlay(const EEndPlayReason::Type InEndPlayReason) override;
	
private:
	FGuid RegisteredLabelID = FGuid();
	TWeakObjectPtr<AObsidianDroppableItem> WeakOwningItemActor;
};
