// Copyright 2024 out of sCope team - Michał Ogiński


#include "Gameplay/InventoryItems/ObsidianDroppableItem.h"
#include "CharacterComponents/ObsidianHeroComponent.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "Components/WidgetComponent.h"
#include "Core/ObsidianUIFunctionLibrary.h"
#include "InventoryItems/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "UI/Inventory/ObsidianDraggedItem.h"
#include "Kismet/GameplayStatics.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "UI/Inventory/ObsidianItemWorldName.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "UI/ObsidianHUD.h"
#include "UI/Inventory/ObsidianItemDescriptionBase.h"
#include "UI/WidgetControllers/ObsidianInventoryWidgetController.h"

AObsidianDroppableItem::AObsidianDroppableItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	StaticMeshComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	StaticMeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	StaticMeshComp->SetCollisionResponseToChannel(Obsidian_TraceChannel_PlayerCursorTrace, ECR_Block);
	StaticMeshComp->SetCustomDepthStencilValue(ObsidianHighlight::White);
	StaticMeshComp->SetRenderCustomDepth(false);
	StaticMeshComp->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));
	SetRootComponent(StaticMeshComp);
	
	WorldItemNameWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WorldItemNameWidgetComp"));
	WorldItemNameWidgetComp->SetupAttachment(GetRootComponent());
	WorldItemNameWidgetComp->SetRelativeLocation(FVector(0.0f, 50.0f, 0.0f));
	WorldItemNameWidgetComp->SetDrawAtDesiredSize(true);
	WorldItemNameWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	WorldItemNameWidgetComp->SetupAttachment(StaticMeshComp);

	OnClicked.AddDynamic(this, &ThisClass::HandleActorClicked);
}

void AObsidianDroppableItem::AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd)
{
	Super::AddItemInstance(InstanceToAdd);

	SetupItemAppearanceFromInstance();
}

void AObsidianDroppableItem::AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 ItemStacks)
{
	Super::AddItemDefinition(ItemDef, ItemStacks);

	SetupItemAppearanceFromDefinition();
}

AActor* AObsidianDroppableItem::GetHighlightAvatarActor()
{
	return this;
}

void AObsidianDroppableItem::StartHighlight()
{
	if(IsValid(GroundItemDesc))
	{
		GroundItemDesc->HandleWorldNameHighlightBegin();
	}
	OnItemMouseHover(true);
}

void AObsidianDroppableItem::StopHighlight()
{
	if(IsValid(GroundItemDesc))
	{
		GroundItemDesc->HandleWorldNameHighlightEnd();
	}
	OnItemMouseHover(false);
}

void AObsidianDroppableItem::SetupItemAppearanceFromInstance()
{
	const UObsidianInventoryItemInstance* ItemInstance = GetFirstItemInstanceFromPickupContent().Item;
	if(ItemInstance == nullptr)
	{
		return;
	}

	UStaticMesh* DroppedMesh = ItemInstance->GetItemDroppedMesh();
	if(DroppedMesh == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Item [%s] failed to set the Dropped Mesh in: AObsidianDroppableItem::SetupItemAppearanceFromDefinition"),
			*ItemInstance->GetItemDebugName());
		return;
	}

	StaticMeshComp->SetStaticMesh(DroppedMesh);
}

void AObsidianDroppableItem::SetupItemAppearanceFromDefinition()
{
	const FPickupTemplate PickupTemplate = GetFirstItemDefFromPickupContent();
	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = PickupTemplate.ItemDef;
	if(ItemDef == nullptr)
	{
		return;
	}
	
	const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef);
	if(ItemDefault == nullptr)
	{
		return;
	}
	
	if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
	{
		if(UStaticMesh* DroppedMesh = AppearanceFrag->GetItemDroppedMesh())
		{
			StaticMeshComp->SetStaticMesh(DroppedMesh);
			return;
		}
	}
	
	UE_LOG(LogInventory, Error, TEXT("Item [%s] failed to set the Dropped Mesh in: AObsidianDroppableItem::SetupItemAppearanceFromDefinition"),
		*ItemDefault->GetDebugName());
}

void AObsidianDroppableItem::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}
	
	GroundItemDesc = CreateWidget<UObsidianItemWorldName>(World, GroundItemDescClass);
	GroundItemDesc->OnItemWorldNameMouseHoverDelegate.AddUObject(this, &ThisClass::OnItemMouseHover);
	GroundItemDesc->OnItemWorldNameMouseButtonDownDelegate.AddUObject(this, &ThisClass::OnItemMouseButtonDown);
	InitItemDesc();
	
	WorldItemNameWidgetComp->SetWidget(GroundItemDesc);
	WorldItemNameWidgetComp->InitWidget();
}

