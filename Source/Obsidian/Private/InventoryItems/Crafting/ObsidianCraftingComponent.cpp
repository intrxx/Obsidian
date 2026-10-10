// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Crafting/ObsidianCraftingComponent.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"
#include "Obsidian/ObsidianLogCategories.h"
#include "UI/InventoryItems/Items/ObsidianDraggedItem_Simple.h"
#include "UI/InventoryItems/Items/ObsidianItem.h"


UObsidianCraftingComponent::UObsidianCraftingComponent(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UObsidianCraftingComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeCraftingComponent();
}

void UObsidianCraftingComponent::TickComponent(float InDeltaTime, enum ELevelTick InTickType,
                                               FActorComponentTickFunction* InThisTickFunction)
{
	Super::TickComponent(InDeltaTime, InTickType, InThisTickFunction);

	if(bUsingItem)
	{
		DragUsableItemIcon();
	}
}

void UObsidianCraftingComponent::InitializeCraftingComponent()
{
	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(ObLogCrafting, Error, TEXT("Controller is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const AActor* OwningActor = Cast<AActor>(Controller->GetPawn());
	if (OwningActor == nullptr)
	{
		UE_LOG(ObLogCrafting, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(OwningActor->GetWorld());
	MessageSubsystem.RegisterListener(ObsidianGameplayTags::Message::Inventory::Changed, this,
		&ThisClass::OnInventoryStateChanged);
	MessageSubsystem.RegisterListener(ObsidianGameplayTags::Message::PlayerStash::Changed, this,
		&ThisClass::OnPlayerStashChanged);
}

void UObsidianCraftingComponent::OnInventoryStateChanged(FGameplayTag InChannel,
	const FObsidianInventoryChangeMessage& InInventoryChangeMessage)
{
	if (GetOwner() != InInventoryChangeMessage.InventoryOwner->GetOwner())
	{
		return;
	}

	const UObsidianInventoryItemInstance* Instance = InInventoryChangeMessage.ItemInstance;
	if(Instance == nullptr)
	{
		UE_LOG(ObLogCrafting, Error, TEXT("Item Instance is invalid in [%hs]"), __FUNCTION__);
		return;
	}
	
	if(InInventoryChangeMessage.ChangeType == EObsidianInventoryChangeType::ICT_ItemRemoved)
	{
		UE_LOG(ObLogCrafting, Verbose, TEXT("Stopping Usage of item: [%s] from Inventory, due to removal"),
			*Instance->GetItemDisplayName().ToString());

		if(bUsingItem && Instance == CachedUsingItemInstance)
		{
			SetUsingItem(false);
		}
	}
}

void UObsidianCraftingComponent::OnPlayerStashChanged(FGameplayTag InChannel,
	const FObsidianStashChangeMessage& InStashChangeMessage)
{
	if (GetOwner() != InStashChangeMessage.PlayerStashOwner->GetOwner())
	{
		return;
	}
	
	const UObsidianInventoryItemInstance* Instance = InStashChangeMessage.ItemInstance;
	if(Instance == nullptr)
	{
		UE_LOG(ObLogCrafting, Error, TEXT("Item Instance is invalid in [%hs]"), __FUNCTION__);
		return;
	}
	
	if(InStashChangeMessage.ChangeType == EObsidianStashChangeType::ICT_ItemRemoved)
	{
		UE_LOG(ObLogCrafting, Verbose, TEXT("Stopping Usage of item: [%s] from Player Stash, due to removal"),
			*Instance->GetItemDisplayName().ToString());
		
		if(bUsingItem && Instance == CachedUsingItemInstance)
		{
			SetUsingItem(false);
		}
	}
}

bool UObsidianCraftingComponent::ReplicateSubobjects(UActorChannel* InChannel, FOutBunch* InBunch,
                                                     FReplicationFlags* InRepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(InChannel, InBunch, InRepFlags);

	if(CachedUsingItemInstance && IsValid(CachedUsingItemInstance))
	{
		WroteSomething |= InChannel->ReplicateSubobject(CachedUsingItemInstance, *InBunch, *InRepFlags);
	}
	
	return WroteSomething;
}

void UObsidianCraftingComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if(IsValid(CachedUsingItemInstance))
	{
		AddReplicatedSubObject(CachedUsingItemInstance);
	}
}

bool UObsidianCraftingComponent::IsUsingItem() const
{
	return bUsingItem;
}

UObsidianInventoryItemInstance* UObsidianCraftingComponent::GetUsingItem()
{
	return CachedUsingItemInstance;
}

void UObsidianCraftingComponent::UseItem(const FObsidianItemPosition& InOnPosition, const bool bInLeftShiftDown)
{
	ServerUseItem(CachedUsingItemInstance, InOnPosition);

	if(bInLeftShiftDown == false)
	{
		SetUsingItem(false);
	}
}

void UObsidianCraftingComponent::ServerActivateUsableItemFromInventory_Implementation(
	UObsidianInventoryItemInstance* InUsingInstance)
{
	if(InUsingInstance == nullptr)
	{
		UE_LOG(ObLogInventory, Error, TEXT("UsingInstance is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(ObLogInventory, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(ObLogInventory, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	InventoryComponent->UseItem(InUsingInstance, nullptr);
}

void UObsidianCraftingComponent::ServerActivateUsableItemFromStash_Implementation(
	UObsidianInventoryItemInstance* InUsingInstance)
{
	if(InUsingInstance == nullptr)
	{
		UE_LOG(ObLogCrafting, Error, TEXT("UsingInstance is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(ObLogCrafting, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if(PlayerStashComponent == nullptr)
	{
		UE_LOG(ObLogCrafting, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	PlayerStashComponent->UseItem(InUsingInstance, nullptr);
}

void UObsidianCraftingComponent::ServerUseItem_Implementation(UObsidianInventoryItemInstance* InUsingInstance,
	const FObsidianItemPosition& InOnPosition)
{
	if(InUsingInstance == nullptr)
	{
		UE_LOG(ObLogCrafting, Error, TEXT("UsingInstance is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(ObLogCrafting, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	if (InOnPosition.IsOnInventoryGrid())
	{
		UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
		if(InventoryComponent == nullptr)
		{
			UE_LOG(ObLogCrafting, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
			return;
		}

		UObsidianInventoryItemInstance* UsingOntoInstance = InventoryComponent->GetItemInstanceAtLocation(
			InOnPosition.GetItemGridPosition());
		InventoryComponent->UseItem(InUsingInstance, UsingOntoInstance);
	}
	else if (InOnPosition.IsOnStash())
	{
		const FGameplayTag StashTabTag = InOnPosition.GetOwningStashTabTag();
		if (StashTabTag == FGameplayTag::EmptyTag)
		{
			UE_LOG(ObLogCrafting, Error, TEXT("StashTab tag is empty in [%hs]."), __FUNCTION__);
		}

		UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
		if(PlayerStashComponent == nullptr)
		{
			UE_LOG(ObLogCrafting, Error, TEXT("PlayerStashComponent is null in [%hs]"), __FUNCTION__);
			return;
		}

		UObsidianInventoryItemInstance* UsingOntoInstance = PlayerStashComponent->GetItemInstanceFromTabAtPosition(
			InOnPosition);
		PlayerStashComponent->UseItem(InUsingInstance, UsingOntoInstance);
	}
	else if (InOnPosition.IsOnEquipmentSlot())
	{
		UObsidianEquipmentComponent* EquipmentComponent = Controller->FindComponentByClass<UObsidianEquipmentComponent>();
		if(EquipmentComponent == nullptr)
		{
			UE_LOG(ObLogCrafting, Error, TEXT("EquipmentComponent is null in [%hs]"), __FUNCTION__);
			return;
		}

		UObsidianInventoryItemInstance* UsingOntoInstance = EquipmentComponent->GetEquippedInstanceAtSlot(
			InOnPosition.GetItemSlotTag());
		//EquipmentComponent->UseItem(UsingInstance, UsingOntoInstance);
	}
}

void UObsidianCraftingComponent::DragUsableItemIcon() const
{
	const APlayerController* PC = Cast<APlayerController>(GetOwner());
	if(PC == nullptr)
	{
		return;
	}

	float LocationX = 0.0f;
	float LocationY = 0.0f;
	if(CachedActiveUsingItemIcon && PC->GetMousePosition(LocationX, LocationY))
	{
		if(UWorld* World = GetWorld())
		{
			const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(World);
			FVector2D ItemSize = CachedActiveUsingItemIcon->GetItemWidgetSize() + FVector2D(5.0f, -5.0f);
			ItemSize *= DPIScale;
			
			const FVector2D ViewportPosition = FVector2D(LocationX - ItemSize.X, LocationY - ItemSize.Y);
			CachedActiveUsingItemIcon->SetPositionInViewport(ViewportPosition);
		}
	}
}

void UObsidianCraftingComponent::SetUsingItem(const bool InInbUsingItem, UObsidianItem* InItemWidget,
                                              UObsidianInventoryItemInstance* InUsingInstance)
{
	if(InInbUsingItem && InItemWidget)
	{
		if (InUsingInstance == nullptr)
		{
			UE_LOG(ObLogCrafting, Error, TEXT("UsingInstance is invalid in [%hs]."), __FUNCTION__);
			return;
		}
		
		UWorld* World = GetWorld();
		if(World == nullptr)
		{
			return;
		}
		
		if(CachedActiveUsingItemIcon)
		{
			CachedActiveUsingItemIcon->RemoveFromParent();
		}
		
		InItemWidget->SetUsingItemProperties();
		CachedUsingItemWidget = InItemWidget;

		checkf(UsingItemIconClass, TEXT("UsingItemIconClass is invalid in [%hs] please fill it."),
			__FUNCTION__);
		CachedActiveUsingItemIcon = CreateWidget<UObsidianDraggedItem_Simple>(World, UsingItemIconClass);
		CachedActiveUsingItemIcon->InitializeDraggedItem(InItemWidget->GetItemImage(), InUsingInstance->GetItemGridSpan());
		CachedActiveUsingItemIcon->AddToViewport();

		CachedUsingItemInstance = InUsingInstance;
	}
	else
	{
		if(CachedActiveUsingItemIcon)
		{
			CachedActiveUsingItemIcon->RemoveFromParent();
		}

		if(CachedUsingItemWidget)
		{
			CachedUsingItemWidget->ResetUsingItemProperties();
		}

		CachedActiveUsingItemIcon = nullptr;
		CachedUsingItemInstance = nullptr;

		OnStopUsingItemDelegate.Broadcast();
	}

	bUsingItem = InInbUsingItem;
}


