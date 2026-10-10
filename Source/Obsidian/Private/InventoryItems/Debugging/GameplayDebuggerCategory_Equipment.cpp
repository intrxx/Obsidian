// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Debugging/GameplayDebuggerCategory_Equipment.h"

#if WITH_GAMEPLAY_DEBUGGER_MENU
#include "CanvasItem.h"
#include "Engine/Canvas.h"

#include "InventoryItems/Equipment/ObsidianEquipmentComponent.h"
#include "InventoryItems/Equipment/ObsidianSpawnedEquipmentPiece.h"
#include "InventoryItems/ItemAffixes/ObsidianAffixAbilitySet.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"


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

void FGameplayDebuggerCategory_Equipment::CollectData(APlayerController* InOwnerPC, AActor* InDebugActor)
{
	DataPack.Items.Empty();
	
	if(const UObsidianEquipmentComponent* EquipmentComponent = InOwnerPC->FindComponentByClass<UObsidianEquipmentComponent>())
	{
		TArray<UObsidianInventoryItemInstance*> Items = EquipmentComponent->GetAllEquippedItems();
		for(const UObsidianInventoryItemInstance* Item : Items)
		{
			FRepData::FEquipmentItemDebug EquipmentItem;
			
			EquipmentItem.Name = Item->GetItemDebugName();
			EquipmentItem.ItemUniqueID = Item->GetUniqueItemID().ToString();
			EquipmentItem.Item = GetNameSafe(Item->GetItemDef());
			EquipmentItem.Item.RemoveFromEnd(TEXT("_C"));
			EquipmentItem.SlotTag = Item->GetItemCurrentPosition().GetItemSlotTag();
			for(const AObsidianSpawnedEquipmentPiece* Piece : Item->GetSpawnedActors())
			{
				EquipmentItem.SpawnedEquipmentPieces.Add(GetNameSafe(Piece));
			}
			for(const UObsidianAffixAbilitySet* Set : Item->GetAffixAbilitySetsFromItem())
			{
				EquipmentItem.OwnedAbilitySets.Add(GetNameSafe(Set));
			}
			
			DataPack.Items.Add(EquipmentItem);
		}

		for (const FObsidianEquipmentSlotDefinition Slot : EquipmentComponent->Internal_GetEquipmentSlots())
		{
			FRepData::FEquipmentSlotDebug EquipmentSlot;

			EquipmentSlot.SlotTag = Slot.GetEquipmentSlotTag().GetTagName().ToString();
			EquipmentSlot.SisterSlotTag = Slot.SisterSlotTag.GetTagName().ToString();
			
			for (const FGameplayTag& Tag : Slot.BaseSlotDefinition.AcceptedItemCategories)
			{
				EquipmentSlot.AcceptedTags.Add(Tag.GetTagName().ToString());
			}
			for (const FGameplayTag& Tag : Slot.BaseSlotDefinition.BannedItemCategories)
			{
				EquipmentSlot.BannedTags.Add(Tag.GetTagName().ToString());
			}
			
			DataPack.EquipmentSlots.Add(EquipmentSlot);
		}
	}
}

void FGameplayDebuggerCategory_Equipment::DrawData(APlayerController* InOwnerPC, FGameplayDebuggerCanvasContext& InCanvasContext)
{
	if (LastDrawDataEndSize <= 0.0f)
	{
		LastDrawDataEndSize = InCanvasContext.Canvas->SizeY - InCanvasContext.CursorY - InCanvasContext.CursorX;
	}

	const float ThisDrawDataStartPos = InCanvasContext.CursorY;
	
	const FVector2D BackgroundPos{InCanvasContext.CursorX, InCanvasContext.CursorY};
	const FVector2D BackgroundSize{InCanvasContext.Canvas->SizeX -  (2.0f * InCanvasContext.CursorX), LastDrawDataEndSize};

	FCanvasTileItem Background(FVector2D(0.0f), BackgroundSize, EquipmentItems::Debug::BackgroundColor);
	Background.BlendMode = SE_BLEND_Translucent;
	
	InCanvasContext.DrawItem(Background, BackgroundPos.X, BackgroundPos.Y);

	DrawItems(InOwnerPC, InCanvasContext);

	LastDrawDataEndSize = InCanvasContext.CursorY - ThisDrawDataStartPos;
}

