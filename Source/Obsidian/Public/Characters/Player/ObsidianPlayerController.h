// Copyright 2024 Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ObsidianPlayerController.generated.h"

struct FObsidianDamageTextProps;
class AObsidianCharacterBase;
class UObsidianDamageNumberWidgetComp;
class AObsidianHUD;
class UObsidianAbilitySystemComponent;
class AObsidianPlayerState;

/**
 * Base class for Obsidian player characters player controller.
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

	void SetupHeroHealthBarWidget();

	UFUNCTION(Client, Reliable)
	void ClientShowDamageNumber(const FObsidianDamageTextProps& DamageTextProps, AObsidianCharacterBase* TargetCharacter);
	
protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Cursor")
	TObjectPtr<UUserWidget> DefaultCursor;


private:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|DamageNumber")
	TSubclassOf<UObsidianDamageNumberWidgetComp> DamageNumberWidgetCompClass;
	
};
