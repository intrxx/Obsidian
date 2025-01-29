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

void AObsidianDroppableItem::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if(World == nullptr)
	{
		return;
	}

	checkf(ItemWorldNameClass, TEXT("ItemWorldNameClass is invalid, please make sure it is set in ObsidianDroppableItem instance."));
	ItemWorldName = CreateWidget<UObsidianItemWorldName>(World, ItemWorldNameClass);
	ItemWorldName->OnItemWorldNameMouseHoverDelegate.AddUObject(this, &ThisClass::OnItemMouseHover);
	ItemWorldName->OnItemWorldNameMouseButtonDownDelegate.AddUObject(this, &ThisClass::OnItemMouseButtonDown);
	InitItemWorldName();
	
	WorldItemNameWidgetComp->SetWidget(ItemWorldName);
	WorldItemNameWidgetComp->InitWidget();
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
	if(IsValid(ItemWorldName))
	{
		ItemWorldName->HandleWorldNameHighlightBegin();
	}
	OnItemMouseHover(true);
}

void AObsidianDroppableItem::StopHighlight()
{
	if(IsValid(ItemWorldName))
	{
		ItemWorldName->HandleWorldNameHighlightEnd();
	}
	OnItemMouseHover(false);
}

void AObsidianDroppableItem::SetupItemAppearanceFromInstance() const
{
	const UObsidianInventoryItemInstance* ItemInstance = GetFirstPickupInstanceFromPickupContent().Item;
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

void AObsidianDroppableItem::SetupItemAppearanceFromDefinition() const
{
	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = GetFirstPickupTemplateFromPickupContent().ItemDef;
	if(!ensureMsgf(ItemDef, TEXT("Item Def is invalid in AObsidianDroppableItem::SetupItemAppearanceFromDefinition, make sure the logic is correct.")))
	{
		return;
	}
	
	const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef);
	if(!ensureMsgf(ItemDefault, TEXT("Failed to extract Default from Item Def in AObsidianDroppableItem::SetupItemAppearanceFromDefinition.")))
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
	
	UE_LOG(LogInventory, Error, TEXT("Item [%s] failed to set the Dropped Mesh in: AObsidianDroppableItem::SetupItemAppearanceFromDefinition."),
		*ItemDefault->GetDebugName());
}

void AObsidianDroppableItem::HandleActorClicked(AActor* AffectedActor, FKey ButtonPressed)
{
	if(ButtonPressed == EKeys::LeftMouseButton)
	{
		const bool bLeftControlDown = FSlateApplication::Get().GetModifierKeys().IsLeftControlDown();
		OnItemMouseButtonDown(bLeftControlDown);
	}
}

