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
#include "InventoryItems/PlayerStash/ObsidianStashTab.h"
#include "InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Grid.h"
#include "InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Slots.h"
#include "UI/ObsidianHUD.h"

namespace PlayerStash::Debug
{
	// This is the longest name we can use for the UI (string format truncate with %.35s).  We use a variety of letters because MeasureString depends on kerning.
	const FString LongestDebugObjectName{ TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZ_ ABCDEFGH") };

	constexpr FLinearColor BackgroundColor(0.1f, 0.1f, 0.1f, 0.7f);

	constexpr FLinearColor TakenColor(1.0f, 0.0f, 0.0f);
	constexpr FLinearColor FreeColor(0.0f, 1.0f, 0.0f);
	
	const FVector2D StateMapTileSize_Regular(37.0f);
	const FVector2D StateMapGridNumberFontSizeScale_Regular(0.9f);
	
	const FVector2D StateMapTileSize_Small(27.0f);
	const FVector2D StateMapGridNumberFontSizeScale_Small(0.6f);
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
	DataPack.Grid.GridStateMap.Reset();
	DataPack.Slots.Empty();
	DataPack.StashTabType = EDebugStashTabType::DSTT_None;
	
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
		
		if(UObsidianStashTab* StashTab = PlayerStashComponent->StashItemList.GetStashTabForTag(DataPack.StashTabTag))
		{
			// We are either Grid Stash Tab or Slots Stash Tab
			if(const UObsidianStashTab_Grid* GridStashTab = Cast<UObsidianStashTab_Grid>(StashTab))
			{
				DataPack.StashTabType = EDebugStashTabType::DSTT_Grid;
				DataPack.Grid.GridStateMap = GridStashTab->GridStateMap;
			}
			else if(UObsidianStashTab_Slots* SlotsStashTab = Cast<UObsidianStashTab_Slots>(StashTab))
			{
				DataPack.StashTabType = EDebugStashTabType::DSTT_Slots;
			
				for(const FObsidianSlotDefinition& SlotDef : SlotsStashTab->TabSlots)
				{
					FRepData::FStashSlotsDebug TabSlot;
					TabSlot.SlotTag = SlotDef.SlotTag.GetTagName().ToString();
				
					for(const FGameplayTag& Tag : SlotDef.AcceptedItemCategories)
					{
						TabSlot.AcceptedTags.Add(Tag.GetTagName().ToString());
					}
					for(const FGameplayTag& Tag : SlotDef.BannedItemCategories)
					{
						TabSlot.BannedTags.Add(Tag.GetTagName().ToString());
					}

					DataPack.Slots.Add(TabSlot);
				}
			}
		}
		
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

	FCanvasTileItem Background(FVector2D(0.0f), BackgroundSize, PlayerStash::Debug::BackgroundColor);
	Background.BlendMode = SE_BLEND_Translucent;
	
	CanvasContext.DrawItem(Background, BackgroundPos.X, BackgroundPos.Y);

	DrawItems(OwnerPC, CanvasContext);

	LastDrawDataEndSize = CanvasContext.CursorY - ThisDrawDataStartPos;
}

void FGameplayDebuggerCategory_PlayerStash::DrawItems(APlayerController* OwnerPC, FGameplayDebuggerCanvasContext& CanvasContext) const
{
	using namespace PlayerStash::Debug;
	
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
	for (const FRepData::FStashedItemsDebug& ItemData : DataPack.Items)
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
		if (ItemData.CurrentGridLocation != FIntPoint::NoneValue)
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

	// End the category with a newline to separate
	CanvasContext.MoveToNewLine();

	if (DataPack.StashTabType == EDebugStashTabType::DSTT_Grid)
	{
		FVector2D StateMapTileSize = StateMapTileSize_Regular;
		FVector2D FontScale = StateMapGridNumberFontSizeScale_Regular;
		if(DataPack.Grid.GridStateMap.Num() > 200)
		{
			StateMapTileSize = StateMapTileSize_Small;
			FontScale = StateMapGridNumberFontSizeScale_Small; 
		}
		
		float DefaultTileX = ((CanvasWidth / 2) - StateMapTileSize.X) + (Padding * 2);
		
		CanvasContext.PrintAt(DefaultTileX, CanvasContext.CursorY, FString::Printf(TEXT("Grid Stash Tab State Map:")));
		CanvasContext.MoveToNewLine();
		
		float TileX = DefaultTileX;
		float TileY = CanvasContext.CursorY;
		constexpr float TilePadding = 5.0f;
		
		int32 CurrentRow = 1;
		for(TTuple<FIntPoint, bool> Pair : DataPack.Grid.GridStateMap)
		{
			if(CurrentRow == Pair.Key.Y)
			{
				TileY = CanvasContext.CursorY + (StateMapTileSize.Y + TilePadding) * CurrentRow;
				TileX = DefaultTileX;
				CurrentRow++;
			}
		
			if(Pair.Value == true)
			{
				FCanvasTileItem TakenField = {FVector2D(TileX, TileY), StateMapTileSize, TakenColor};
				CanvasContext.DrawItem(TakenField, TileX, TileY);
			}
			else
			{
				FCanvasTileItem FreeField = {FVector2D(TileX, TileY), StateMapTileSize, FreeColor};
				CanvasContext.DrawItem(FreeField, TileX, TileY);
			}

			FVector2D ScreenPos(TileX + 3.0f, TileY + 3.0f);
			FString Text = FString::Printf(TEXT("[%d, %d]"), Pair.Key.X, Pair.Key.Y);
			FCanvasTextItem TextItem(ScreenPos, FText::FromString(Text), GEngine->GetSmallFont(), FLinearColor::White);
			TextItem.Scale = FontScale;
			CanvasContext.Canvas->DrawItem(TextItem);
			
			TileX += StateMapTileSize.X + TilePadding;
		}
		
		CanvasContext.CursorY = CanvasContext.CursorY + (StateMapTileSize.Y + TilePadding) * CurrentRow;
		CanvasContext.CursorX = Padding;
		CanvasContext.PrintAt(DefaultTileX, CanvasContext.CursorY, FString::Printf(TEXT("{grey}Taken fields are painted red, free fields are green.")));
		CanvasContext.MoveToNewLine();
	}
	else if (DataPack.StashTabType == EDebugStashTabType::DSTT_Slots)
	{
		CanvasContext.Print(TEXT("Equipment Slots:"));
		CanvasContext.MoveToNewLine();

		float SecondCategoryTopX = CanvasContext.CursorX;
		float SecondCategoryTopY = CanvasContext.CursorY;
		
		CanvasContext.PrintAt(SecondCategoryTopX, SecondCategoryTopY, FString::Printf(TEXT("Slot Tag:")));
		CanvasContext.PrintAt(SecondCategoryTopX + ThirdArgConstX, SecondCategoryTopY, FString::Printf(TEXT("Accepted Equipment Categories:")));
		float CustomForthArgConstX = (ThirdArgConstX + CanvasWidth / 3) + 150.0f;
		CanvasContext.PrintAt(SecondCategoryTopX + CustomForthArgConstX, SecondCategoryTopY, FString::Printf(TEXT("Banned Equipment Categories:")));
		
		CanvasContext.MoveToNewLine();
		CanvasContext.CursorX += Padding;

		int32 AcceptedIncreasedLines = 0;
		int32 BannedIncreasedLines = 0;
		for(const FRepData::FStashSlotsDebug& EquipmentSlot : DataPack.Slots)
		{
			float CursorX = CanvasContext.CursorX;
			float CursorY = CanvasContext.CursorY;

			// Print positions manually to align them properly
			CanvasContext.PrintAt(CursorX, CursorY, FColor::Cyan, EquipmentSlot.SlotTag);
			
			float CachedCursorX = CursorX;
			float CachedCursorY = CursorY;
			for(const FString& AcceptedTagString : EquipmentSlot.AcceptedTags)
			{
				float SizeX;
				float SizeY;
				CanvasContext.MeasureString(AcceptedTagString, SizeX, SizeY);

				if (CachedCursorX + SizeX >= CursorX + CustomForthArgConstX - 500.0f)
				{
					CachedCursorY += CanvasContext.GetLineHeight();
					CachedCursorX = CursorX;
					AcceptedIncreasedLines++;
				}
				
				CanvasContext.PrintAt(CachedCursorX + ThirdArgConstX, CachedCursorY, AcceptedTagString);
				CachedCursorX += SizeX + Padding;
			}
			
			CachedCursorX = CursorX;
			CachedCursorY += CanvasContext.GetLineHeight();
			for(const FString& BannedTagString : EquipmentSlot.BannedTags)
			{
				float SizeX;
				float SizeY;
				CanvasContext.MeasureString(BannedTagString, SizeX, SizeY);
				
				if (CachedCursorX + SizeX >= CanvasWidth)
				{
					CachedCursorY += CanvasContext.GetLineHeight();
					CachedCursorX = CursorX;
					BannedIncreasedLines++;
				}
				
				CanvasContext.PrintAt(CachedCursorX + CustomForthArgConstX, CachedCursorY, BannedTagString);
				CachedCursorX += SizeX + Padding;
			}
			
			// PrintAt would have reset these values, restore them.
			CanvasContext.CursorX = CursorX + (CanvasWidth / NumColumns);
			CanvasContext.CursorY = CursorY + CanvasContext.GetLineHeight();

			int32 NumberOfEntriesInLine = FMath::Max<int32>(AcceptedIncreasedLines, BannedIncreasedLines);
			for(int i = 0; i < NumberOfEntriesInLine; i++)
			{
				CanvasContext.MoveToNewLine();
			}
			AcceptedIncreasedLines = 0;
			BannedIncreasedLines = 0;
			
			// If we're going to overflow, go to the next line...
			if (CanvasContext.CursorX + ColumnWidth >= CanvasWidth)
			{
				CanvasContext.MoveToNewLine();
				CanvasContext.CursorX += Padding;
			}
		}
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
		Ar << Slots[i].SlotTag;
		Ar << Slots[i].AcceptedTags;
		Ar << Slots[i].BannedTags;
	}

	Ar << bStashActive;
	Ar << StashTabTag;
	Ar << StashTabType;
	
	Ar << Grid.GridStateMap;
}

#endif // WITH_GAMEPLAY_DEBUGGER_MENU
