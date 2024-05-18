// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ObsidianPlayerController.generated.h"

class AObsidianHUD;
class UObsidianAbilitySystemComponent;
class AObsidianPlayerState;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AObsidianPlayerController();
	
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|PlayerController")
	AObsidianPlayerState* GetObsidianPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|PlayerController")
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|PlayerController")
	AObsidianHUD* GetObsidianHUD() const;
	
protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Cursor")
	TObjectPtr<UUserWidget> DefaultCursor;
	
};
