// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Debugging/GameplayDebuggerCategory_PlayerStash.h"

// ~ Core
#if WITH_GAMEPLAY_DEBUGGER_MENU
#include "Engine/Canvas.h"

// ~ Project
#include "CanvasItem.h"
#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "UI/ObsidianHUD.h"

namespace InventoryItems::Debug
{
	// This is the longest name we can use for the UI (string format truncate with %.35s).  We use a variety of letters because MeasureString depends on kerning.
	const FString LongestDebugObjectName{ TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZ_ ABCDEFGH") };

	constexpr FLinearColor BackgroundColor(0.1f, 0.1f, 0.1f, 0.7f);

	// constexpr FLinearColor TakenColor(1.0f, 0.0f, 0.0f);
	// constexpr FLinearColor FreeColor(0.0f, 1.0f, 0.0f);
	// const FVector2D StateMapTileSize(50.0f, 50.0f);
}

FGameplayDebuggerCategory_PlayerStash::FGameplayDebuggerCategory_PlayerStash()
{
	SetDataPackReplication<FRepData>(&DataPack);
}

TSharedRef<FGameplayDebuggerCategory> FGameplayDebuggerCategory_PlayerStash::MakeInstance()
{
	return MakeShareable(new FGameplayDebuggerCategory_PlayerStash());
}

void FGameplayDebuggerCategory_PlayerStash::CollectData(APlayerController* OwnerPC, AActor* DebugActor)
{
	DataPack.Items.Empty();
	DataPack.Grid.Reset();
	DataPack.Slots.Empty();
	
	if(const AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(OwnerPC))
	{
		if(const AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD())
		{
			DataPack.bStashActive = ObsidianHUD->IsPlayerStashOpened();
			DataPack.StashTabTag = ObsidianHUD->GetActiveStashTabTag();
		}
	}
	
	if(DataPack.bStashActive == false)
	{
		return;
	}
	
	if(UObsidianPlayerStashComponent* PlayerStashComponent = OwnerPC->FindComponentByClass<UObsidianPlayerStashComponent>())
	{
		TArray<UObsidianInventoryItemInstance*> Items = PlayerStashComponent->GetAllItemsFromStashTab(DataPack.StashTabTag);
		for(const UObsidianInventoryItemInstance* Item : Items)
		{
			FRepData::FStashedItemsDebug InventoryItems;
			
			InventoryItems.Name = Item->GetItemDebugName();
			InventoryItems.Item = GetNameSafe(Item->GetItemDef());
			InventoryItems.Item.RemoveFromEnd(TEXT("_C"));
			InventoryItems.CurrentStackCount = Item->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
			InventoryItems.MaxStackCount = Item->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Max);
			InventoryItems.LimitStackCount = Item->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Limit);
			InventoryItems.GridSpan = Item->GetItemGridSpan();
			InventoryItems.CurrentGridLocation = Item->GetItemCurrentPosition().GetItemGridLocation(false);
			InventoryItems.CurrentSlotTag = Item->GetItemCurrentPosition().GetItemSlotTag(false);
			
			DataPack.Items.Add(InventoryItems);
		}
		//DataPack.InventoryStateMap = InventoryComponent->GetGridStateMap();
	}
}

void FGameplayDebuggerCategory_PlayerStash::DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext)
{
	if (LastDrawDataEndSize <= 0.0f)
	{
		LastDrawDataEndSize = CanvasContext.Canvas->SizeY - CanvasContext.CursorY - CanvasContext.CursorX;
	}

	const float ThisDrawDataStartPos = CanvasContext.CursorY;
	
	const FVector2D BackgroundPos{CanvasContext.CursorX, CanvasContext.CursorY};
	const FVector2D BackgroundSize{CanvasContext.Canvas->SizeX -  (2.0f * CanvasContext.CursorX), LastDrawDataEndSize};

	FCanvasTileItem Background(FVector2D(0.0f), BackgroundSize, InventoryItems::Debug::BackgroundColor);
	Background.BlendMode = SE_BLEND_Translucent;
	
	CanvasContext.DrawItem(Background, BackgroundPos.X, BackgroundPos.Y);

	DrawItems(OwnerPC, CanvasContext);

	LastDrawDataEndSize = CanvasContext.CursorY - ThisDrawDataStartPos;
}

