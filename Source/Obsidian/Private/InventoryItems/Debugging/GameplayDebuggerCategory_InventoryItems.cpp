// Copyright 2024 out of sCope team - Michał Ogiński


#include "InventoryItems/Debugging/GameplayDebuggerCategory_InventoryItems.h"

#include "Engine/Canvas.h"
#include "InventoryItems/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"

#if WITH_GAMEPLAY_DEBUGGER_MENU

namespace InventoryItems::Debug
{
	// This is the longest name we can use for the UI (string format truncate with %.35s).  We use a variety of letters because MeasureString depends on kerning.
	const FString LongestDebugObjectName{ TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZ_ ABCDEFGH") };
}

FGameplayDebuggerCategory_InventoryItems::FGameplayDebuggerCategory_InventoryItems()
{
	SetDataPackReplication<FRepData>(&DataPack);
}

TSharedRef<FGameplayDebuggerCategory> FGameplayDebuggerCategory_InventoryItems::MakeInstance()
{
	return MakeShareable(new FGameplayDebuggerCategory_InventoryItems());
}

void FGameplayDebuggerCategory_InventoryItems::CollectData(APlayerController* OwnerPC, AActor* DebugActor)
{
	DataPack.Items.Empty();
	
	if(const UObsidianInventoryComponent* InventoryComponent = OwnerPC->FindComponentByClass<UObsidianInventoryComponent>())
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
	}
}

void FGameplayDebuggerCategory_InventoryItems::DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext)
{
	if (LastDrawDataEndSize <= 0.0f)
	{
		LastDrawDataEndSize = CanvasContext.Canvas->SizeY - CanvasContext.CursorY - CanvasContext.CursorX;
	}

	const float ThisDrawDataStartPos = CanvasContext.CursorY;

	constexpr FLinearColor BackgroundColor(0.1f, 0.1f, 0.1f, 0.7f);
	const FVector2D BackgroundPos{CanvasContext.CursorX, CanvasContext.CursorY};
	const FVector2D BackgroundSize{CanvasContext.Canvas->SizeX -  (2.0f * CanvasContext.CursorX), LastDrawDataEndSize};

	FCanvasTileItem Background(FVector2D(0.0f), BackgroundSize, BackgroundColor);
	Background.BlendMode = SE_BLEND_Translucent;
	
	CanvasContext.DrawItem(Background, BackgroundPos.X, BackgroundPos.Y);

	DrawItems(OwnerPC, CanvasContext);

	LastDrawDataEndSize = CanvasContext.CursorY - ThisDrawDataStartPos;
}

void FGameplayDebuggerCategory_InventoryItems::DrawItems(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext) const
{
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
	if (ObjNameSize <= 0.0f)
	{
		float TempSizeY = 0.0f;

		// We have to actually use representative strings because of the kerning
		CanvasContext.MeasureString(*InventoryItems::Debug::LongestDebugObjectName, ObjNameSize, TempSizeY);
		CanvasContext.MeasureString(*InventoryItems::Debug::LongestDebugObjectName, ItemNameSize, TempSizeY);
		CanvasContext.MeasureString(TEXT("current stack count: 00"), CurrentStackCountNameSize, TempSizeY);
		CanvasContext.MeasureString(TEXT("max stack count: 00"), MaxStackCountSize, TempSizeY);
		CanvasContext.MeasureString(TEXT("limit stack count: 00"), LimitStackCountSize, TempSizeY);
		CanvasContext.MeasureString(TEXT("grid size: 00"), GridSizeSize, TempSizeY);
		CanvasContext.MeasureString(TEXT("grid location: 00"), CurrentGridLocationSize, TempSizeY);
		ObjNameSize += Padding;
	}


	const float ColumnWidth = ObjNameSize * 5 + ItemNameSize + CurrentStackCountNameSize + MaxStackCountSize + LimitStackCountSize + GridSizeSize + CurrentGridLocationSize;
	const int NumColumns = FMath::Max(1, FMath::FloorToInt(CanvasWidth / ColumnWidth));

	CanvasContext.Print(TEXT("Inventory Items:"));
	CanvasContext.CursorX += 200.0f;
	CanvasContext.CursorY -= CanvasContext.GetLineHeight();
	CanvasContext.Printf(TEXT("Owned Items Count: {yellow}[%d]"), DataPack.Items.Num());

	CanvasContext.CursorX += Padding;
	for (const FRepData::FInventoryItemDebug& ItemData : DataPack.Items)
	{
		float CursorX = CanvasContext.CursorX;
		float CursorY = CanvasContext.CursorY;

		// Print positions manually to align them properly
		CanvasContext.PrintAt(CursorX + ObjNameSize * 0, CursorY, FColor::Cyan, ItemData.Name.Left(35));
		CanvasContext.PrintAt(CursorX + ObjNameSize * 0.7, CursorY, FColor::Emerald, ItemData.Item);
		CanvasContext.PrintAt(CursorX + ObjNameSize * 0.5 + ItemNameSize, CursorY, FString::Printf(TEXT("{grey}current stack count: {white}%02d"), ItemData.CurrentStackCount));
		CanvasContext.PrintAt(CursorX + ObjNameSize * 0.9 + ItemNameSize + CurrentStackCountNameSize, CursorY, FString::Printf(TEXT("{grey}max stack count: {white}%02d"), ItemData.MaxStackCount));
		CanvasContext.PrintAt(CursorX + ObjNameSize * 1.3 + ItemNameSize + CurrentStackCountNameSize + MaxStackCountSize, CursorY, FString::Printf(TEXT("{grey}limit stack count: {white}%02d"), ItemData.LimitStackCount));
		CanvasContext.PrintAt(CursorX + ObjNameSize * 1.7 + ItemNameSize +  CurrentStackCountNameSize + MaxStackCountSize + LimitStackCountSize, CursorY, FString::Printf(TEXT("{grey}grid size: {white}[%f, %f]"), ItemData.GridSize.X, ItemData.GridSize.Y));
		CanvasContext.PrintAt(CursorX + ObjNameSize * 2.4 + ItemNameSize + CurrentStackCountNameSize + MaxStackCountSize + LimitStackCountSize + GridSizeSize, CursorY, FString::Printf(TEXT("{grey}grid location: {white}[%f, %f]"), ItemData.CurrentGridLocation.X, ItemData.CurrentGridLocation.Y));

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

	// End the category with a newline to separate from the other categories
	CanvasContext.MoveToNewLine();
}

void FGameplayDebuggerCategory_InventoryItems::FRepData::Serialize(FArchive& Ar)
{
	int32 NumItems = Items.Num();
	Ar << NumItems;
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
