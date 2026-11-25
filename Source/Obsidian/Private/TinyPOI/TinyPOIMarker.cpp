// Copyright 2026 out of sCope team - intrxx

#include "TinyPOI/TinyPOIMarker.h"

#include "TinyPOI/TinyPOIComponent.h"


ATinyPOIMarker::ATinyPOIMarker(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	POIComponent = CreateDefaultSubobject<UTinyPOIComponent>(TEXT("POIComponent"));
	RootComponent = POIComponent;

	SetCanBeDamaged(false);

#if WITH_EDITORONLY_DATA
	// The bake only sees the markup that is loaded, in a World Partition level a Marker that is streamed out in the
	// editor would silently drop out of the baked data.
	bIsSpatiallyLoaded = false;
#endif
}
