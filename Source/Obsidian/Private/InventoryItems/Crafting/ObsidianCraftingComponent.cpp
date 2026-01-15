// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Crafting/ObsidianCraftingComponent.h"

#include <Blueprint/WidgetLayoutLibrary.h>
#include <Engine/ActorChannel.h>
#include <GameFramework/GameplayMessageSubsystem.h>

#include "UI/Inventory/Items/ObsidianDraggedItem_Simple.h"
#include "UI/Inventory/Items/ObsidianItem.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"

DEFINE_LOG_CATEGORY(LogCrafting)

UObsidianCraftingComponent::UObsidianCraftingComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
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

void UObsidianCraftingComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

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
		UE_LOG(LogCrafting, Error, TEXT("Controller is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const AActor* OwningActor = Cast<AActor>(Controller->GetPawn());
	if (OwningActor == nullptr)
	{
		UE_LOG(LogCrafting, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(OwningActor->GetWorld());
	MessageSubsystem.RegisterListener(ObsidianGameplayTags::Message_Inventory_Changed, this,
		&ThisClass::OnInventoryStateChanged);
	MessageSubsystem.RegisterListener(ObsidianGameplayTags::Message_PlayerStash_Changed, this,
		&ThisClass::OnPlayerStashChanged);
}

void UObsidianCraftingComponent::OnInventoryStateChanged(FGameplayTag Channel,
	const FObsidianInventoryChangeMessage& InventoryChangeMessage)
{
	if (GetOwner() != InventoryChangeMessage.InventoryOwner->GetOwner())
	{
		return;
	}

	const UObsidianInventoryItemInstance* Instance = InventoryChangeMessage.ItemInstance;
	if(Instance == nullptr)
	{
		UE_LOG(LogCrafting, Error, TEXT("Item Instance is invalid in [%hs]"), __FUNCTION__);
		return;
	}
	
	if(InventoryChangeMessage.ChangeType == EObsidianInventoryChangeType::ICT_ItemRemoved)
	{
		UE_LOG(LogCrafting, Display, TEXT("Stopping Usage of item: [%s] from Inventory, due to removal"),
			*Instance->GetItemDisplayName().ToString());

		if(bUsingItem && Instance == CachedUsingItemInstance)
		{
			SetUsingItem(false);
		}
	}
}

void UObsidianCraftingComponent::OnPlayerStashChanged(FGameplayTag Channel,
	const FObsidianStashChangeMessage& StashChangeMessage)
{
	if (GetOwner() != StashChangeMessage.PlayerStashOwner->GetOwner())
	{
		return;
	}
	
	const UObsidianInventoryItemInstance* Instance = StashChangeMessage.ItemInstance;
	if(Instance == nullptr)
	{
		UE_LOG(LogCrafting, Error, TEXT("Item Instance is invalid in [%hs]"), __FUNCTION__);
		return;
	}
	
	if(StashChangeMessage.ChangeType == EObsidianStashChangeType::ICT_ItemRemoved)
	{
		UE_LOG(LogCrafting, Display, TEXT("Stopping Usage of item: [%s] from Player Stash, due to removal"),
			*Instance->GetItemDisplayName().ToString());
		
		if(bUsingItem && Instance == CachedUsingItemInstance)
		{
			SetUsingItem(false);
		}
	}
}

bool UObsidianCraftingComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch,
                                                     FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if(CachedUsingItemInstance && IsValid(CachedUsingItemInstance))
	{
		WroteSomething |= Channel->ReplicateSubobject(CachedUsingItemInstance, *Bunch, *RepFlags);
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

void UObsidianCraftingComponent::UseItem(const FObsidianItemPosition& OnPosition, const bool bLeftShiftDown)
{
	ServerUseItem(CachedUsingItemInstance, OnPosition);

	if(bLeftShiftDown == false)
	{
		SetUsingItem(false);
	}
}

void UObsidianCraftingComponent::ServerActivateUsableItemFromInventory_Implementation(
	UObsidianInventoryItemInstance* UsingInstance)
{
	if(UsingInstance == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("UsingInstance is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
	if(InventoryComponent == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	InventoryComponent->UseItem(UsingInstance, nullptr);
}

void UObsidianCraftingComponent::ServerActivateUsableItemFromStash_Implementation(
	UObsidianInventoryItemInstance* UsingInstance)
{
	if(UsingInstance == nullptr)
	{
		UE_LOG(LogCrafting, Error, TEXT("UsingInstance is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(LogCrafting, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
	if(PlayerStashComponent == nullptr)
	{
		UE_LOG(LogCrafting, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	PlayerStashComponent->UseItem(UsingInstance, nullptr);
}

void UObsidianCraftingComponent::ServerUseItem_Implementation(UObsidianInventoryItemInstance* UsingInstance,
	const FObsidianItemPosition& OnPosition)
{
	if(UsingInstance == nullptr)
	{
		UE_LOG(LogCrafting, Error, TEXT("UsingInstance is null in [%hs]"), __FUNCTION__);
		return;
	}
	
	const AController* Controller = Cast<AController>(GetOwner());
	if(Controller == nullptr)
	{
		UE_LOG(LogCrafting, Error, TEXT("OwningActor is null in [%hs]"), __FUNCTION__);
		return;
	}

	const FGameplayTag StashTabTag = OnPosition.GetOwningStashTabTag();
	if (StashTabTag != FGameplayTag::EmptyTag)
	{
		UObsidianPlayerStashComponent* PlayerStashComponent = Controller->FindComponentByClass<UObsidianPlayerStashComponent>();
		if(PlayerStashComponent == nullptr)
		{
			UE_LOG(LogPlayerStash, Error, TEXT("PlayerStashComponent is null in [%hs]"), __FUNCTION__);
			return;
		}

		UObsidianInventoryItemInstance* UsingOntoInstance = PlayerStashComponent->GetItemInstanceFromTabAtPosition(OnPosition);
		PlayerStashComponent->UseItem(UsingInstance, UsingOntoInstance);
	}
	else
	{
		UObsidianInventoryComponent* InventoryComponent = Controller->FindComponentByClass<UObsidianInventoryComponent>();
		if(InventoryComponent == nullptr)
		{
			UE_LOG(LogCrafting, Error, TEXT("InventoryComponent is null in [%hs]"), __FUNCTION__);
			return;
		}

		UObsidianInventoryItemInstance* UsingOntoInstance = InventoryComponent->GetItemInstanceAtLocation(OnPosition.GetItemGridPosition());
		InventoryComponent->UseItem(UsingInstance, UsingOntoInstance);
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

void UObsidianCraftingComponent::SetUsingItem(const bool InbUsingItem, UObsidianItem* ItemWidget,
                                              UObsidianInventoryItemInstance* UsingInstance)
{
	if(InbUsingItem && ItemWidget)
	{
		if (UsingInstance == nullptr)
		{
			UE_LOG(LogCrafting, Error, TEXT("UsingInstance is invalid in [%hs]."), __FUNCTION__);
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
		
		ItemWidget->SetUsingItemProperties();
		CachedUsingItemWidget = ItemWidget;

		checkf(UsingItemIconClass, TEXT("UsingItemIconClass is invalid in [%hs] please fill it."),
			__FUNCTION__);
		CachedActiveUsingItemIcon = CreateWidget<UObsidianDraggedItem_Simple>(World, UsingItemIconClass);
		CachedActiveUsingItemIcon->InitializeDraggedItem(ItemWidget->GetItemImage(), UsingInstance->GetItemGridSpan());
		CachedActiveUsingItemIcon->AddToViewport();

		CachedUsingItemInstance = UsingInstance;
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

	bUsingItem = InbUsingItem;
}


