// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include <GameplayTagContainer.h>

#include <Components/ActorComponent.h>
#include "ObsidianCraftingComponent.generated.h"

struct FObsidianItemPosition;
struct FObsidianStashChangeMessage;
struct FObsidianInventoryChangeMessage;

class UObsidianItem;
class UObsidianInventoryItemInstance;
class UObsidianDraggedItem_Simple;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStartUsingItemSignature, UObsidianInventoryItemInstance* UsingInstance)
DECLARE_MULTICAST_DELEGATE(FOnStopUsingItemSignature)

DECLARE_LOG_CATEGORY_EXTERN(LogCrafting, Log, All);


/**
 * Component which grants the ability to use crafting items to modify other item's properties.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OBSIDIAN_API UObsidianCraftingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UObsidianCraftingComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	bool IsUsingItem() const;
	UObsidianInventoryItemInstance* GetUsingItem();
	
	void SetUsingItem(const bool InbUsingItem, UObsidianItem* ItemWidget = nullptr,
		UObsidianInventoryItemInstance* UsingInstance = nullptr);
	
	void UseItem(const FObsidianItemPosition& OnPosition, const bool bLeftShiftDown);

	UFUNCTION(Server, Reliable)
	void ServerActivateUsableItemFromInventory(UObsidianInventoryItemInstance* UsingInstance);
	UFUNCTION(Server, Reliable)
	void ServerActivateUsableItemFromStash(UObsidianInventoryItemInstance* UsingInstance);

	//~ Start of UObject interface
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~ End of UObject interface

public:
	FOnStartUsingItemSignature OnStartUsingItemDelegate;
	FOnStopUsingItemSignature OnStopUsingItemDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	void OnInventoryStateChanged(FGameplayTag Channel, const FObsidianInventoryChangeMessage& InventoryChangeMessage);
	void OnPlayerStashChanged(FGameplayTag Channel, const FObsidianStashChangeMessage& StashChangeMessage);

	void InitializeCraftingComponent();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Items", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianDraggedItem_Simple> UsingItemIconClass;
	
private:
	UFUNCTION(Server, Reliable)
	void ServerUseItem(UObsidianInventoryItemInstance* UsingInstance, const FObsidianItemPosition& OnPosition);
	
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