void FGameplayDebuggerCategory_PlayerStash::DrawItems(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext) const
{
	using namespace InventoryItems::Debug;
	
	const float CanvasWidth = CanvasContext.Canvas->SizeX;
	Algo::Sort(DataPack.Items, [](const FRepData::FStashedItemsDebug& ItemOne, const FRepData::FStashedItemsDebug& ItemTwo) { return ItemOne.Name < ItemTwo.Name; });

	constexpr float Padding = 10.0f;
	static float ObjNameSize = 0.0f;
	static float ItemNameSize = 0.0f;
	static float CurrentStackCountNameSize = 0.0f;
	static float MaxStackCountSize = 0.0f;
	static float LimitStackCountSize = 0.0f;
	static float GridSpanSize = 0.0f;
	static float CurrentGridLocationSize = 0.0f;
	static float NoItemsTextSize = 0.0f;
	if(ObjNameSize <= 0.0f)
	{
		float TempSizeY = 0.0f;

		// We have to actually use representative strings because of the kerning
		CanvasContext.MeasureString(*LongestDebugObjectName, ObjNameSize, TempSizeY);
		CanvasContext.MeasureString(*LongestDebugObjectName, ItemNameSize, TempSizeY);
		CanvasContext.MeasureString(TEXT("current stack count: 00"), CurrentStackCountNameSize, TempSizeY);
		CanvasContext.MeasureString(TEXT("max stack count: 00"), MaxStackCountSize, TempSizeY);
		CanvasContext.MeasureString(TEXT("limit stack count: 00"), LimitStackCountSize, TempSizeY);
		CanvasContext.MeasureString(TEXT("grid size: 00"), GridSpanSize, TempSizeY);
		CanvasContext.MeasureString(TEXT("grid location: 00"), CurrentGridLocationSize, TempSizeY);
		CanvasContext.MeasureString(TEXT("Player Stash is Closed, to view items open it."), NoItemsTextSize, TempSizeY);
		ObjNameSize += Padding;
	}
	const float SecondArgConstX = ObjNameSize * 0.7;
	const float ThirdArgConstX = ObjNameSize * 0.5 + ItemNameSize;
	const float FourthArgConstX = ObjNameSize * 0.9 + ItemNameSize + CurrentStackCountNameSize;
	const float FifthArgConstX = ObjNameSize * 1.3 + ItemNameSize + CurrentStackCountNameSize + MaxStackCountSize;
	const float SixthArgConstX = ObjNameSize * 1.7 + ItemNameSize +  CurrentStackCountNameSize + MaxStackCountSize + LimitStackCountSize;
	const float SeventhArgConstX = ObjNameSize * 2.4 + ItemNameSize + CurrentStackCountNameSize + MaxStackCountSize + LimitStackCountSize + GridSpanSize;


	const float ColumnWidth = ObjNameSize * 5 + ItemNameSize + CurrentStackCountNameSize + MaxStackCountSize + LimitStackCountSize + GridSpanSize + CurrentGridLocationSize;
	const int NumColumns = FMath::Max(1, FMath::FloorToInt(CanvasWidth / ColumnWidth));

	if (DataPack.bStashActive == false)
	{
		CanvasContext.PrintAt((CanvasWidth / 2) - (NoItemsTextSize / 2), CanvasContext.Canvas->SizeY / 2,
			FString::Printf(TEXT("{red}Player Stash is Closed, to view items open it.")));
		return;
	}

	float TopCursorY = CanvasContext.CursorY;
	float TopCursorX = CanvasContext.CursorX;
	CanvasContext.PrintAt(TopCursorX, TopCursorY, FString::Printf(TEXT("Stashed Items:")));
	TopCursorX += 300.0f;

	const int32 ItemsNum = DataPack.Items.Num();
	CanvasContext.PrintAt(TopCursorX, TopCursorY, FString::Printf(TEXT("Owned Items Count: {yellow}%d"), ItemsNum));
	TopCursorX += 300.0f;

	CanvasContext.PrintAt(TopCursorX, TopCursorY, FString::Printf(TEXT("Stash Gameplay Tag: {yellow}%s"), *DataPack.StashTabTag.GetTagName().ToString()));
	CanvasContext.MoveToNewLine();
	
	CanvasContext.MoveToNewLine();
	TopCursorX = CanvasContext.CursorX;
	TopCursorY = CanvasContext.CursorY;
	
	CanvasContext.PrintAt(TopCursorX, TopCursorY, FString::Printf(TEXT("Item Debug Name:")));
	CanvasContext.PrintAt(TopCursorX + SecondArgConstX, TopCursorY, FString::Printf(TEXT("Item Definition Class:")));
	CanvasContext.PrintAt(TopCursorX + ThirdArgConstX, TopCursorY, FString::Printf(TEXT("Current Item Stack Count:")));
	CanvasContext.PrintAt(TopCursorX + FourthArgConstX, TopCursorY, FString::Printf(TEXT("Max Item Stack Count:")));
	CanvasContext.PrintAt(TopCursorX + FifthArgConstX, TopCursorY, FString::Printf(TEXT("Item Stack Count Inventory Limit:")));
	CanvasContext.PrintAt(TopCursorX + SixthArgConstX, TopCursorY, FString::Printf(TEXT("Item Grid Size:")));
	CanvasContext.PrintAt(TopCursorX + SeventhArgConstX, TopCursorY, FString::Printf(TEXT("Item Location:")));

	CanvasContext.MoveToNewLine();
	CanvasContext.CursorX += Padding;
	for(const FRepData::FStashedItemsDebug& ItemData : DataPack.Items)
	{
		float CursorX = CanvasContext.CursorX;
		float CursorY = CanvasContext.CursorY;

		// Print positions manually to align them properly
		CanvasContext.PrintAt(CursorX, CursorY, FColor::Cyan, ItemData.Name.Left(35));
		CanvasContext.PrintAt(CursorX + SecondArgConstX, CursorY, FColor::Emerald, ItemData.Item);
		CanvasContext.PrintAt(CursorX + ThirdArgConstX, CursorY, FString::Printf(TEXT("{grey}Count: {yellow}%d"), ItemData.CurrentStackCount));
		CanvasContext.PrintAt(CursorX + FourthArgConstX, CursorY, FString::Printf(TEXT("{grey}Count: {yellow}%d"), ItemData.MaxStackCount));
		CanvasContext.PrintAt(CursorX + FifthArgConstX, CursorY, FString::Printf(TEXT("{grey}Count: {yellow}%d"), ItemData.LimitStackCount));
		CanvasContext.PrintAt(CursorX + SixthArgConstX, CursorY, FString::Printf(TEXT("{grey}Size: {yellow}[%d, %d]"), ItemData.GridSpan.X, ItemData.GridSpan.Y));
		if(ItemData.CurrentGridLocation != FIntPoint::NoneValue)
		{
			CanvasContext.PrintAt(CursorX + SeventhArgConstX, CursorY, FString::Printf(TEXT("{grey}Location: {yellow}[%d, %d]"), ItemData.CurrentGridLocation.X, ItemData.CurrentGridLocation.Y));
		}
		else if (ItemData.CurrentSlotTag != FGameplayTag::EmptyTag)
		{
			CanvasContext.PrintAt(CursorX + SeventhArgConstX, CursorY, FString::Printf(TEXT("{grey}Slot Tag: {yellow}%s"), *ItemData.CurrentSlotTag.GetTagName().ToString()));
		}
		
		// PrintAt would have reset these values, restore them.
		CanvasContext.CursorX = CursorX + (CanvasWidth / NumColumns);
		CanvasContext.CursorY = CursorY;

		// If we're going to overflow, go to the next line...
		if (CanvasContext.CursorX + ColumnWidth >= CanvasWidth)
		{
			CanvasContext.MoveToNewLine();
			CanvasContext.CursorX += Padding;
		}
	}

	// End the row with a newline
	if (CanvasContext.CursorX != CanvasContext.DefaultX)
	{
		CanvasContext.MoveToNewLine();
	}
}

