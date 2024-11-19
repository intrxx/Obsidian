// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianPickableInterface.h"
#include "GameFramework/Actor.h"
#include "ObsidianDroppable.generated.h"

UCLASS()
class OBSIDIAN_API AObsidianDroppable : public AActor, public IObsidianPickableInterface
{
	GENERATED_BODY()
	
public:	
	AObsidianDroppable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual FPickupContent GetPickupContent() const override;
	
protected:
	UPROPERTY(EditAnywhere)
	FPickupContent StaticContent;
};
