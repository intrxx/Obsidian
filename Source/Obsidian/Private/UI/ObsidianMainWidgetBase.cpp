// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/ObsidianMainWidgetBase.h"
#include "CharacterComponents/ObsidianHeroComponent.h"
#include "Kismet/GameplayStatics.h"

void UObsidianMainWidgetBase::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return;
	}
	
	if(HeroComp == nullptr)
	{
		if(const UWorld* World = GetWorld())
		{
			if(APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
			{
				AActor* OwningActor = Cast<AActor>(PC->GetPawn());
				HeroComp = UObsidianHeroComponent::FindHeroComponent(OwningActor);
			}
		}
	}
	HeroComp->SetCursorOverUI(true);
	UE_LOG(LogTemp, Warning, TEXT("Entered Main Obsidian Widget"));
	
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UObsidianMainWidgetBase::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return;
	}
	
	if(HeroComp == nullptr)
	{
		if(const UWorld* World = GetWorld())
		{
			if(APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
			{
				AActor* OwningActor = Cast<AActor>(PC->GetPawn());
				HeroComp = UObsidianHeroComponent::FindHeroComponent(OwningActor);
			}
		}
	}
	HeroComp->SetCursorOverUI(false);
	UE_LOG(LogTemp, Warning, TEXT("Left Main Obsidian Widget"));
	
	Super::NativeOnMouseLeave(InMouseEvent);
}