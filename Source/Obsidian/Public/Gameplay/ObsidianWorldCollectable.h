// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "InventoryItems/ObsidianPickableInterface.h"
#include "GameFramework/Actor.h"
#include "ObsidianWorldCollectable.generated.h"

UCLASS()
class OBSIDIAN_API AObsidianWorldCollectable : public AActor, public IObsidianPickableInterface
{
	GENERATED_BODY()
	
public:	
	AObsidianWorldCollectable(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual FPickupContent GetPickupContent() const override;
	
protected:
	UPROPERTY(EditAnywhere)
	FPickupContent StaticContent;
};