// Copyright 2026 out of sCope team - intrxx

#include "CoreMinimal.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "CQTest.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

#include "Obsidian/ObsidianGameplayTags.h"
#include "ObsidianTestWorldCleanup.h"
#include "TinyPOI/TinyPOIComponent.h"
#include "TinyPOI/TinyPOIData.h"
#include "TinyPOI/TinyPOIMarker.h"
#include "TinyPOI/TinyPOISubsystem.h"


namespace ObsidianTinyPOITestUtils
{
	/**
	 * A transient world for the TinyPOI tests. The bake only runs in an editor world and the baked data is only used
	 * on its own in a game world, so the tests need both kinds.
	 *
	 * The world does not tick, tests call UTinyPOISubsystem::RebuildData() where the editor would do it on Tick.
	 */
	struct FTestWorld
	{
		explicit FTestWorld(const EWorldType::Type InWorldType)
		{
			World = UWorld::CreateWorld(InWorldType, false);

			// Actors can not be destroyed in a world the engine has no context for.
			GEngine->CreateNewWorldContext(InWorldType).SetCurrentWorld(World);
		}

		~FTestWorld()
		{
			GEngine->DestroyWorldContext(World);
			World->DestroyWorld(false);
		}

		UTinyPOISubsystem& Subsystem() const
		{
			return *World->GetSubsystem<UTinyPOISubsystem>();
		}

		ATinyPOIMarker* AddMarker(const FVector& InLocation, const FGameplayTag& InTag, const float InRadius = 100.0f) const
		{
			ATinyPOIMarker* Marker = World->SpawnActor<ATinyPOIMarker>(InLocation, FRotator::ZeroRotator);
			Marker->GetPOIComponent()->SetTag(InTag);
			Marker->GetPOIComponent()->SetRadius(InRadius);
			return Marker;
		}

		TArray<ATinyPOIData*> GetAllData() const
		{
			TArray<ATinyPOIData*> AllData;
			for(TActorIterator<ATinyPOIData> It(World); It; ++It)
			{
				AllData.Add(*It);
			}
			return AllData;
		}

		/** Bakes and returns the Storage of the one Data the persistent level is expected to end up with. */
		const FTinyPOIStorage* Bake() const
		{
			Subsystem().RebuildData();

			const TArray<ATinyPOIData*> AllData = GetAllData();
			return AllData.Num() == 1 ? &AllData[0]->GetStorage() : nullptr;
		}

		/** Declared before the World, so it is destroyed after it. */
		FObsidianTestWorldCleanup WorldCleanup;
		UWorld* World = nullptr;
	};

	/** Saves the Storage the same way it is saved with a level, as tagged properties. */
	TArray<uint8> SaveStorage(const FTinyPOIStorage& InStorage)
	{
		TArray<uint8> Bytes;
		FMemoryWriter Writer(Bytes);
		FObjectAndNameAsStringProxyArchive Archive(Writer, false);
		FTinyPOIStorage::StaticStruct()->SerializeItem(Archive, const_cast<FTinyPOIStorage*>(&InStorage), nullptr);
		return Bytes;
	}

	void LoadStorage(const TArray<uint8>& InBytes, FTinyPOIStorage& OutStorage)
	{
		FMemoryReader Reader(InBytes);
		FObjectAndNameAsStringProxyArchive Archive(Reader, false);
		FTinyPOIStorage::StaticStruct()->SerializeItem(Archive, &OutStorage, nullptr);
	}
}

/**
 * Tests of the TinyPOI system, run them from Session Frontend -> Automation -> Obsidian.TinyPOI.
 * They cover the whole way of a Point Of Interest: markup in an editor world, the bake into FTinyPOIStorage and the
 * queries on the baked data, including a game world that has nothing but the saved data.
 */
