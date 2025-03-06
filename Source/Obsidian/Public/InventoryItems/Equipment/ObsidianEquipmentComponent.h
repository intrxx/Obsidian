// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObsidianEquipmentComponent.generated.h"

/**
 * Component that manages equipping items on the Heroes.
 */
UCLASS( ClassGroup=(InventoryItems), meta=(BlueprintSpawnableComponent) )
class OBSIDIAN_API UObsidianEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UObsidianEquipmentComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};