void FGameplayDebuggerCategory_Equipment::DrawItems(APlayerController* InOwnerPC, FGameplayDebuggerCanvasContext& InCanvasContext) const
{
	using namespace EquipmentItems::Debug;
	
	const float CanvasWidth = InCanvasContext.Canvas->SizeX;
	Algo::Sort(DataPack.Items, [](const FRepData::FEquipmentItemDebug& InItemOne, const FRepData::FEquipmentItemDebug& InItemTwo) { return InItemOne.Name < InItemTwo.Name; });

	constexpr float Padding = 10.0f;
	static float ObjNameSize = 0.0f;
	static float ItemNameSize = 0.0f;
	static float UniqueIDSize = 0.0f;
	static float CurrentSlotTagNameSize = 0.0f;
	static float SpawnedActorsTagNameSize = 0.0f;
	static float OwningAbilitySets = 0.0f;
	if(ObjNameSize <= 0.0f)
	{
		float TempSizeY = 0.0f;

		// We have to actually use representative strings because of the kerning
		InCanvasContext.MeasureString(*LongestDebugObjectName, ObjNameSize, TempSizeY);
		InCanvasContext.MeasureString(*LongestDebugObjectName, ItemNameSize, TempSizeY);
		InCanvasContext.MeasureString(*LongestDebugObjectName, UniqueIDSize, TempSizeY);
		InCanvasContext.MeasureString(*LongestDebugObjectName, CurrentSlotTagNameSize, TempSizeY);
		InCanvasContext.MeasureString(*LongestDebugObjectName, SpawnedActorsTagNameSize, TempSizeY);
		InCanvasContext.MeasureString(*LongestDebugObjectName, OwningAbilitySets, TempSizeY);
		ObjNameSize += Padding;
	}
	const float SecondArgConstX = ObjNameSize * 0.9;
	const float ThirdArgConstX = ObjNameSize * 1 + ItemNameSize;
	const float ForthArgConstX = ObjNameSize * 0.95 + ItemNameSize + UniqueIDSize;
	const float FifthArgConstX = ObjNameSize * 1.1 + ItemNameSize + UniqueIDSize + SpawnedActorsTagNameSize;
	const float SixthArgConstX = ObjNameSize * 1.4 + ItemNameSize + UniqueIDSize + SpawnedActorsTagNameSize + OwningAbilitySets;
	
	const float ColumnWidth = ObjNameSize * 5 + ItemNameSize + CurrentSlotTagNameSize;
	const int NumColumns = FMath::Max(1, FMath::FloorToInt(CanvasWidth / ColumnWidth));

	float TopCursorY = InCanvasContext.CursorY;
	float TopCursorX = InCanvasContext.CursorX;
	InCanvasContext.PrintAt(TopCursorX, TopCursorY, FString::Printf(TEXT("Equipment Items:")));
	TopCursorX += 300.0f;

	const int32 ItemsNum = DataPack.Items.Num();
	InCanvasContext.PrintAt(TopCursorX, TopCursorY, FString::Printf(TEXT("Equipped Items Count: {yellow}%d"), ItemsNum));
	InCanvasContext.MoveToNewLine();
	
	InCanvasContext.MoveToNewLine();
	TopCursorX = InCanvasContext.CursorX;
	TopCursorY = InCanvasContext.CursorY;
	
	InCanvasContext.PrintAt(TopCursorX, TopCursorY, FString::Printf(TEXT("Item Debug Name:")));
	InCanvasContext.PrintAt(TopCursorX + SecondArgConstX, TopCursorY, FString::Printf(TEXT("Unique Item ID:")));
	InCanvasContext.PrintAt(TopCursorX + ThirdArgConstX, TopCursorY, FString::Printf(TEXT("Item Definition Class:")));
	InCanvasContext.PrintAt(TopCursorX + ForthArgConstX, TopCursorY, FString::Printf(TEXT("Current Item Slot:")));
	InCanvasContext.PrintAt(TopCursorX + FifthArgConstX, TopCursorY, FString::Printf(TEXT("Spawned Actors:")));
	InCanvasContext.PrintAt(TopCursorX + SixthArgConstX, TopCursorY, FString::Printf(TEXT("Owning Ability Sets:")));

	InCanvasContext.MoveToNewLine();
	InCanvasContext.CursorX += Padding;
	for(const FRepData::FEquipmentItemDebug& ItemData : DataPack.Items)
	{
		float CursorX = InCanvasContext.CursorX;
		float CursorY = InCanvasContext.CursorY;

		// Print positions manually to align them properly
		InCanvasContext.PrintAt(CursorX, CursorY, FColor::Cyan, ItemData.Name.Left(35));
		InCanvasContext.PrintAt(CursorX + SecondArgConstX, CursorY, FColor::Emerald, ItemData.ItemUniqueID);
		InCanvasContext.PrintAt(CursorX + ThirdArgConstX, CursorY, FColor::Emerald, ItemData.Item);
		InCanvasContext.PrintAt(CursorX + ForthArgConstX, CursorY, FString::Printf(TEXT("{grey}Slot Tag: {yellow}%s"), *ItemData.SlotTag.GetTagName().ToString()));

		float CachedCursorY = CursorY;
		for(FString ItemName : ItemData.SpawnedEquipmentPieces)
		{
			ItemName.RemoveFromEnd(TEXT("_C"));
			InCanvasContext.PrintAt(CursorX + FifthArgConstX, CachedCursorY, FString::Printf(TEXT("{grey}Actor: {yellow}%s"), *ItemName));
			CachedCursorY += InCanvasContext.GetLineHeight(); 
		}

		CachedCursorY = CursorY;
		for(FString SetName : ItemData.OwnedAbilitySets)
		{
			SetName.RemoveFromEnd(TEXT("_C"));
			InCanvasContext.PrintAt(CursorX + SixthArgConstX, CachedCursorY, FString::Printf(TEXT("{grey}Ability Set: {yellow}%s"), *SetName));
			CachedCursorY += InCanvasContext.GetLineHeight(); 
		}
		
		// PrintAt would have reset these values, restore them.
		InCanvasContext.CursorX = CursorX + (CanvasWidth / NumColumns);
		InCanvasContext.CursorY = CursorY;

		int32 NumberOfEntriesInLine = FMath::Max<int32>(ItemData.OwnedAbilitySets.Num(), ItemData.SpawnedEquipmentPieces.Num()) - 1;
		for(;;)
		{
			InCanvasContext.MoveToNewLine();
			
			NumberOfEntriesInLine--;
			if(NumberOfEntriesInLine <= 0)
			{
				break;
			}
		}

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
	InCanvasContext.Print(TEXT("Equipment Slots:"));
	InCanvasContext.MoveToNewLine();

	float SecondCategoryTopX = InCanvasContext.CursorX;
	float SecondCategoryTopY = InCanvasContext.CursorY;
	
	InCanvasContext.PrintAt(SecondCategoryTopX, SecondCategoryTopY, FString::Printf(TEXT("Slot Tag:")));
	InCanvasContext.PrintAt(SecondCategoryTopX + SecondArgConstX, SecondCategoryTopY, FString::Printf(TEXT("Slot's Sister Slot Tag:")));
	InCanvasContext.PrintAt(SecondCategoryTopX + ThirdArgConstX, SecondCategoryTopY, FString::Printf(TEXT("Accepted Equipment Categories:")));
	float CustomForthArgConstX = (ThirdArgConstX + CanvasWidth / 3) + 150.0f;
	InCanvasContext.PrintAt(SecondCategoryTopX + CustomForthArgConstX, SecondCategoryTopY, FString::Printf(TEXT("Banned Equipment Categories:")));
	
	InCanvasContext.MoveToNewLine();
	InCanvasContext.CursorX += Padding;

	int32 AcceptedIncreasedLines = 0;
	int32 BannedIncreasedLines = 0;
	for(const FRepData::FEquipmentSlotDebug& EquipmentSlot : DataPack.EquipmentSlots)
	{
		float CursorX = InCanvasContext.CursorX;
		float CursorY = InCanvasContext.CursorY;

		// Print positions manually to align them properly
		InCanvasContext.PrintAt(CursorX, CursorY, FColor::Cyan, EquipmentSlot.SlotTag);
		InCanvasContext.PrintAt(CursorX + SecondArgConstX, CursorY, FColor::Emerald, EquipmentSlot.SisterSlotTag);
		
		float CachedCursorX = CursorX;
		float CachedCursorY = CursorY;
		for(const FString& AcceptedTagString : EquipmentSlot.AcceptedTags)
		{
			float SizeX;
			float SizeY;
			InCanvasContext.MeasureString(AcceptedTagString, SizeX, SizeY);

			if (CachedCursorX + SizeX >= CursorX + CustomForthArgConstX - 500.0f)
			{
				CachedCursorY += InCanvasContext.GetLineHeight();
				CachedCursorX = CursorX;
				AcceptedIncreasedLines++;
			}
			
			InCanvasContext.PrintAt(CachedCursorX + ThirdArgConstX, CachedCursorY, AcceptedTagString);
			CachedCursorX += SizeX + Padding;
		}
		
		CachedCursorX = CursorX;
		CachedCursorY += InCanvasContext.GetLineHeight();
		for(const FString& BannedTagString : EquipmentSlot.BannedTags)
		{
			float SizeX;
			float SizeY;
			InCanvasContext.MeasureString(BannedTagString, SizeX, SizeY);
			
			if (CachedCursorX + SizeX >= CanvasWidth)
			{
				CachedCursorY += InCanvasContext.GetLineHeight();
				CachedCursorX = CursorX;
				BannedIncreasedLines++;
			}
			
			InCanvasContext.PrintAt(CachedCursorX + CustomForthArgConstX, CachedCursorY, BannedTagString);
			CachedCursorX += SizeX + Padding;
		}
		
		// PrintAt would have reset these values, restore them.
		InCanvasContext.CursorX = CursorX + (CanvasWidth / NumColumns);
		InCanvasContext.CursorY = CursorY + InCanvasContext.GetLineHeight();

		int32 NumberOfEntriesInLine = FMath::Max<int32>(AcceptedIncreasedLines, BannedIncreasedLines);
		for(int i = 0; i < NumberOfEntriesInLine; i++)
		{
			InCanvasContext.MoveToNewLine();
		}
		AcceptedIncreasedLines = 0;
		BannedIncreasedLines = 0;
		
		// If we're going to overflow, go to the next line...
		if (InCanvasContext.CursorX + ColumnWidth >= CanvasWidth)
		{
			InCanvasContext.MoveToNewLine();
			InCanvasContext.CursorX += Padding;
		}
	}
}

void FGameplayDebuggerCategory_Equipment::FRepData::Serialize(FArchive& InOutAr)
{
	int32 NumItems = Items.Num();
	InOutAr << NumItems;
	int32 NumSlots = EquipmentSlots.Num();
	InOutAr << NumSlots;
	
	if(InOutAr.IsLoading())
	{
		Items.SetNum(NumItems);
		EquipmentSlots.SetNum(NumSlots);
	}

	for(int32 i = 0; i < NumItems; i++)
	{
		InOutAr << Items[i].Name;
		InOutAr << Items[i].Item;
		InOutAr << Items[i].SlotTag;
		InOutAr << Items[i].SpawnedEquipmentPieces;
		InOutAr << Items[i].OwnedAbilitySets;
	}

	for(int32 i = 0; i < NumSlots; i++)
	{
		InOutAr << EquipmentSlots[i].SlotTag;
		InOutAr << EquipmentSlots[i].SisterSlotTag;
		InOutAr << EquipmentSlots[i].AcceptedTags;
		InOutAr << EquipmentSlots[i].BannedTags;
	}
}

#endif // WITH_GAMEPLAY_DEBUGGER_MENU
