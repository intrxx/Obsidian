// Copyright 2024 out of sCope team - Michał Ogiński


#include "InventoryItems/Debugging/GameplayDebuggerCategory_InventoryItems.h"
#include "Engine/Canvas.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"

#if WITH_GAMEPLAY_DEBUGGER_MENU

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

void FGameplayDebuggerCategory_InventoryItems:: CollectData(APlayerController* OwnerPC, AActor* DebugActor)
{
	DataPack.Items.Empty();
	
	if(UObsidianInventoryComponent* InventoryComponent = OwnerPC->FindComponentByClass<UObsidianInventoryComponent>())
	{
		TArray<UObsidianInventoryItemInstance*> Items = InventoryComponent->GetAllItems();
		for(UObsidianInventoryItemInstance* Item : Items)
		{
			FRepData::FInventoryItemDebug InventoryItems;
			
			InventoryItems.Name = Item->GetItemDebugName();
			InventoryItems.Item = GetNameSafe(Item->GetItemDef());
			InventoryItems.Item.RemoveFromEnd(TEXT("_C"));
			InventoryItems.CurrentStackCount = Item->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
			InventoryItems.MaxStackCount = Item->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Max);
			InventoryItems.LimitStackCount = Item->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Limit);
			InventoryItems.GridSize = Item->GetItemGridSpan();
			InventoryItems.CurrentGridLocation = Item->GetItemCurrentGridLocation();

			DataPack.Items.Add(InventoryItems);
		}
		DataPack.InventoryStateMap = InventoryComponent->Internal_GetInventoryStateMap();
	}
}

void FGameplayDebuggerCategory_InventoryItems::DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext)
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

