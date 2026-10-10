// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Debugging/GameplayDebuggerCategory_InventoryItems.h"

#if WITH_GAMEPLAY_DEBUGGER_MENU
#include "Engine/Canvas.h"

#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"


namespace InventoryItems::Debug
{
	// This is the longest name we can use for the UI (string format truncate with %.35s).  We use a variety of letters because MeasureString depends on kerning.
	const FString LongestDebugObjectName{ TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZ_ ABCDEFGH") };

	constexpr FLinearColor BackgroundColor(0.1f, 0.1f, 0.1f, 0.7f);

	constexpr FLinearColor TakenColor(1.0f, 0.0f, 0.0f);
	constexpr FLinearColor FreeColor(0.0f, 1.0f, 0.0f);
	const FVector2D StateMapTileSize(50.0f, 50.0f);
}

FGameplayDebuggerCategory_InventoryItems::FGameplayDebuggerCategory_InventoryItems()
{
	SetDataPackReplication<FRepData>(&DataPack);
}

TSharedRef<FGameplayDebuggerCategory> FGameplayDebuggerCategory_InventoryItems::MakeInstance()
{
	return MakeShareable(new FGameplayDebuggerCategory_InventoryItems());
}

void FGameplayDebuggerCategory_InventoryItems:: CollectData(APlayerController* InOwnerPC, AActor* InDebugActor)
{
	DataPack.Items.Empty();
	
	if(UObsidianInventoryComponent* InventoryComponent = InOwnerPC->FindComponentByClass<UObsidianInventoryComponent>())
	{
		TArray<UObsidianInventoryItemInstance*> Items = InventoryComponent->GetAllItems();
		for(const UObsidianInventoryItemInstance* Item : Items)
		{
			FRepData::FInventoryItemDebug InventoryItem;
			
			InventoryItem.Name = Item->GetItemDebugName();
			InventoryItem.ItemUniqueID = Item->GetUniqueItemID().ToString();
			InventoryItem.Item = GetNameSafe(Item->GetItemDef());
			InventoryItem.Item.RemoveFromEnd(TEXT("_C"));
			InventoryItem.CurrentStackCount = Item->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
			InventoryItem.MaxStackCount = Item->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Max);
			InventoryItem.LimitStackCount = Item->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Limit);
			InventoryItem.GridSpan = Item->GetItemGridSpan();
			InventoryItem.CurrentGridLocation = Item->GetItemCurrentPosition().GetItemGridPosition();

			DataPack.Items.Add(InventoryItem);
		}
		DataPack.InventoryStateMap = InventoryComponent->GetGridStateMap();
	}
}

void FGameplayDebuggerCategory_InventoryItems::DrawData(APlayerController* InOwnerPC, FGameplayDebuggerCanvasContext& InCanvasContext)
{
	if (LastDrawDataEndSize <= 0.0f)
	{
		LastDrawDataEndSize = InCanvasContext.Canvas->SizeY - InCanvasContext.CursorY - InCanvasContext.CursorX;
	}

	const float ThisDrawDataStartPos = InCanvasContext.CursorY;
	
	const FVector2D BackgroundPos{InCanvasContext.CursorX, InCanvasContext.CursorY};
	const FVector2D BackgroundSize{InCanvasContext.Canvas->SizeX -  (2.0f * InCanvasContext.CursorX), LastDrawDataEndSize};

	FCanvasTileItem Background(FVector2D(0.0f), BackgroundSize, InventoryItems::Debug::BackgroundColor);
	Background.BlendMode = SE_BLEND_Translucent;
	
	InCanvasContext.DrawItem(Background, BackgroundPos.X, BackgroundPos.Y);

	DrawItems(InOwnerPC, InCanvasContext);

	LastDrawDataEndSize = InCanvasContext.CursorY - ThisDrawDataStartPos;
}

