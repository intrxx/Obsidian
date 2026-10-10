// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Debugging/GameplayDebuggerCategory_PlayerStash.h"

#if WITH_GAMEPLAY_DEBUGGER_MENU
#include "CanvasItem.h"
#include "Engine/Canvas.h"

#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"
#include "InventoryItems/PlayerStash/ObsidianStashItemList.h"
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

void FGameplayDebuggerCategory_PlayerStash::CollectData(APlayerController* InOwnerPC, AActor* InDebugActor)
{
	DataPack.Items.Empty();
	DataPack.Grid.GridStateMap.Reset();
	DataPack.Slots.Empty();
	DataPack.StashTabType = EDebugStashTabType::DSTT_None;
	
	if(const AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(InOwnerPC))
	{
		if(const AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD())
		{
			DataPack.bStashActive = ObsidianHUD->IsPlayerStashOpened();
		}
	}
	
	if(UObsidianPlayerStashComponent* PlayerStashComponent = InOwnerPC->FindComponentByClass<UObsidianPlayerStashComponent>())
	{
		DataPack.StashTabTag = PlayerStashComponent->GetActiveStashTag();
		if (DataPack.StashTabTag == FGameplayTag::EmptyTag)
		{
			return;
		}
		
		TArray<UObsidianInventoryItemInstance*> Items = PlayerStashComponent->GetAllItemsFromStashTab(DataPack.StashTabTag);
		for(const UObsidianInventoryItemInstance* Item : Items)
		{
			FRepData::FStashedItemsDebug InventoryItems;
			
			InventoryItems.Name = Item->GetItemDebugName();
			InventoryItems.ItemUniqueID = Item->GetUniqueItemID().ToString();
			InventoryItems.Item = GetNameSafe(Item->GetItemDef());
			InventoryItems.Item.RemoveFromEnd(TEXT("_C"));
			InventoryItems.CurrentStackCount = Item->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
			InventoryItems.MaxStackCount = Item->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Max);
			InventoryItems.LimitStackCount = Item->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Limit);
			InventoryItems.GridSpan = Item->GetItemGridSpan();
			InventoryItems.CurrentGridLocation = Item->GetItemCurrentPosition().GetItemGridPosition(false);
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
			
				for(const FObsidianStashSlotDefinition& SlotDef : SlotsStashTab->TabSlots)
				{
					FRepData::FStashSlotsDebug TabSlot;
					TabSlot.SlotTag = SlotDef.BaseSlotDefinition.SlotTag.GetTagName().ToString();

					//TODO(intrxx) Add Limit from FObsidianStashSlotDefinition;
					
					for(const FGameplayTag& Tag : SlotDef.BaseSlotDefinition.AcceptedItemCategories)
					{
						TabSlot.AcceptedTags.Add(Tag.GetTagName().ToString());
					}
					for(const FGameplayTag& Tag : SlotDef.BaseSlotDefinition.BannedItemCategories)
					{
						TabSlot.BannedTags.Add(Tag.GetTagName().ToString());
					}

					DataPack.Slots.Add(TabSlot);
				}
			}
		}
		
	}
}

void FGameplayDebuggerCategory_PlayerStash::DrawData(APlayerController* InOwnerPC, FGameplayDebuggerCanvasContext& InCanvasContext)
{
	if (LastDrawDataEndSize <= 0.0f)
	{
		LastDrawDataEndSize = InCanvasContext.Canvas->SizeY - InCanvasContext.CursorY - InCanvasContext.CursorX;
	}

	const float ThisDrawDataStartPos = InCanvasContext.CursorY;
	
	const FVector2D BackgroundPos{InCanvasContext.CursorX, InCanvasContext.CursorY};
	const FVector2D BackgroundSize{InCanvasContext.Canvas->SizeX -  (2.0f * InCanvasContext.CursorX), LastDrawDataEndSize};

	FCanvasTileItem Background(FVector2D(0.0f), BackgroundSize, PlayerStash::Debug::BackgroundColor);
	Background.BlendMode = SE_BLEND_Translucent;
	
	InCanvasContext.DrawItem(Background, BackgroundPos.X, BackgroundPos.Y);

	DrawItems(InOwnerPC, InCanvasContext);

	LastDrawDataEndSize = InCanvasContext.CursorY - ThisDrawDataStartPos;
}