void AObsidianDroppableItem::HandleActorClicked(AActor* AffectedActor, FKey ButtonPressed)
{
	if(ButtonPressed == EKeys::LeftMouseButton)
	{
		const bool bLeftControlDown = FSlateApplication::Get().GetModifierKeys().IsLeftControlDown();
		OnItemMouseButtonDown(bLeftControlDown);
	}
}

void AObsidianDroppableItem::InitItemDesc() const
{
	if(!IsValid(GroundItemDesc))
	{
		return;
	}
	
	if(const TSubclassOf<UObsidianInventoryItemDefinition> PickupItemDef = GetFirstItemDefFromPickupContent().ItemDef)
	{
		if(const UObsidianInventoryItemDefinition* DefaultItem = PickupItemDef.GetDefaultObject())
		{
			const UOInventoryItemFragment_Appearance* Appearance = Cast<UOInventoryItemFragment_Appearance>(DefaultItem->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass()));

			const FText ItemDisplayName = Appearance->GetItemDisplayName();
			GroundItemDesc->SetItemName(ItemDisplayName);
		}
	}
	else if(const UObsidianInventoryItemInstance* ItemInstance = GetFirstItemInstanceFromPickupContent().Item)
    {
         const FText ItemDisplayName = ItemInstance->GetItemDisplayName();
         GroundItemDesc->SetItemName(ItemDisplayName);
    }
}

void AObsidianDroppableItem::OnItemMouseHover(const bool bMouseEnter)
{
	if(StaticMeshComp)
	{
		StaticMeshComp->SetRenderCustomDepth(bMouseEnter);
	}

	if(bMouseEnter)
	{
		ActiveItemDescription = CreateItemDescription();
	}
	else
	{
		DestroyItemDescription();
	}
}

UObsidianItemDescriptionBase* AObsidianDroppableItem::CreateItemDescription()
{
	DestroyItemDescription();
	
	UObsidianInventoryWidgetController* InventoryController = UObsidianUIFunctionLibrary::GetInventoryWidgetController(this);
	if(InventoryController == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Unable to get InventoryController in AObsidianDroppableItem::CreateItemDescription."));
		return nullptr;	
	}
	
	UObsidianItemDescriptionBase* ItemDescriptionToReturn = nullptr; 
	if(CarriesItemDef())
	{
		const FPickupTemplate PickupTemplate = GetFirstItemDefFromPickupContent();
		ItemDescriptionToReturn = InventoryController->CreateItemDescriptionForDroppedItem(PickupTemplate.ItemDef, PickupTemplate.StackCount);
	}
	else if(CarriesItemInstance())
	{
		const FPickupInstance PickupInstance = GetFirstItemInstanceFromPickupContent();
		ItemDescriptionToReturn = InventoryController->CreateItemDescriptionForDroppedItem(PickupInstance.Item);
	}
#if !UE_BUILD_SHIPPING
	else
	{
		ensureMsgf(ItemDescriptionToReturn, TEXT("Returned ItemDescription is invalid in AObsidianDroppableItem::CreateItemDescription(), verify if the Dropped Item contains any Pickup Content."));
	}
#endif
	
	return ItemDescriptionToReturn;
}

void AObsidianDroppableItem::DestroyItemDescription()
{
	if(ActiveItemDescription)
	{
		ActiveItemDescription->DestroyDescriptionWidget();
		ActiveItemDescription = nullptr;
	}
}

void AObsidianDroppableItem::UpdateStacksOnActiveItemDescription(const UObsidianInventoryItemInstance* ItemInstance) const
{
	if(ActiveItemDescription)
	{
		if(ensureMsgf(ItemInstance, TEXT("Item Instance is invalid in AObsidianDroppableItem::UpdateStacksOnActiveItemDescription")))
		{
			const int32 CurrentStacks = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
			ActiveItemDescription->UpdateCurrentStackCount(CurrentStacks);
		}
	}
}

void AObsidianDroppableItem::UpdateStacksOnActiveItemDescription(const int32 StacksToSet) const
{
	if(ActiveItemDescription)
	{
		ActiveItemDescription->UpdateCurrentStackCount(StacksToSet);
	}
}