void FGameplayDebuggerCategory_InventoryItems::DrawItems(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext) const
{
	using namespace InventoryItems::Debug;
	
	const float CanvasWidth = CanvasContext.Canvas->SizeX;
	Algo::Sort(DataPack.Items, [](const FRepData::FInventoryItemDebug& ItemOne, const FRepData::FInventoryItemDebug& ItemTwo) { return ItemOne.Name < ItemTwo.Name; });

	constexpr float Padding = 10.0f;
	static float ObjNameSize = 0.0f;
	static float ItemNameSize = 0.0f;
	static float CurrentStackCountNameSize = 0.0f;
	static float MaxStackCountSize = 0.0f;
	static float LimitStackCountSize = 0.0f;
	static float GridSizeSize = 0.0f;
	static float CurrentGridLocationSize = 0.0f;
	if(ObjNameSize <= 0.0f)
	{
		float TempSizeY = 0.0f;

		// We have to actually use representative strings because of the kerning
		CanvasContext.MeasureString(*LongestDebugObjectName, ObjNameSize, TempSizeY);
		CanvasContext.MeasureString(*LongestDebugObjectName, ItemNameSize, TempSizeY);
		CanvasContext.MeasureString(TEXT("current stack count: 00"), CurrentStackCountNameSize, TempSizeY);
		CanvasContext.MeasureString(TEXT("max stack count: 00"), MaxStackCountSize, TempSizeY);
		CanvasContext.MeasureString(TEXT("limit stack count: 00"), LimitStackCountSize, TempSizeY);
		CanvasContext.MeasureString(TEXT("grid size: 00"), GridSizeSize, TempSizeY);
		CanvasContext.MeasureString(TEXT("grid location: 00"), CurrentGridLocationSize, TempSizeY);
		ObjNameSize += Padding;
	}
	const float SecondArgConstX = ObjNameSize * 0.7;
	const float ThirdArgConstX = ObjNameSize * 0.5 + ItemNameSize;
	const float FourthArgConstX = ObjNameSize * 0.9 + ItemNameSize + CurrentStackCountNameSize;
	const float FifthArgConstX = ObjNameSize * 1.3 + ItemNameSize + CurrentStackCountNameSize + MaxStackCountSize;
	const float SixthArgConstX = ObjNameSize * 1.7 + ItemNameSize +  CurrentStackCountNameSize + MaxStackCountSize + LimitStackCountSize;
	const float SeventhArgConstX = ObjNameSize * 2.4 + ItemNameSize + CurrentStackCountNameSize + MaxStackCountSize + LimitStackCountSize + GridSizeSize;


	const float ColumnWidth = ObjNameSize * 5 + ItemNameSize + CurrentStackCountNameSize + MaxStackCountSize + LimitStackCountSize + GridSizeSize + CurrentGridLocationSize;
	const int NumColumns = FMath::Max(1, FMath::FloorToInt(CanvasWidth / ColumnWidth));

	float TopCursorY = CanvasContext.CursorY;
	float TopCursorX = CanvasContext.CursorX;
	CanvasContext.PrintAt(TopCursorX, TopCursorY, FString::Printf(TEXT("Inventory Items:")));
	TopCursorX += 300.0f;

	const int32 ItemsNum = DataPack.Items.Num();
	CanvasContext.PrintAt(TopCursorX, TopCursorY, FString::Printf(TEXT("Owned Items Count: {yellow}%d"), ItemsNum));
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
	CanvasContext.PrintAt(TopCursorX + SeventhArgConstX, TopCursorY, FString::Printf(TEXT("Item Origin Location On The Grid:")));

	CanvasContext.MoveToNewLine();
	CanvasContext.CursorX += Padding;
	for(const FRepData::FInventoryItemDebug& ItemData : DataPack.Items)
	{
		float CursorX = CanvasContext.CursorX;
		float CursorY = CanvasContext.CursorY;

		// Print positions manually to align them properly
		CanvasContext.PrintAt(CursorX, CursorY, FColor::Cyan, ItemData.Name.Left(35));
		CanvasContext.PrintAt(CursorX + SecondArgConstX, CursorY, FColor::Emerald, ItemData.Item);
		CanvasContext.PrintAt(CursorX + ThirdArgConstX, CursorY, FString::Printf(TEXT("{grey}Count: {yellow}%d"), ItemData.CurrentStackCount));
		CanvasContext.PrintAt(CursorX + FourthArgConstX, CursorY, FString::Printf(TEXT("{grey}Count: {yellow}%d"), ItemData.MaxStackCount));
		CanvasContext.PrintAt(CursorX + FifthArgConstX, CursorY, FString::Printf(TEXT("{grey}Count: {yellow}%d"), ItemData.LimitStackCount));
		CanvasContext.PrintAt(CursorX + SixthArgConstX, CursorY, FString::Printf(TEXT("{grey}Size: {yellow}[%lld, %lld]"), FMath::FloorToInt(ItemData.GridSize.X), FMath::FloorToInt(ItemData.GridSize.Y)));
		CanvasContext.PrintAt(CursorX + SeventhArgConstX, CursorY, FString::Printf(TEXT("{grey}Location: {yellow}[%lld, %lld]"), FMath::FloorToInt(ItemData.CurrentGridLocation.X), FMath::FloorToInt(ItemData.CurrentGridLocation.Y)));

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

	// End the category with a newline to separate
	CanvasContext.MoveToNewLine();
	CanvasContext.Print(TEXT("Inventory State Map:"));
	CanvasContext.MoveToNewLine();

	float TileX = Padding * 2;
	float TileY = CanvasContext.CursorY;
	constexpr float TilePadding = 5.0f;
	
	int32 CurrentRow = 1;
	for(TTuple<FVector2D, bool> Pair : DataPack.InventoryStateMap)
	{
		if(CurrentRow == Pair.Key.Y)
		{
			TileY = CanvasContext.CursorY + (StateMapTileSize.Y + TilePadding) * CurrentRow;
			TileX = Padding * 2;
			CurrentRow++;
		}
		
		if(Pair.Value == true)
		{
			FCanvasTileItem TakenField = {FVector2D(TileX, TileY), StateMapTileSize, TakenColor};
			CanvasContext.DrawItem(TakenField, TileX, TileY);
			CanvasContext.PrintAt(TileX + 5.0f, TileY + 5.0f, FString::Printf(TEXT("[%lld, %lld]"), FMath::FloorToInt(Pair.Key.X), FMath::FloorToInt(Pair.Key.Y)));
		}
		else
		{
			FCanvasTileItem FreeField = {FVector2D(TileX, TileY), StateMapTileSize, FreeColor};
			CanvasContext.DrawItem(FreeField, TileX, TileY);
			CanvasContext.PrintAt(TileX + 5.0f, TileY + 5.0f, FString::Printf(TEXT("[%lld, %lld]"), FMath::FloorToInt(Pair.Key.X), FMath::FloorToInt(Pair.Key.Y)));
		}
		
		TileX += StateMapTileSize.X + TilePadding;
	}
	CanvasContext.CursorY = CanvasContext.CursorY + (StateMapTileSize.Y + TilePadding) * CurrentRow;
	CanvasContext.CursorX = Padding;
	CanvasContext.PrintAt(CanvasContext.CursorX, CanvasContext.CursorY, FString::Printf(TEXT("{grey}Taken fields are painted red, free fields are green.")));
	CanvasContext.MoveToNewLine();
}

void FGameplayDebuggerCategory_InventoryItems::FRepData::Serialize(FArchive& Ar)
{
	int32 NumItems = Items.Num();
	Ar << NumItems;
	Ar << InventoryStateMap;
	
	if(Ar.IsLoading())
	{
		Items.SetNum(NumItems);
	}

	for(int32 i = 0; i < NumItems; i++)
	{
		Ar << Items[i].Name;
		Ar << Items[i].Item;
		Ar << Items[i].CurrentStackCount;
		Ar << Items[i].MaxStackCount;
		Ar << Items[i].LimitStackCount;
		Ar << Items[i].GridSize;
		Ar << Items[i].CurrentGridLocation;
	}
}

#endif // WITH_GAMEPLAY_DEBUGGER_MENU
