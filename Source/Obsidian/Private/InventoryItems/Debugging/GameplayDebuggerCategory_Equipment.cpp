// Copyright 2024 out of sCope team - Michał Ogiński


#include "InventoryItems/Debugging/GameplayDebuggerCategory_Equipment.h"

#if WITH_GAMEPLAY_DEBUGGER_MENU

#include "Engine/Canvas.h"
#include "CanvasItem.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"
#include "InventoryItems/Equipment/ObsidianSpawnedEquipmentPiece.h"

namespace EquipmentItems::Debug
{
	// This is the longest name we can use for the UI (string format truncate with %.35s).  We use a variety of letters because MeasureString depends on kerning.
	const FString LongestDebugObjectName{ TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZ_ ABCDEFGH") };

	constexpr FLinearColor BackgroundColor(0.1f, 0.1f, 0.1f, 0.7f);
}

FGameplayDebuggerCategory_Equipment::FGameplayDebuggerCategory_Equipment()
{
	SetDataPackReplication<FRepData>(&DataPack);
}

TSharedRef<FGameplayDebuggerCategory> FGameplayDebuggerCategory_Equipment::MakeInstance()
{
	return MakeShareable(new FGameplayDebuggerCategory_Equipment());
}

void FGameplayDebuggerCategory_Equipment::CollectData(APlayerController* OwnerPC, AActor* DebugActor)
{
	DataPack.Items.Empty();
	
	if(const UObsidianEquipmentComponent* EquipmentComponent = OwnerPC->FindComponentByClass<UObsidianEquipmentComponent>())
	{
		TArray<UObsidianInventoryItemInstance*> Items = EquipmentComponent->GetAllEquippedItems();
		for(const UObsidianInventoryItemInstance* Item : Items)
		{
			FRepData::FEquipmentItemDebug EquipmentItems;
			
			EquipmentItems.Name = Item->GetItemDebugName();
			EquipmentItems.Item = GetNameSafe(Item->GetItemDef());
			EquipmentItems.Item.RemoveFromEnd(TEXT("_C"));
			EquipmentItems.SlotTag = Item->GetItemCurrentEquipmentSlot();
			for(const AObsidianSpawnedEquipmentPiece* Piece : Item->GetSpawnedActors())
			{
				EquipmentItems.SpawnedEquipmentPieces.Add(GetNameSafe(Piece));
			}
			for(const UObsidianAbilitySet* Set : Item->GetOwningAbilitySets())
			{
				EquipmentItems.OwnedAbilitySets.Add(GetNameSafe(Set));
			}
			
			DataPack.Items.Add(EquipmentItems);
		}
	}
}

void FGameplayDebuggerCategory_Equipment::DrawData(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext)
{
	if (LastDrawDataEndSize <= 0.0f)
	{
		LastDrawDataEndSize = CanvasContext.Canvas->SizeY - CanvasContext.CursorY - CanvasContext.CursorX;
	}

	const float ThisDrawDataStartPos = CanvasContext.CursorY;
	
	const FVector2D BackgroundPos{CanvasContext.CursorX, CanvasContext.CursorY};
	const FVector2D BackgroundSize{CanvasContext.Canvas->SizeX -  (2.0f * CanvasContext.CursorX), LastDrawDataEndSize};

	FCanvasTileItem Background(FVector2D(0.0f), BackgroundSize, EquipmentItems::Debug::BackgroundColor);
	Background.BlendMode = SE_BLEND_Translucent;
	
	CanvasContext.DrawItem(Background, BackgroundPos.X, BackgroundPos.Y);

	DrawItems(OwnerPC, CanvasContext);

	LastDrawDataEndSize = CanvasContext.CursorY - ThisDrawDataStartPos;
}

void FGameplayDebuggerCategory_Equipment::DrawItems(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext) const
{
	using namespace EquipmentItems::Debug;
	
	const float CanvasWidth = CanvasContext.Canvas->SizeX;
	Algo::Sort(DataPack.Items, [](const FRepData::FEquipmentItemDebug& ItemOne, const FRepData::FEquipmentItemDebug& ItemTwo) { return ItemOne.Name < ItemTwo.Name; });

	constexpr float Padding = 10.0f;
	static float ObjNameSize = 0.0f;
	static float ItemNameSize = 0.0f;
	static float CurrentSlotTagNameSize = 0.0f;
	static float SpawnedActorsTagNameSize = 0.0f;
	static float OwningAbilitySets = 0.0f;
	if(ObjNameSize <= 0.0f)
	{
		float TempSizeY = 0.0f;

		// We have to actually use representative strings because of the kerning
		CanvasContext.MeasureString(*LongestDebugObjectName, ObjNameSize, TempSizeY);
		CanvasContext.MeasureString(*LongestDebugObjectName, ItemNameSize, TempSizeY);
		CanvasContext.MeasureString(*LongestDebugObjectName, CurrentSlotTagNameSize, TempSizeY);
		CanvasContext.MeasureString(*LongestDebugObjectName, SpawnedActorsTagNameSize, TempSizeY);
		CanvasContext.MeasureString(*LongestDebugObjectName, OwningAbilitySets, TempSizeY);
		ObjNameSize += Padding;
	}
	const float SecondArgConstX = ObjNameSize * 0.7;
	const float ThirdArgConstX = ObjNameSize * 0.5 + ItemNameSize;
	const float ForthArgConstX = ObjNameSize * 0.9 + ItemNameSize + SpawnedActorsTagNameSize;
	const float FifthArgConstX = ObjNameSize * 1.3 + ItemNameSize + SpawnedActorsTagNameSize + OwningAbilitySets;
	
	const float ColumnWidth = ObjNameSize * 5 + ItemNameSize + CurrentSlotTagNameSize;
	const int NumColumns = FMath::Max(1, FMath::FloorToInt(CanvasWidth / ColumnWidth));

	float TopCursorY = CanvasContext.CursorY;
	float TopCursorX = CanvasContext.CursorX;
	CanvasContext.PrintAt(TopCursorX, TopCursorY, FString::Printf(TEXT("Equipment Items:")));
	TopCursorX += 300.0f;

	const int32 ItemsNum = DataPack.Items.Num();
	CanvasContext.PrintAt(TopCursorX, TopCursorY, FString::Printf(TEXT("Equipped Items Count: {yellow}%d"), ItemsNum));
	CanvasContext.MoveToNewLine();
	
	CanvasContext.MoveToNewLine();
	TopCursorX = CanvasContext.CursorX;
	TopCursorY = CanvasContext.CursorY;
	
	CanvasContext.PrintAt(TopCursorX, TopCursorY, FString::Printf(TEXT("Item Debug Name:")));
	CanvasContext.PrintAt(TopCursorX + SecondArgConstX, TopCursorY, FString::Printf(TEXT("Item Definition Class:")));
	CanvasContext.PrintAt(TopCursorX + ThirdArgConstX, TopCursorY, FString::Printf(TEXT("Current Item Slot:")));
	CanvasContext.PrintAt(TopCursorX + ForthArgConstX, TopCursorY, FString::Printf(TEXT("Spawned Actors:")));
	CanvasContext.PrintAt(TopCursorX + FifthArgConstX, TopCursorY, FString::Printf(TEXT("Owning Ability Sets:")));

	CanvasContext.MoveToNewLine();
	CanvasContext.CursorX += Padding;
	for(const FRepData::FEquipmentItemDebug& ItemData : DataPack.Items)
	{
		float CursorX = CanvasContext.CursorX;
		float CursorY = CanvasContext.CursorY;

		// Print positions manually to align them properly
		CanvasContext.PrintAt(CursorX, CursorY, FColor::Cyan, ItemData.Name.Left(35));
		CanvasContext.PrintAt(CursorX + SecondArgConstX, CursorY, FColor::Emerald, ItemData.Item);
		CanvasContext.PrintAt(CursorX + ThirdArgConstX, CursorY, FString::Printf(TEXT("{grey}Slot Tag: {yellow}%s"), *ItemData.SlotTag.GetTagName().ToString()));

		float CachedCursorY = CursorY;
		for(FString ItemName : ItemData.SpawnedEquipmentPieces)
		{
			ItemName.RemoveFromEnd(TEXT("_C"));
			CanvasContext.PrintAt(CursorX + ForthArgConstX, CachedCursorY, FString::Printf(TEXT("{grey}Actor: {yellow}%s"), *ItemName));
			CachedCursorY += CanvasContext.GetLineHeight(); 
		}

		CachedCursorY = CursorY;
		for(FString SetName : ItemData.OwnedAbilitySets)
		{
			SetName.RemoveFromEnd(TEXT("_C"));
			CanvasContext.PrintAt(CursorX + FifthArgConstX, CachedCursorY, FString::Printf(TEXT("{grey}Ability Set: {yellow}%s"), *SetName));
			CachedCursorY += CanvasContext.GetLineHeight(); 
		}
		
		// PrintAt would have reset these values, restore them.
		CanvasContext.CursorX = CursorX + (CanvasWidth / NumColumns);
		CanvasContext.CursorY = CursorY;

		int32 NumberOfEntriesInLine = FMath::Max<int32>(ItemData.OwnedAbilitySets.Num(), ItemData.SpawnedEquipmentPieces.Num()) - 1;
		for(;;)
		{
			CanvasContext.MoveToNewLine();
			
			NumberOfEntriesInLine--;
			if(NumberOfEntriesInLine <= 0)
			{
				break;
			}
		}

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
}

void FGameplayDebuggerCategory_Equipment::FRepData::Serialize(FArchive& Ar)
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
		Ar << Items[i].SlotTag;
		Ar << Items[i].SpawnedEquipmentPieces;
		Ar << Items[i].OwnedAbilitySets;
	}
}

#endif // WITH_GAMEPLAY_DEBUGGER_MENU