void AObsidianDroppableItem::OnItemMouseButtonDown(const bool bLeftControlDown)
{
	bool bAddedWholeItem = true;
	if(CarriesItemDef())
	{
		bAddedWholeItem = PickupItemDef(bLeftControlDown);
	}
	else if(CarriesItemInstance())
	{
		bAddedWholeItem = PickupItemInstance(bLeftControlDown);
	}
	
	if(bAddedWholeItem)
	{
		DestroyItemDescription();
		Destroy();
	}
}

bool AObsidianDroppableItem::PickupItemInstance(const bool bLeftControlDown)
{
	AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if(ObsidianPC == nullptr)
	{
		return false;
	}
	AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD();
	if(ObsidianHUD == nullptr)
	{
		return false;
	}

	UObsidianInventoryItemInstance* ItemInstance = GetFirstItemInstanceFromPickupContent().Item;
	if(ItemInstance == nullptr)
	{
		return false;
	}
	
	const AActor* OwningActor = Cast<AActor>(ObsidianPC->GetPawn());
	UObsidianHeroComponent* HeroComp = UObsidianHeroComponent::FindHeroComponent(OwningActor);
	check(HeroComp);
	
	const bool bIsDraggingAnItem = HeroComp->IsDraggingAnItem();
	if(ObsidianHUD->IsInventoryOpened() && !bLeftControlDown) // If the inventory is opened, and we don't press the left control button spawn the item (with its whole stacks) on cursor.
	{
		bool bDroppedItem = false;
		if(bIsDraggingAnItem)
		{
			bDroppedItem = HeroComp->DropItem();
		}

		if((!bIsDraggingAnItem) || (bIsDraggingAnItem && bDroppedItem))
		{
			UObsidianDraggedItem* DraggedItem = CreateWidget<UObsidianDraggedItem>(ObsidianPC, DraggedItemWidgetClass);
			DraggedItem->InitializeItemWidgetWithItemInstance(ItemInstance);
			DraggedItem->AddToViewport();
			HeroComp->DragItem(DraggedItem);
			return true;
		}
		return false;
	}
	
	int32 OutStacksLeft = 0;
	UObsidianInventoryComponent* InventoryComponent = ObsidianPC->GetInventoryComponent();
	InventoryComponent->AddItemInstance(ItemInstance, /** OUT */ OutStacksLeft);
			
	if(OutStacksLeft > 0)
	{
		ItemInstance->OverrideItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, OutStacksLeft);
		UpdateStacksOnActiveItemDescription(OutStacksLeft);
		return false;
	}
	return true;
}

bool AObsidianDroppableItem::PickupItemDef(const bool bLeftControlDown)
{
	AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if(ObsidianPC == nullptr)
	{
		return false;
	}

	AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD();
	if(ObsidianHUD == nullptr)
	{
		return false;
	}

	const FPickupTemplate PickupTemplate = GetFirstItemDefFromPickupContent();
	const TSubclassOf<UObsidianInventoryItemDefinition> PickupItemDef = PickupTemplate.ItemDef;
	const int32 StackCount = PickupTemplate.StackCount;

	const AActor* OwningActor = Cast<AActor>(ObsidianPC->GetPawn());
	UObsidianHeroComponent* HeroComp = UObsidianHeroComponent::FindHeroComponent(OwningActor);
	check(HeroComp);
	const bool bIsDraggingAnItem = HeroComp->IsDraggingAnItem();
			
	if(ObsidianHUD->IsInventoryOpened() && !bLeftControlDown) // If the inventory is opened, and we don't press left control button, spawn the item on cursor
	{
		bool bDroppedItem = false;
		if(bIsDraggingAnItem)
		{
			bDroppedItem = HeroComp->DropItem();
		}

		if((!bIsDraggingAnItem) || (bIsDraggingAnItem && bDroppedItem))
		{
			UObsidianDraggedItem* DraggedItem = CreateWidget<UObsidianDraggedItem>(ObsidianPC, DraggedItemWidgetClass);
			DraggedItem->InitializeItemWidgetWithItemDef(PickupItemDef, StackCount);
			DraggedItem->AddToViewport();
			HeroComp->DragItem(DraggedItem);
			return true;
		}
		return false;
	}
	
	UObsidianInventoryComponent* InventoryComponent = ObsidianPC->GetInventoryComponent();

	int32 OutStacksLeft = 0;
	InventoryComponent->AddItemDefinition(PickupItemDef, /** OUT */ OutStacksLeft, StackCount);
	if(OutStacksLeft > 0)
	{
		OverrideTemplateStacks(OutStacksLeft);
		UpdateStacksOnActiveItemDescription(OutStacksLeft);
		return false;
	}
	return true;
}


