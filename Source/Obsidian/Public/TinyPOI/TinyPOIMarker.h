// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "TinyPOIMarker.generated.h"

class UTinyPOIComponent;

/**
 * Actor to place a Point Of Interest in the level with. Just like its component it only exists in the editor.
 */
UCLASS(HideCategories = (Input), meta = (PrioritizeCategories = "TinyPOI"))
class OBSIDIAN_API ATinyPOIMarker : public AActor
{
	GENERATED_BODY()

public:
	ATinyPOIMarker(const FObjectInitializer& InObjectInitializer);

	UTinyPOIComponent* GetPOIComponent() const { return POIComponent; }

	//~ Begin UObject Interface
	virtual bool IsEditorOnly() const override { return true; }
	//~ End UObject Interface

#if WITH_EDITOR
	//~ Begin AActor Interface
	virtual bool CanChangeIsSpatiallyLoadedFlag() const override { return false; }
	//~ End AActor Interface
#endif

private:
	UPROPERTY(VisibleAnywhere, Category = "TinyPOI")
	TObjectPtr<UTinyPOIComponent> POIComponent;
};