void FGameplayDebuggerCategory_PlayerStash::FRepData::Serialize(FArchive& Ar)
{
	int32 NumItems = Items.Num();
	Ar << NumItems;
	int32 NumSlots = Slots.Num();
	Ar << NumSlots;
	
	if(Ar.IsLoading())
	{
		Items.SetNum(NumItems);
		Slots.SetNum(NumSlots);
	}

	for(int32 i = 0; i < NumItems; i++)
	{
		Ar << Items[i].Name;
		Ar << Items[i].Item;
		Ar << Items[i].CurrentStackCount;
		Ar << Items[i].MaxStackCount;
		Ar << Items[i].LimitStackCount;
		Ar << Items[i].GridSpan;
		Ar << Items[i].CurrentGridLocation;
		Ar << Items[i].CurrentSlotTag;
	}

	for(int32 i = 0; i < NumSlots; i++)
	{
		Ar << Slots[i].bUsed;
		Ar << Slots[i].SlotTag;
		Ar << Slots[i].AcceptedTags;
		Ar << Slots[i].BannedTags;
	}

	Ar << bStashActive;
	Ar << StashTabTag;
	
	Ar << Grid.bUsed;
	Ar << Grid.GridStateMap;
}

#endif // WITH_GAMEPLAY_DEBUGGER_MENU