TEST_CLASS(ObsidianTinyPOITests, "Obsidian.TinyPOI")
{
	using FTestWorld = ObsidianTinyPOITestUtils::FTestWorld;

	const FGameplayTag RestTag = ObsidianGameplayTags::TinyPOI::Rest;
	const FGameplayTag LookoutTag = ObsidianGameplayTags::TinyPOI::Lookout;
	const FGameplayTag AmbushTag = ObsidianGameplayTags::TinyPOI::Ambush;

	TUniquePtr<FTestWorld> Editor;

	BEFORE_EACH()
	{
		Editor = MakeUnique<FTestWorld>(EWorldType::Editor);
	}

	AFTER_EACH()
	{
		Editor.Reset();
	}

	/**
	 * Baking.
	 */

	TEST_METHOD(Bake_SpawnsDataInTheLevelOfTheMarkers)
	{
		ASSERT_THAT(AreEqual(0, Editor->GetAllData().Num()));
		ASSERT_THAT(IsFalse(Editor->Subsystem().GetBuilder().NeedsRebuild()));

		const ATinyPOIMarker* Marker = Editor->AddMarker(FVector(100.0, 0.0, 0.0), RestTag);
		Editor->AddMarker(FVector(500.0, 0.0, 0.0), LookoutTag);
		Editor->AddMarker(FVector(-200.0, 0.0, 0.0), RestTag);

		ASSERT_THAT(AreEqual(3, Editor->Subsystem().GetBuilder().GetRegisteredPOIComponents().Num()));
		ASSERT_THAT(IsTrue(Editor->Subsystem().GetBuilder().NeedsRebuild()));
		ASSERT_THAT(AreEqual(0, Editor->Subsystem().GetNumPoints()));

		Editor->Subsystem().RebuildData();

		const TArray<ATinyPOIData*> AllData = Editor->GetAllData();
		ASSERT_THAT(AreEqual(1, AllData.Num()));
		ASSERT_THAT(IsTrue(AllData[0]->GetLevel() == Marker->GetLevel()));
		ASSERT_THAT(IsTrue(AllData[0]->IsRegistered()));
		ASSERT_THAT(IsTrue(AllData[0]->GetStorage().DataHandle.IsValid()));
		ASSERT_THAT(AreEqual(3, AllData[0]->GetStorage().Points.Num()));
		ASSERT_THAT(AreEqual(3, Editor->Subsystem().GetNumPoints()));
		ASSERT_THAT(IsFalse(Editor->Subsystem().GetBuilder().NeedsRebuild()));

		// Baking again uses the Data that is already there.
		Editor->AddMarker(FVector(0.0, 300.0, 0.0), AmbushTag);
		Editor->Subsystem().RebuildData();

		ASSERT_THAT(AreEqual(1, Editor->GetAllData().Num()));
		ASSERT_THAT(AreEqual(4, Editor->Subsystem().GetNumPoints()));
	}

	TEST_METHOD(Bake_GroupsThePointsByTag)
	{
		Editor->AddMarker(FVector(100.0, 0.0, 0.0), RestTag);
		Editor->AddMarker(FVector(500.0, 0.0, 0.0), LookoutTag);
		Editor->AddMarker(FVector(-200.0, 0.0, 0.0), RestTag);

		const FTinyPOIStorage* Storage = Editor->Bake();
		ASSERT_THAT(IsNotNull(Storage));
		ASSERT_THAT(AreEqual(3, Storage->Points.Num()));

		// One range per Tag, sorted by the name of the Tag.
		ASSERT_THAT(AreEqual(2, Storage->TagRanges.Num()));
		ASSERT_THAT(IsTrue(Storage->TagRanges[0].Tag == LookoutTag));
		ASSERT_THAT(AreEqual(0, Storage->TagRanges[0].PointsBegin));
		ASSERT_THAT(AreEqual(1, Storage->TagRanges[0].PointsEnd));
		ASSERT_THAT(IsTrue(Storage->TagRanges[1].Tag == RestTag));
		ASSERT_THAT(AreEqual(1, Storage->TagRanges[1].PointsBegin));
		ASSERT_THAT(AreEqual(3, Storage->TagRanges[1].PointsEnd));

		// Inside of a range the Points are sorted by position, no matter the order the Markers were added in.
		ASSERT_THAT(IsTrue(Storage->Points[0].Tag == LookoutTag));
		ASSERT_THAT(IsNear(500.0, Storage->Points[0].Position.X, UE_DOUBLE_KINDA_SMALL_NUMBER));
		ASSERT_THAT(IsNear(-200.0, Storage->Points[1].Position.X, UE_DOUBLE_KINDA_SMALL_NUMBER));
		ASSERT_THAT(IsNear(100.0, Storage->Points[2].Position.X, UE_DOUBLE_KINDA_SMALL_NUMBER));
	}

	TEST_METHOD(Bake_StoresThePointsInWorldSpace)
	{
		ATinyPOIMarker* Marker = Editor->AddMarker(FVector(100.0, 200.0, 300.0), RestTag, 50.0f);
		Marker->SetActorRotation(FRotator(0.0, 90.0, 0.0));
		Marker->SetActorScale3D(FVector(2.0));

		const FTinyPOIStorage* Storage = Editor->Bake();
		ASSERT_THAT(IsNotNull(Storage));
		ASSERT_THAT(AreEqual(1, Storage->Points.Num()));

		const FTinyPOIPoint& Point = Storage->Points[0];
		ASSERT_THAT(IsTrue(Point.Position.Equals(FVector(100.0, 200.0, 300.0), UE_KINDA_SMALL_NUMBER)));
		ASSERT_THAT(IsTrue(Point.Forward.Equals(FVector(0.0, 1.0, 0.0), UE_KINDA_SMALL_NUMBER)));
		ASSERT_THAT(IsNear(100.0f, Point.Radius, UE_KINDA_SMALL_NUMBER));

		// Bounds cover the Point with its radius.
		ASSERT_THAT(IsTrue(Storage->Bounds.Min.Equals(FVector(0.0, 100.0, 200.0), UE_KINDA_SMALL_NUMBER)));
		ASSERT_THAT(IsTrue(Storage->Bounds.Max.Equals(FVector(200.0, 300.0, 400.0), UE_KINDA_SMALL_NUMBER)));
	}

	TEST_METHOD(Bake_IsSkippedWhenTheMarkupDidNotChange)
	{
		Editor->AddMarker(FVector(100.0, 0.0, 0.0), RestTag);
		ASSERT_THAT(IsNotNull(Editor->Bake()));

		ATinyPOIData* Data = Editor->GetAllData()[0];
		const uint32 BakedHash = Data->GetCombinedHash();

		// Break the baked data by hand to see if the next bake touches it.
		Data->GetStorageMutable().Points.Reset();

		Editor->Subsystem().RebuildData();
		ASSERT_THAT(AreEqual(0, Data->GetStorage().Points.Num()));

		Editor->Subsystem().RebuildData(true);
		ASSERT_THAT(AreEqual(1, Data->GetStorage().Points.Num()));
		ASSERT_THAT(AreEqual(BakedHash, Data->GetCombinedHash()));
	}

	TEST_METHOD(Bake_FollowsTheChangesOfTheMarkers)
	{
		ATinyPOIMarker* Marker = Editor->AddMarker(FVector(100.0, 0.0, 0.0), RestTag);
		const FTinyPOIStorage* Storage = Editor->Bake();
		ASSERT_THAT(IsNotNull(Storage));

		const ATinyPOIData* Data = Editor->GetAllData()[0];
		const FTinyPOIBuilder& Builder = Editor->Subsystem().GetBuilder();
		uint32 LastHash = Data->GetCombinedHash();

		// Moved.
		Marker->SetActorLocation(FVector(0.0, 700.0, 0.0));
		ASSERT_THAT(IsTrue(Builder.NeedsRebuild()));
		Editor->Subsystem().RebuildData();
		ASSERT_THAT(IsTrue(Storage->Points[0].Position.Equals(FVector(0.0, 700.0, 0.0), UE_KINDA_SMALL_NUMBER)));
		ASSERT_THAT(AreNotEqual(LastHash, Data->GetCombinedHash()));
		LastHash = Data->GetCombinedHash();

		// Tag changed.
		Marker->GetPOIComponent()->SetTag(AmbushTag);
		ASSERT_THAT(IsTrue(Builder.NeedsRebuild()));
		Editor->Subsystem().RebuildData();
		ASSERT_THAT(IsTrue(Storage->Points[0].Tag == AmbushTag));
		ASSERT_THAT(IsTrue(Storage->TagRanges[0].Tag == AmbushTag));
		ASSERT_THAT(AreNotEqual(LastHash, Data->GetCombinedHash()));
		LastHash = Data->GetCombinedHash();

		// Radius changed.
		Marker->GetPOIComponent()->SetRadius(250.0f);
		ASSERT_THAT(IsTrue(Builder.NeedsRebuild()));
		Editor->Subsystem().RebuildData();
		ASSERT_THAT(IsNear(250.0f, Storage->Points[0].Radius, UE_KINDA_SMALL_NUMBER));
		ASSERT_THAT(AreNotEqual(LastHash, Data->GetCombinedHash()));

		// Removed, the level keeps its Data, it is just empty.
		Marker->Destroy();
		ASSERT_THAT(IsTrue(Builder.NeedsRebuild()));
		Editor->Subsystem().RebuildData();
		ASSERT_THAT(AreEqual(1, Editor->GetAllData().Num()));
		ASSERT_THAT(AreEqual(0, Storage->Points.Num()));
		ASSERT_THAT(AreEqual(0, Storage->TagRanges.Num()));
		ASSERT_THAT(IsFalse(static_cast<bool>(Storage->Bounds.IsValid)));
	}

	TEST_METHOD(Bake_GivesTheSameHashNoMatterTheOrderOfTheMarkers)
	{
		Editor->AddMarker(FVector(100.0, 0.0, 0.0), RestTag);
		Editor->AddMarker(FVector(500.0, 0.0, 0.0), LookoutTag);
		ASSERT_THAT(IsNotNull(Editor->Bake()));

		// The same markup, registered the other way around, as it could be the next time the level is loaded.
		FTestWorld OtherEditor(EWorldType::Editor);
		OtherEditor.AddMarker(FVector(500.0, 0.0, 0.0), LookoutTag);
		OtherEditor.AddMarker(FVector(100.0, 0.0, 0.0), RestTag);
		ASSERT_THAT(IsNotNull(OtherEditor.Bake()));

		ASSERT_THAT(AreEqual(Editor->GetAllData()[0]->GetCombinedHash(), OtherEditor.GetAllData()[0]->GetCombinedHash()));
	}

	/**
	 * Queries.
	 */

	TEST_METHOD(FindNearestPoint_RespectsDistanceAndTagFilter)
	{
		Editor->AddMarker(FVector(100.0, 0.0, 0.0), RestTag);
		Editor->AddMarker(FVector(500.0, 0.0, 0.0), LookoutTag);
		Editor->AddMarker(FVector(2000.0, 0.0, 0.0), RestTag);
		Editor->AddMarker(FVector(50.0, 0.0, 0.0), FGameplayTag());
		ASSERT_THAT(IsNotNull(Editor->Bake()));

		const UTinyPOISubsystem& Subsystem = Editor->Subsystem();
		const FGameplayTag AnyTinyPOITag = RestTag.RequestDirectParent();
		FTinyPOIHandle Handle;

		// No filter, the untagged Point is the closest one.
		ASSERT_THAT(IsTrue(Subsystem.FindNearestPoint(FVector::ZeroVector, 1000.0f, FGameplayTag(), Handle)));
		ASSERT_THAT(IsNotNull(Subsystem.GetPoint(Handle)));
		ASSERT_THAT(IsNear(50.0, Subsystem.GetPoint(Handle)->Position.X, UE_DOUBLE_KINDA_SMALL_NUMBER));

		// Exact Tag.
		ASSERT_THAT(IsTrue(Subsystem.FindNearestPoint(FVector::ZeroVector, 1000.0f, LookoutTag, Handle)));
		ASSERT_THAT(IsNear(500.0, Subsystem.GetPoint(Handle)->Position.X, UE_DOUBLE_KINDA_SMALL_NUMBER));

		ASSERT_THAT(IsTrue(Subsystem.FindNearestPoint(FVector(1800.0, 0.0, 0.0), 1000.0f, RestTag, Handle)));
		ASSERT_THAT(IsNear(2000.0, Subsystem.GetPoint(Handle)->Position.X, UE_DOUBLE_KINDA_SMALL_NUMBER));

		// Parent Tag matches every tagged Point, but not the untagged one.
		ASSERT_THAT(IsTrue(Subsystem.FindNearestPoint(FVector::ZeroVector, 1000.0f, AnyTinyPOITag, Handle)));
		ASSERT_THAT(IsNear(100.0, Subsystem.GetPoint(Handle)->Position.X, UE_DOUBLE_KINDA_SMALL_NUMBER));

		// Nothing in range, nothing with the Tag.
		ASSERT_THAT(IsFalse(Subsystem.FindNearestPoint(FVector::ZeroVector, 400.0f, LookoutTag, Handle)));
		ASSERT_THAT(IsFalse(Handle.IsValid()));
		ASSERT_THAT(IsFalse(Subsystem.FindNearestPoint(FVector(0.0, 0.0, 50000.0), 1000.0f, FGameplayTag(), Handle)));
		ASSERT_THAT(IsFalse(Subsystem.FindNearestPoint(FVector::ZeroVector, 100000.0f, AmbushTag, Handle)));
	}

	TEST_METHOD(FindPointsInRange_ReturnsEveryMatchingPoint)
	{
		Editor->AddMarker(FVector(100.0, 0.0, 0.0), RestTag);
		Editor->AddMarker(FVector(500.0, 0.0, 0.0), LookoutTag);
		Editor->AddMarker(FVector(2000.0, 0.0, 0.0), RestTag);
		ASSERT_THAT(IsNotNull(Editor->Bake()));

		const UTinyPOISubsystem& Subsystem = Editor->Subsystem();
		TArray<FTinyPOIHandle> Handles;

		ASSERT_THAT(IsTrue(Subsystem.FindPointsInRange(FVector::ZeroVector, 1000.0f, FGameplayTag(), Handles)));
		ASSERT_THAT(AreEqual(2, Handles.Num()));

		ASSERT_THAT(IsTrue(Subsystem.FindPointsInRange(FVector::ZeroVector, 5000.0f, RestTag, Handles)));
		ASSERT_THAT(AreEqual(2, Handles.Num()));
		for(const FTinyPOIHandle& Handle : Handles)
		{
			ASSERT_THAT(IsNotNull(Subsystem.GetPoint(Handle)));
			ASSERT_THAT(IsTrue(Subsystem.GetPoint(Handle)->Tag == RestTag));
		}

		ASSERT_THAT(IsFalse(Subsystem.FindPointsInRange(FVector::ZeroVector, 50.0f, FGameplayTag(), Handles)));
		ASSERT_THAT(AreEqual(0, Handles.Num()));
	}

	/**
	 * The game.
	 */

	TEST_METHOD(GameWorld_UsesTheSavedDataWithoutAnyMarkup)
	{
		Editor->AddMarker(FVector(100.0, 0.0, 0.0), RestTag);
		Editor->AddMarker(FVector(500.0, 0.0, 0.0), LookoutTag, 300.0f);
		const FTinyPOIStorage* BakedStorage = Editor->Bake();
		ASSERT_THAT(IsNotNull(BakedStorage));

		const TArray<uint8> SavedStorage = ObsidianTinyPOITestUtils::SaveStorage(*BakedStorage);

		// The editor world is gone, the saved bytes are all the game gets.
		Editor.Reset();

		const FTestWorld Game(EWorldType::Game);
		UTinyPOISubsystem& Subsystem = Game.Subsystem();

		// Markup does not take part in a game world, even if it ends up in one (as it does in PIE).
		Game.AddMarker(FVector(-5000.0, 0.0, 0.0), AmbushTag);
		ASSERT_THAT(AreEqual(0, Subsystem.GetBuilder().GetRegisteredPOIComponents().Num()));
		Subsystem.RebuildData(true);
		ASSERT_THAT(AreEqual(0, Game.GetAllData().Num()));

		// Stands for the Data that would be loaded with the level.
		ATinyPOIData* Data = Game.World->SpawnActor<ATinyPOIData>();
		ASSERT_THAT(IsTrue(Data->IsRegistered()));
		ObsidianTinyPOITestUtils::LoadStorage(SavedStorage, Data->GetStorageMutable());

		// The handle is not saved, it comes from the registration in this world.
		ASSERT_THAT(IsTrue(Data->GetStorage().DataHandle.IsValid()));
		ASSERT_THAT(IsTrue(Subsystem.GetStorage(Data->GetStorage().DataHandle) == &Data->GetStorage()));
		ASSERT_THAT(AreEqual(2, Subsystem.GetNumPoints()));
		ASSERT_THAT(AreEqual(2, Data->GetStorage().TagRanges.Num()));

		FTinyPOIHandle Handle;
		ASSERT_THAT(IsTrue(Subsystem.FindNearestPoint(FVector::ZeroVector, 1000.0f, LookoutTag, Handle)));

		const FTinyPOIPoint* Point = Subsystem.GetPoint(Handle);
		ASSERT_THAT(IsNotNull(Point));
		ASSERT_THAT(IsTrue(Point->Tag == LookoutTag));
		ASSERT_THAT(IsNear(500.0, Point->Position.X, UE_DOUBLE_KINDA_SMALL_NUMBER));
		ASSERT_THAT(IsNear(300.0f, Point->Radius, UE_KINDA_SMALL_NUMBER));

		// Once the Data is gone (its level got unloaded), the handles to its Points stop resolving...
		Data->Destroy();
		ASSERT_THAT(IsNull(Subsystem.GetPoint(Handle)));
		ASSERT_THAT(AreEqual(0, Subsystem.GetNumPoints()));

		// ...and keep doing so after another Data takes over the freed index.
		ATinyPOIData* OtherData = Game.World->SpawnActor<ATinyPOIData>();
		ObsidianTinyPOITestUtils::LoadStorage(SavedStorage, OtherData->GetStorageMutable());
		ASSERT_THAT(AreEqual(Handle.DataHandle.Index, OtherData->GetStorage().DataHandle.Index));
		ASSERT_THAT(IsNull(Subsystem.GetPoint(Handle)));
		ASSERT_THAT(IsTrue(Subsystem.FindNearestPoint(FVector::ZeroVector, 1000.0f, LookoutTag, Handle)));
		ASSERT_THAT(IsNotNull(Subsystem.GetPoint(Handle)));
	}
};

#endif // WITH_DEV_AUTOMATION_TESTS
