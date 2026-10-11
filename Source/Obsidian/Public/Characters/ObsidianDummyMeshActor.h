// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "ObsidianDummyMeshActor.generated.h"

class UPoseableMeshComponent;

/**
 * Actor used to replace the lying enemy with a lighter actor composed of PoseableMesh.
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API AObsidianDummyMeshActor : public AActor
{
	GENERATED_BODY()

public:
	AObsidianDummyMeshActor(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	void SetupDummyMeshActor(USkeletalMeshComponent* InMeshToCopy, const float InLifeSpan);

	//~ Begin AActor Interface
	virtual void Destroyed() override;
	//~ End AActor Interface

protected:
	//~ Begin AActor Interface
	virtual void BeginPlay() override;
	//~ End AActor Interface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPoseableMeshComponent> PoseableMeshComp;

	UPROPERTY(EditAnywhere, Category = "Dummy Dead Mesh")
	TObjectPtr<USkeletalMeshComponent> DeadMeshToCopy;
};
