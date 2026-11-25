// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GameplayTagContainer.h"

#include "TinyPOIComponent.generated.h"

/**
 * Markup of a single Point Of Interest, the source the baked FTinyPOIPoint is made from.
 *
 * It only exists in the editor: it is stripped from cooked builds and does nothing in PIE, the game reads the Points
 * from ATinyPOIData. While in the editor world it keeps the FTinyPOIBuilder up to date about itself, so the bake
 * follows every change made to the level.
 */
UCLASS(ClassGroup = (TinyPOI), meta = (PrioritizeCategories = "TinyPOI", BlueprintSpawnableComponent))
class OBSIDIAN_API UTinyPOIComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UTinyPOIComponent(const FObjectInitializer& InObjectInitializer);

	FGameplayTag GetTag() const { return Tag; }
	void SetTag(const FGameplayTag& InTag);

	float GetRadius() const { return Radius; }
	void SetRadius(const float InRadius);

#if WITH_EDITOR
	/**
	 * Hash of everything the bake reads from this component, used to tell if the baked data is still up to date.
	 * It is saved with the level, so it has to give the same value in every editor session.
	 */
	uint32 GetPOIHash() const;
#endif

	//~ Begin UObject Interface
	virtual bool IsEditorOnly() const override { return true; }
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& InPropertyChangedEvent) override;
	virtual void PostEditUndo() override;
#endif
	//~ End UObject Interface

protected:
	//~ Begin UActorComponent Interface
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	//~ End UActorComponent Interface

	//~ Begin USceneComponent Interface
	virtual void OnUpdateTransform(EUpdateTransformFlags InUpdateTransformFlags, ETeleportType InTeleport = ETeleportType::None) override;
	//~ End USceneComponent Interface

private:
#if WITH_EDITOR
	/** Tells the Builder of the editor world that the bake is out of date. */
	void NotifyBuilderOfChange();
#endif

	/** What kind of Point this is, queries filter by it. */
	UPROPERTY(EditAnywhere, Category = "TinyPOI", meta = (Categories = "TinyPOI"))
	FGameplayTag Tag;

	/** Size of the Point, gets multiplied by the scale of the component when baked. */
	UPROPERTY(EditAnywhere, Category = "TinyPOI", meta = (ClampMin = "0.0", Units = "cm"))
	float Radius = 100.0f;
};
