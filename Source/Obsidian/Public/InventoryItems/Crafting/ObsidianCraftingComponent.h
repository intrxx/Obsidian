// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "ObsidianCraftingComponent.generated.h"

struct FObsidianItemPosition;
struct FObsidianStashChangeMessage;
struct FObsidianInventoryChangeMessage;

class UObsidianItem;
class UObsidianInventoryItemInstance;
class UObsidianDraggedItem_Simple;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStartUsingItemSignature, UObsidianInventoryItemInstance* UsingInstance)
DECLARE_MULTICAST_DELEGATE(FOnStopUsingItemSignature)


/**
 * Component which grants the ability to use crafting items to modify other item's properties.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OBSIDIAN_API UObsidianCraftingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UObsidianCraftingComponent(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	
	bool IsUsingItem() const;
	UObsidianInventoryItemInstance* GetUsingItem();

	void UseItem(const FObsidianItemPosition& InOnPosition, const bool bInLeftShiftDown);
	void SetUsingItem(const bool InInbUsingItem, UObsidianItem* InItemWidget = nullptr,
		UObsidianInventoryItemInstance* InUsingInstance = nullptr);
	
	UFUNCTION(Server, Reliable)
	void ServerActivateUsableItemFromInventory(UObsidianInventoryItemInstance* InUsingInstance);
	UFUNCTION(Server, Reliable)
	void ServerActivateUsableItemFromStash(UObsidianInventoryItemInstance* InUsingInstance);

	//~ Start of UObject interface
	virtual bool ReplicateSubobjects(UActorChannel* InChannel, FOutBunch* InBunch, FReplicationFlags* InRepFlags) override;
	virtual void ReadyForReplication() override;
	//~ End of UObject interface

public:
	FOnStartUsingItemSignature OnStartUsingItemDelegate;
	FOnStopUsingItemSignature OnStopUsingItemDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float InDeltaTime, enum ELevelTick InTickType,
		FActorComponentTickFunction* InThisTickFunction) override;

	void OnInventoryStateChanged(FGameplayTag InChannel, const FObsidianInventoryChangeMessage& InInventoryChangeMessage);
	void OnPlayerStashChanged(FGameplayTag InChannel, const FObsidianStashChangeMessage& InStashChangeMessage);

	void InitializeCraftingComponent();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Items", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianDraggedItem_Simple> UsingItemIconClass;
	
private:
	UFUNCTION(Server, Reliable)
	void ServerUseItem(UObsidianInventoryItemInstance* InUsingInstance, const FObsidianItemPosition& InOnPosition);
	
	void DragUsableItemIcon() const;

private:
	UPROPERTY()
	TObjectPtr<UObsidianInventoryItemInstance> CachedUsingItemInstance = nullptr;

	UPROPERTY()
	TObjectPtr<UObsidianDraggedItem_Simple> CachedActiveUsingItemIcon = nullptr;

	/** Widget of the item being currently used. */
	UPROPERTY()
	TObjectPtr<UObsidianItem> CachedUsingItemWidget = nullptr;

	bool bUsingItem = false;
};
