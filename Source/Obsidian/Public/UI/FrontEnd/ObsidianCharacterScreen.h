// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "UI/ObsidianActivatableWidget.h"

#include "ObsidianCharacterScreen.generated.h"

struct FObsidianHeroSaveInfo;

class UObsidianHeroSaveGame;
class UObsidianCharacterEntry;
class AObsidianFrontEndGameMode;
class UCommonHierarchicalScrollBox;
class UCommonTextBlock;
class UObsidianButtonBase;
class UObCharacterSelectionWidgetController;

/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API UObsidianCharacterScreen : public UObsidianActivatableWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterScreen")
	void InitializeOnlineCharacterScreen();

	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterScreen")
	void InitializeOfflineCharacterScreen();

protected:
	void OnPlayClicked();
	void OnDeleteClicked();

	void PopulateCharacterScreen();
	void InitCharacterScreen();

	void HandleClickingOnCharacterEntry(UObsidianCharacterEntry* InEntryClicked);

	void CreateHeroEntries(const TArray<FObsidianHeroSaveInfo>& InSaveInfos);

	void OnPlayHeroLoadFinished(UObsidianHeroSaveGame* InSaveObject, bool bInSuccess);

	//~ Begin UCommonActivatableWidget Interface
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	//~ End UCommonActivatableWidget Interface

	//~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	//~ End UUserWidget Interface

	//~ Begin UObsidianActivatableWidget Interface
	virtual void HandleWidgetControllerSet() override;
	//~ End UObsidianActivatableWidget Interface

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TSoftClassPtr<UCommonActivatableWidget> SoftOnlineLobbyWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TSubclassOf<UObsidianCharacterEntry> CharacterEntryWidgetClass;

	UPROPERTY()
	TObjectPtr<UObsidianCharacterEntry> CachedChosenCharacterEntry;

	UPROPERTY()
	TObjectPtr<AObsidianFrontEndGameMode> FrontEndGameMode;

	UPROPERTY()
	TArray<TObjectPtr<UObsidianCharacterEntry>> CharacterEntries; 

	//TODO(intrxx) Change this to some general call into network subsystem?
	UPROPERTY(BlueprintReadOnly)
	bool bOnline = false;

	FDelegateHandle OnPopulateLoadingFinishedDelegateHandle;
	FDelegateHandle OnPlayLoadingFinishedDelegateHandle;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonHierarchicalScrollBox> CharacterList_ScrollBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Play_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Create_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Delete_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> TabName_TextBlock;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObCharacterSelectionWidgetController> CharacterSelectionWidgetController;
};