void FGameplayDebuggerCategory_InventoryItems::DrawItems(APlayerController* InOwnerPC, FGameplayDebuggerCanvasContext& InCanvasContext) const
{
	using namespace InventoryItems::Debug;
	
	const float CanvasWidth = InCanvasContext.Canvas->SizeX;
	Algo::Sort(DataPack.Items, [](const FRepData::FInventoryItemDebug& InItemOne, const FRepData::FInventoryItemDebug& InItemTwo) { return InItemOne.Name < InItemTwo.Name; });

	constexpr float Padding = 10.0f;
	static float ObjNameSize = 0.0f;
	static float ItemNameSize = 0.0f;
	static float UniqueIDSize = 0.0f;
	static float CurrentStackCountNameSize = 0.0f;
	static float MaxStackCountSize = 0.0f;
	static float LimitStackCountSize = 0.0f;
	static float GridSpanSize = 0.0f;
	static float CurrentGridLocationSize = 0.0f;
	if(ObjNameSize <= 0.0f)
	{
		float TempSizeY = 0.0f;

		// We have to actually use representative strings because of the kerning
		InCanvasContext.MeasureString(*LongestDebugObjectName, ObjNameSize, TempSizeY);
		InCanvasContext.MeasureString(*LongestDebugObjectName, ItemNameSize, TempSizeY);
		InCanvasContext.MeasureString(*LongestDebugObjectName, UniqueIDSize, TempSizeY);
		InCanvasContext.MeasureString(TEXT("current stack count: 00"), CurrentStackCountNameSize, TempSizeY);
		InCanvasContext.MeasureString(TEXT("max stack count: 00"), MaxStackCountSize, TempSizeY);
		InCanvasContext.MeasureString(TEXT("limit stack count: 00"), LimitStackCountSize, TempSizeY);
		InCanvasContext.MeasureString(TEXT("grid size: 00"), GridSpanSize, TempSizeY);
		InCanvasContext.MeasureString(TEXT("grid location: 00"), CurrentGridLocationSize, TempSizeY);
		ObjNameSize += Padding;
	}
	const float SecondArgConstX = ObjNameSize * 0.7;
	const float ThirdArgConstX = ObjNameSize * 0.8 + ItemNameSize;
	const float FourthArgConstX = ObjNameSize * 0.5 + ItemNameSize + UniqueIDSize;
	const float FifthArgConstX = ObjNameSize * 0.9 + ItemNameSize + UniqueIDSize + CurrentStackCountNameSize;
	const float SixthArgConstX = ObjNameSize * 1.3 + ItemNameSize + UniqueIDSize + CurrentStackCountNameSize + MaxStackCountSize;
	const float SeventhArgConstX = ObjNameSize * 1.7 + ItemNameSize + UniqueIDSize + CurrentStackCountNameSize + MaxStackCountSize + LimitStackCountSize;
	const float EightArgConstX = ObjNameSize * 2.1 + ItemNameSize + UniqueIDSize + CurrentStackCountNameSize + MaxStackCountSize + LimitStackCountSize + GridSpanSize;

	const float ColumnWidth = ObjNameSize * 5 + ItemNameSize + CurrentStackCountNameSize + MaxStackCountSize + LimitStackCountSize + GridSpanSize + CurrentGridLocationSize;
	const int NumColumns = FMath::Max(1, FMath::FloorToInt(CanvasWidth / ColumnWidth));

	float TopCursorY = InCanvasContext.CursorY;
	float TopCursorX = InCanvasContext.CursorX;
	InCanvasContext.PrintAt(TopCursorX, TopCursorY, FString::Printf(TEXT("Inventory Items:")));
	TopCursorX += 300.0f;

	const int32 ItemsNum = DataPack.Items.Num();
	InCanvasContext.PrintAt(TopCursorX, TopCursorY, FString::Printf(TEXT("Owned Items Count: {yellow}%d"), ItemsNum));
	InCanvasContext.MoveToNewLine();
	
	InCanvasContext.MoveToNewLine();
	TopCursorX = InCanvasContext.CursorX;
	TopCursorY = InCanvasContext.CursorY;
	
	InCanvasContext.PrintAt(TopCursorX, TopCursorY, FString::Printf(TEXT("Item Debug Name:")));
	InCanvasContext.PrintAt(TopCursorX + SecondArgConstX, TopCursorY, FString::Printf(TEXT("Unique Item ID:")));
	InCanvasContext.PrintAt(TopCursorX + ThirdArgConstX, TopCursorY, FString::Printf(TEXT("Item Definition Class:")));
	InCanvasContext.PrintAt(TopCursorX + FourthArgConstX, TopCursorY, FString::Printf(TEXT("Current Item Stack Count:")));
	InCanvasContext.PrintAt(TopCursorX + FifthArgConstX, TopCursorY, FString::Printf(TEXT("Max Item Stack Count:")));
	InCanvasContext.PrintAt(TopCursorX + SixthArgConstX, TopCursorY, FString::Printf(TEXT("Item Stack Count Inventory Limit:")));
	InCanvasContext.PrintAt(TopCursorX + SeventhArgConstX, TopCursorY, FString::Printf(TEXT("Item Grid Size:")));
	InCanvasContext.PrintAt(TopCursorX + EightArgConstX, TopCursorY, FString::Printf(TEXT("Item Origin Location On The Grid:")));

	InCanvasContext.MoveToNewLine();
	InCanvasContext.CursorX += Padding;
	for(const FRepData::FInventoryItemDebug& ItemData : DataPack.Items)
	{
		float CursorX = InCanvasContext.CursorX;
		float CursorY = InCanvasContext.CursorY;

		// Print positions manually to align them properly
		InCanvasContext.PrintAt(CursorX, CursorY, FColor::Cyan, ItemData.Name.Left(35));
		InCanvasContext.PrintAt(CursorX + SecondArgConstX, CursorY, FColor::Cyan, ItemData.ItemUniqueID);
		InCanvasContext.PrintAt(CursorX + ThirdArgConstX, CursorY, FColor::Emerald, ItemData.Item);
		InCanvasContext.PrintAt(CursorX + FourthArgConstX, CursorY, FString::Printf(TEXT("{grey}Count: {yellow}%d"), ItemData.CurrentStackCount));
		InCanvasContext.PrintAt(CursorX + FifthArgConstX, CursorY, FString::Printf(TEXT("{grey}Count: {yellow}%d"), ItemData.MaxStackCount));
		InCanvasContext.PrintAt(CursorX + SixthArgConstX, CursorY, FString::Printf(TEXT("{grey}Count: {yellow}%d"), ItemData.LimitStackCount));
		InCanvasContext.PrintAt(CursorX + SeventhArgConstX, CursorY, FString::Printf(TEXT("{grey}Size: {yellow}[%d, %d]"), ItemData.GridSpan.X, ItemData.GridSpan.Y));
		InCanvasContext.PrintAt(CursorX + EightArgConstX, CursorY, FString::Printf(TEXT("{grey}Location: {yellow}[%d, %d]"), ItemData.CurrentGridLocation.X, ItemData.CurrentGridLocation.Y));

		// PrintAt would have reset these values, restore them.
		InCanvasContext.CursorX = CursorX + (CanvasWidth / NumColumns);
		InCanvasContext.CursorY = CursorY;

		// If we're going to overflow, go to the next line...
		if (InCanvasContext.CursorX + ColumnWidth >= CanvasWidth)
		{
			InCanvasContext.MoveToNewLine();
			InCanvasContext.CursorX += Padding;
		}
	}

	// End the row with a newline
	if (InCanvasContext.CursorX != InCanvasContext.DefaultX)
	{
		InCanvasContext.MoveToNewLine();
	}

	// End the category with a newline to separate
	InCanvasContext.MoveToNewLine();
	InCanvasContext.Print(TEXT("Inventory State Map:"));
	InCanvasContext.MoveToNewLine();

	float TileX = Padding * 2;
	float TileY = InCanvasContext.CursorY;
	constexpr float TilePadding = 5.0f;
	
	int32 CurrentRow = 1;
	for(TTuple<FIntPoint, bool> Pair : DataPack.InventoryStateMap)
	{
		if(CurrentRow == Pair.Key.Y)
		{
			TileY = InCanvasContext.CursorY + (StateMapTileSize.Y + TilePadding) * CurrentRow;
			TileX = Padding * 2;
			CurrentRow++;
		}
		
		if(Pair.Value == true)
		{
			FCanvasTileItem TakenField = {FVector2D(TileX, TileY), StateMapTileSize, TakenColor};
			InCanvasContext.DrawItem(TakenField, TileX, TileY);
		}
		else
		{
			FCanvasTileItem FreeField = {FVector2D(TileX, TileY), StateMapTileSize, FreeColor};
			InCanvasContext.DrawItem(FreeField, TileX, TileY);
		}
		
		InCanvasContext.PrintAt(TileX + 5.0f, TileY + 5.0f, FString::Printf(TEXT("[%d, %d]"), Pair.Key.X, Pair.Key.Y));
		TileX += StateMapTileSize.X + TilePadding;
	}
	InCanvasContext.CursorY = InCanvasContext.CursorY + (StateMapTileSize.Y + TilePadding) * CurrentRow;
	InCanvasContext.CursorX = Padding;
	InCanvasContext.PrintAt(InCanvasContext.CursorX, InCanvasContext.CursorY, FString::Printf(TEXT("{grey}Taken fields are painted red, free fields are green.")));
	InCanvasContext.MoveToNewLine();
}

void FGameplayDebuggerCategory_InventoryItems::FRepData::Serialize(FArchive& InOutAr)
{
	int32 NumItems = Items.Num();
	InOutAr << NumItems;
	
	if(InOutAr.IsLoading())
	{
		Items.SetNum(NumItems);
	}

	for(int32 i = 0; i < NumItems; i++)
	{
		InOutAr << Items[i].Name;
		InOutAr << Items[i].Item;
		InOutAr << Items[i].CurrentStackCount;
		InOutAr << Items[i].MaxStackCount;
		InOutAr << Items[i].LimitStackCount;
		InOutAr << Items[i].GridSpan;
		InOutAr << Items[i].CurrentGridLocation;
	}

	InOutAr << InventoryStateMap;
}

#endif // WITH_GAMEPLAY_DEBUGGER_MENU
