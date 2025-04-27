// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/FrontEnd/ObsidianCharacterScreen.h"

#include "Kismet/GameplayStatics.h"
#include "UI/Components/ObsidianButtonBase.h"

UWidget* UObsidianCharacterScreen::NativeGetDesiredFocusTarget() const
{
	return Play_Button;
}

void UObsidianCharacterScreen::NativeConstruct()
{
	Super::NativeConstruct();

	if(Play_Button)
	{
		Play_Button->OnClicked().AddUObject(this, &ThisClass::OnPlayClicked);
	}
}

void UObsidianCharacterScreen::OnPlayClicked()
{
	if(UWorld* World = GetWorld())
	{
		UGameplayStatics::OpenLevel(World, FName("L_TestMap"));
	}
	
}