void AObsidianDroppableItem::InitItemWorldName() const
{
	if(!IsValid(ItemWorldName))
	{
		UE_LOG(LogInventory, Error, TEXT("Item World Name is invalid in AObsidianDroppableItem::InitItemWorldName."));
		return;
	}

#if !UE_BUILD_SHIPPING
	bool bSuccess = false;
#endif
	if(const TSubclassOf<UObsidianInventoryItemDefinition> PickupItemDef = GetFirstPickupTemplateFromPickupContent().ItemDef)
	{
		if(const UObsidianInventoryItemDefinition* DefaultItem = PickupItemDef.GetDefaultObject())
		{
			if(const UOInventoryItemFragment_Appearance* Appearance = Cast<UOInventoryItemFragment_Appearance>(DefaultItem->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
			{
				const FText ItemDisplayName = Appearance->GetItemDisplayName();
				ItemWorldName->SetItemName(ItemDisplayName);
#if !UE_BUILD_SHIPPING
				bSuccess = true;
#endif
			}
		}
	}
	else if(const UObsidianInventoryItemInstance* ItemInstance = GetFirstPickupInstanceFromPickupContent().Item)
    {
         const FText ItemDisplayName = ItemInstance->GetItemDisplayName();
         ItemWorldName->SetItemName(ItemDisplayName);
#if !UE_BUILD_SHIPPING
		bSuccess = true;
#endif
    }

#if !UE_BUILD_SHIPPING
	if(!bSuccess)
	{
		UE_LOG(LogInventory, Error, TEXT("Failed to setup Item World Name in AObsidianDroppableItem::InitItemWorldName."));
	}
#endif
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
		const FPickupTemplate PickupTemplate = GetFirstPickupTemplateFromPickupContent();
		ItemDescriptionToReturn = InventoryController->CreateItemDescriptionForDroppedItem(PickupTemplate.ItemDef, PickupTemplate.StackCount);
	}
	else if(CarriesItemInstance())
	{
		ItemDescriptionToReturn = InventoryController->CreateItemDescriptionForDroppedItem(GetFirstPickupInstanceFromPickupContent().Item);
	}
#if !UE_BUILD_SHIPPING
	else
	{
		ensureMsgf(ItemDescriptionToReturn, TEXT("Returned ItemDescription is invalid in AObsidianDroppableItem::CreateItemDescription(), verify if the Dropped Item contains any Pickup Content."));
	}
#endif
	
	return ItemDescriptionToReturn;
}

void AObsidianDroppableItem::UpdateStacksOnActiveItemDescription(const UObsidianInventoryItemInstance* ItemInstance) const
{
	if(ActiveItemDescription)
	{
		int32 CurrentStacks = 0;
		if(!ensureMsgf(ItemInstance, TEXT("Item Instance is invalid in AObsidianDroppableItem::UpdateStacksOnActiveItemDescription, stacks were set to 0.")))
		{
			CurrentStacks = ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
		}
		ActiveItemDescription->UpdateCurrentStackCount(CurrentStacks);
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

bool AObsidianDroppableItem::PickupItemInstance(const bool bLeftControlDown) const
{
	AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if(!ensureMsgf(ObsidianPC, TEXT("Failed to acquire valid ObsidianPlayerController in AObsidianDroppableItem::PickupItemInstance.")))
	{
		return false;
	}
	
	AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD();
	if(!ensureMsgf(ObsidianHUD, TEXT("Failed to acquire valid ObsidianHUD in AObsidianDroppableItem::PickupItemInstance.")))
	{
		return false;
	}

	UObsidianInventoryItemInstance* ItemInstance = GetFirstPickupInstanceFromPickupContent().Item;
	checkf(ItemInstance, TEXT("First ItemInstance from Pickable Content is invalid in AObsidianDroppableItem::PickupItemInstance"));
	
	const AActor* OwningActor = Cast<AActor>(ObsidianPC->GetPawn());
	checkf(OwningActor, TEXT("OwningActor acquired from ObsidianPC is invalid in AObsidianDroppableItem::PickupItemInstance."))
	
	UObsidianHeroComponent* HeroComp = UObsidianHeroComponent::FindHeroComponent(OwningActor);
	checkf(HeroComp, TEXT("HeroComp acquired from OwningActor is invalid in AObsidianDroppableItem::PickupItemInstance."));
	
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
			checkf(DraggedItemWidgetClass, TEXT("DraggedItemWidgetClass is invalid in AObsidianDroppableItem::PickupItemInstance please fill it on ObsidianDroppableItem Instance."));
			UObsidianDraggedItem* DraggedItem = CreateWidget<UObsidianDraggedItem>(ObsidianPC, DraggedItemWidgetClass);
			DraggedItem->InitializeItemWidgetWithItemInstance(ItemInstance);
			DraggedItem->AddToViewport();
			HeroComp->DragItem(DraggedItem);
			return true; // Added whole Item
		}
		return false; // Added some Item stacks
	}
	
	int32 OutStacksLeft = 0;
	UObsidianInventoryComponent* InventoryComponent = ObsidianPC->GetInventoryComponent();
	checkf(InventoryComponent, TEXT("InventoryComponent acquired from ObsidianPC is invalid in AObsidianDroppableItem::PickupItemInstance."));
	
	InventoryComponent->AddItemInstance(ItemInstance, /** OUT */ OutStacksLeft);
			
	if(OutStacksLeft > 0)
	{
		ItemInstance->OverrideItemStackCount(ObsidianGameplayTags::Item_StackCount_Current, OutStacksLeft);
		UpdateStacksOnActiveItemDescription(OutStacksLeft);
		return false; // Added some Item stacks
	}
	return true; // Added whole Item
}

bool AObsidianDroppableItem::PickupItemDef(const bool bLeftControlDown)
{
	AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if(!ensureMsgf(ObsidianPC, TEXT("Failed to acquire valid ObsidianPlayerController in AObsidianDroppableItem::PickupItemDef.")))
	{
		return false;
	}

	AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD();
	if(!ensureMsgf(ObsidianHUD, TEXT("Failed to acquire valid ObsidianHUD in AObsidianDroppableItem::PickupItemDef.")))
	{
		return false;
	}

	const FPickupTemplate PickupTemplate = GetFirstPickupTemplateFromPickupContent();
	const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = PickupTemplate.ItemDef;
	checkf(ItemDef, TEXT("First ItemInstance from Pickable Content is invalid in AObsidianDroppableItem::PickupItemDef"));
	
	const int32 StackCount = PickupTemplate.StackCount;

	const AActor* OwningActor = Cast<AActor>(ObsidianPC->GetPawn());
	checkf(OwningActor, TEXT("OwningActor acquired from ObsidianPC is invalid in AObsidianDroppableItem::PickupItemDef."))
	
	UObsidianHeroComponent* HeroComp = UObsidianHeroComponent::FindHeroComponent(OwningActor);
	checkf(HeroComp, TEXT("HeroComp acquired from OwningActor is invalid in AObsidianDroppableItem::PickupItemDef."));
	
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
			checkf(DraggedItemWidgetClass, TEXT("DraggedItemWidgetClass is invalid in AObsidianDroppableItem::PickupItemInstance please fill it on ObsidianDroppableItem Instance."));
			UObsidianDraggedItem* DraggedItem = CreateWidget<UObsidianDraggedItem>(ObsidianPC, DraggedItemWidgetClass);
			DraggedItem->InitializeItemWidgetWithItemDef(ItemDef, StackCount);
			DraggedItem->AddToViewport();
			HeroComp->DragItem(DraggedItem);
			return true; // Added whole Item
		}
		return false; // Added some Item stacks
	}
	
	UObsidianInventoryComponent* InventoryComponent = ObsidianPC->GetInventoryComponent();
	checkf(InventoryComponent, TEXT("InventoryComponent acquired from ObsidianPC is invalid in AObsidianDroppableItem::PickupItemDef."));

	int32 OutStacksLeft = 0;
	InventoryComponent->AddItemDefinition(ItemDef, /** OUT */ OutStacksLeft, StackCount);
	if(OutStacksLeft > 0)
	{
		OverrideTemplateStacks(OutStacksLeft);
		UpdateStacksOnActiveItemDescription(OutStacksLeft);
		return false; // Added some Item stacks
	}
	return true; // Added whole Item
}

void AObsidianDroppableItem::DestroyItemDescription()
{
	if(ActiveItemDescription)
	{
		ActiveItemDescription->DestroyDescriptionWidget();
		ActiveItemDescription = nullptr;
	}
}


