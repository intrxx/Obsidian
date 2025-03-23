// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "ObsidianSpawnedEquipmentPiece.generated.h"

UCLASS()
class OBSIDIAN_API AObsidianSpawnedEquipmentPiece : public AActor
{
	GENERATED_BODY()
	
public:	
	AObsidianSpawnedEquipmentPiece(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	USkeletalMeshSocket const* GetEquipmentSocketByName(const FName SocketName) const;
	USkeletalMeshComponent* GetEquipmentPieceMesh() const
	{
		return EquipmentPieceMesh;
	}

public:
	FGameplayTag AssociatedSlotTag = FGameplayTag::EmptyTag;

	/** Whether, or not this piece is main equipment for example: Bow might add two Actor to the RightHandSlot, the actual bow and some protection strap on the hand. */
	bool bMainEquipmentPiece = true;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TObjectPtr<USkeletalMeshComponent> EquipmentPieceMesh;
};