void FGameplayDebuggerCategory_PlayerStash::DrawItems(APlayerController* InOwnerPC, FGameplayDebuggerCanvasContext& InCanvasContext) const
{
	using namespace PlayerStash::Debug;
	
	const float CanvasWidth = InCanvasContext.Canvas->SizeX;
	Algo::Sort(DataPack.Items, [](const FRepData::FStashedItemsDebug& InItemOne, const FRepData::FStashedItemsDebug& InItemTwo) { return InItemOne.Name < InItemTwo.Name; });

	constexpr float Padding = 10.0f;
	static float ObjNameSize = 0.0f;
	static float ItemNameSize = 0.0f;
	static float UniqueItemIDSize = 0.0f;
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
		InCanvasContext.MeasureString(*LongestDebugObjectName, ObjNameSize, TempSizeY);
		InCanvasContext.MeasureString(*LongestDebugObjectName, ItemNameSize, TempSizeY);
		InCanvasContext.MeasureString(*LongestDebugObjectName, UniqueItemIDSize, TempSizeY);
		InCanvasContext.MeasureString(TEXT("current stack count: 00"), CurrentStackCountNameSize, TempSizeY);
		InCanvasContext.MeasureString(TEXT("max stack count: 00"), MaxStackCountSize, TempSizeY);
		InCanvasContext.MeasureString(TEXT("limit stack count: 00"), LimitStackCountSize, TempSizeY);
		InCanvasContext.MeasureString(TEXT("grid size: 00"), GridSpanSize, TempSizeY);
		InCanvasContext.MeasureString(TEXT("grid location: 00"), CurrentGridLocationSize, TempSizeY);
		InCanvasContext.MeasureString(TEXT("Player Stash is Closed, to view items open it."), NoItemsTextSize, TempSizeY);
		ObjNameSize += Padding;
	}
	const float SecondArgConstX = ObjNameSize * 0.7;
	const float ThirdArgConstX = ObjNameSize * 0.8 + ItemNameSize;
	const float FourthArgConstX = ObjNameSize * 0.6 + ItemNameSize + UniqueItemIDSize;
	const float FifthArgConstX = ObjNameSize * 0.9 + ItemNameSize + UniqueItemIDSize + CurrentStackCountNameSize;
	const float SixthArgConstX = ObjNameSize * 1.2 + ItemNameSize + UniqueItemIDSize + CurrentStackCountNameSize + MaxStackCountSize;
	const float SeventhArgConstX = ObjNameSize * 1.7 + ItemNameSize + UniqueItemIDSize + CurrentStackCountNameSize + MaxStackCountSize + LimitStackCountSize;
	const float EighthArgConstX = ObjNameSize * 2.4 + ItemNameSize + UniqueItemIDSize + CurrentStackCountNameSize + MaxStackCountSize + LimitStackCountSize + GridSpanSize;


	const float ColumnWidth = ObjNameSize * 5 + ItemNameSize + CurrentStackCountNameSize + MaxStackCountSize + LimitStackCountSize + GridSpanSize + CurrentGridLocationSize;
	const int NumColumns = FMath::Max(1, FMath::FloorToInt(CanvasWidth / ColumnWidth));

	bool bStashActive = DataPack.bStashActive;
	if (InOwnerPC->HasAuthority() == false) // If we are on the client the check failed on the Server since we check the HUD, recheck here
	{
		if(const AObsidianPlayerController* ObsidianPC = Cast<AObsidianPlayerController>(InOwnerPC))
		{
			if(const AObsidianHUD* ObsidianHUD = ObsidianPC->GetObsidianHUD())
			{
				bStashActive = ObsidianHUD->IsPlayerStashOpened();
			}
		}
	}
	
	if (bStashActive == false)
	{
		InCanvasContext.PrintAt((CanvasWidth / 2) - (NoItemsTextSize / 2), InCanvasContext.Canvas->SizeY / 2,
			FString::Printf(TEXT("{red}Player Stash is Closed, to view items open it.")));
		return;
	}

	float TopCursorY = InCanvasContext.CursorY;
	float TopCursorX = InCanvasContext.CursorX;
	InCanvasContext.PrintAt(TopCursorX, TopCursorY, FString::Printf(TEXT("Stashed Items:")));
	TopCursorX += 300.0f;

	const int32 ItemsNum = DataPack.Items.Num();
	InCanvasContext.PrintAt(TopCursorX, TopCursorY, FString::Printf(TEXT("Owned Items Count: {yellow}%d"), ItemsNum));
	TopCursorX += 300.0f;

	InCanvasContext.PrintAt(TopCursorX, TopCursorY, FString::Printf(TEXT("Stash Gameplay Tag: {yellow}%s"), *DataPack.StashTabTag.GetTagName().ToString()));
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
	InCanvasContext.PrintAt(TopCursorX + EighthArgConstX, TopCursorY, FString::Printf(TEXT("Item Location:")));

	InCanvasContext.MoveToNewLine();
	InCanvasContext.CursorX += Padding;
	for (const FRepData::FStashedItemsDebug& ItemData : DataPack.Items)
	{
		float CursorX = InCanvasContext.CursorX;
		float CursorY = InCanvasContext.CursorY;

		// Print positions manually to align them properly
		InCanvasContext.PrintAt(CursorX, CursorY, FColor::Cyan, ItemData.Name.Left(35));
		InCanvasContext.PrintAt(CursorX + SecondArgConstX, CursorY, FColor::Emerald, ItemData.ItemUniqueID);
		InCanvasContext.PrintAt(CursorX + ThirdArgConstX, CursorY, FColor::Emerald, ItemData.Item);
		InCanvasContext.PrintAt(CursorX + FourthArgConstX, CursorY, FString::Printf(TEXT("{grey}Count: {yellow}%d"), ItemData.CurrentStackCount));
		InCanvasContext.PrintAt(CursorX + FifthArgConstX, CursorY, FString::Printf(TEXT("{grey}Count: {yellow}%d"), ItemData.MaxStackCount));
		InCanvasContext.PrintAt(CursorX + SixthArgConstX, CursorY, FString::Printf(TEXT("{grey}Count: {yellow}%d"), ItemData.LimitStackCount));
		InCanvasContext.PrintAt(CursorX + SeventhArgConstX, CursorY, FString::Printf(TEXT("{grey}Size: {yellow}[%d, %d]"), ItemData.GridSpan.X, ItemData.GridSpan.Y));
		if (ItemData.CurrentGridLocation != FIntPoint::NoneValue)
		{
			InCanvasContext.PrintAt(CursorX + EighthArgConstX, CursorY, FString::Printf(TEXT("{grey}Location: {yellow}[%d, %d]"), ItemData.CurrentGridLocation.X, ItemData.CurrentGridLocation.Y));
		}
		else if (ItemData.CurrentSlotTag != FGameplayTag::EmptyTag)
		{
			InCanvasContext.PrintAt(CursorX + EighthArgConstX, CursorY, FString::Printf(TEXT("{grey}Slot Tag: {yellow}%s"), *ItemData.CurrentSlotTag.GetTagName().ToString()));
		}
		
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
		
		InCanvasContext.PrintAt(DefaultTileX, InCanvasContext.CursorY, FString::Printf(TEXT("Grid Stash Tab State Map:")));
		InCanvasContext.MoveToNewLine();
		
		float TileX = DefaultTileX;
		float TileY = InCanvasContext.CursorY;
		constexpr float TilePadding = 5.0f;
		
		int32 CurrentRow = 1;
		for(TTuple<FIntPoint, bool> Pair : DataPack.Grid.GridStateMap)
		{
			if(CurrentRow == Pair.Key.Y)
			{
				TileY = InCanvasContext.CursorY + (StateMapTileSize.Y + TilePadding) * CurrentRow;
				TileX = DefaultTileX;
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

			FVector2D ScreenPos(TileX + 3.0f, TileY + 3.0f);
			FString Text = FString::Printf(TEXT("[%d, %d]"), Pair.Key.X, Pair.Key.Y);
			FCanvasTextItem TextItem(ScreenPos, FText::FromString(Text), GEngine->GetSmallFont(), FLinearColor::White);
			TextItem.Scale = FontScale;
			InCanvasContext.Canvas->DrawItem(TextItem);
			
			TileX += StateMapTileSize.X + TilePadding;
		}
		
		InCanvasContext.CursorY = InCanvasContext.CursorY + (StateMapTileSize.Y + TilePadding) * CurrentRow;
		InCanvasContext.CursorX = Padding;
		InCanvasContext.PrintAt(DefaultTileX, InCanvasContext.CursorY, FString::Printf(TEXT("{grey}Taken fields are painted red, free fields are green.")));
		InCanvasContext.MoveToNewLine();
	}
	else if (DataPack.StashTabType == EDebugStashTabType::DSTT_Slots)
	{
		InCanvasContext.Print(TEXT("Equipment Slots:"));
		InCanvasContext.MoveToNewLine();

		float SecondCategoryTopX = InCanvasContext.CursorX;
		float SecondCategoryTopY = InCanvasContext.CursorY;
		
		InCanvasContext.PrintAt(SecondCategoryTopX, SecondCategoryTopY, FString::Printf(TEXT("Slot Tag:")));
		InCanvasContext.PrintAt(SecondCategoryTopX + ThirdArgConstX, SecondCategoryTopY, FString::Printf(TEXT("Accepted Equipment Categories:")));
		float CustomForthArgConstX = (ThirdArgConstX + CanvasWidth / 3) + 150.0f;
		InCanvasContext.PrintAt(SecondCategoryTopX + CustomForthArgConstX, SecondCategoryTopY, FString::Printf(TEXT("Banned Equipment Categories:")));
		
		InCanvasContext.MoveToNewLine();
		InCanvasContext.CursorX += Padding;

		int32 AcceptedIncreasedLines = 0;
		int32 BannedIncreasedLines = 0;
		for(const FRepData::FStashSlotsDebug& EquipmentSlot : DataPack.Slots)
		{
			float CursorX = InCanvasContext.CursorX;
			float CursorY = InCanvasContext.CursorY;

			// Print positions manually to align them properly
			InCanvasContext.PrintAt(CursorX, CursorY, FColor::Cyan, EquipmentSlot.SlotTag);
			
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
}

void FGameplayDebuggerCategory_PlayerStash::FRepData::Serialize(FArchive& InOutAr)
{
	int32 NumItems = Items.Num();
	InOutAr << NumItems;
	int32 NumSlots = Slots.Num();
	InOutAr << NumSlots;
	
	if(InOutAr.IsLoading())
	{
		Items.SetNum(NumItems);
		Slots.SetNum(NumSlots);
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
		InOutAr << Items[i].CurrentSlotTag;
	}

	for(int32 i = 0; i < NumSlots; i++)
	{
		InOutAr << Slots[i].SlotTag;
		InOutAr << Slots[i].AcceptedTags;
		InOutAr << Slots[i].BannedTags;
	}

	InOutAr << bStashActive;
	InOutAr << StashTabTag;
	InOutAr << StashTabType;
	
	InOutAr << Grid.GridStateMap;
}

#endif // WITH_GAMEPLAY_DEBUGGER_MENU
