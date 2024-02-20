// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ObsidianPlayerController.generated.h"

class IObsidianHighlightInterface;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AObsidianPlayerController();

	virtual void PlayerTick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;

private:
	void CursorTrace();
	
	IObsidianHighlightInterface* LastHighlightedActor = nullptr;
	IObsidianHighlightInterface* CurrentHighlightedActor = nullptr;
};
