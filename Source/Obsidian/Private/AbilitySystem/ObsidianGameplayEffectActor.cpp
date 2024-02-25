// Copyright 2024 Michał Ogiński


#include "AbilitySystem/ObsidianGameplayEffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/ObsidianCommonAttributeSet.h"
#include "Characters/ObsidianCharacterBase.h"
#include "Components/SphereComponent.h"

AObsidianGameplayEffectActor::AObsidianGameplayEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(MeshComp);
	
	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphereComp"));
	OverlapSphere->SetupAttachment(GetRootComponent());
}

void AObsidianGameplayEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);
}

void AObsidianGameplayEffectActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//TODO Change this to apply a Gameplay Effect, for now just testing
	if(AObsidianCharacterBase* OverlappingCharacter = Cast<AObsidianCharacterBase>(OtherActor))
	{
		const UObsidianCommonAttributeSet* Set = Cast<UObsidianCommonAttributeSet>(OverlappingCharacter->GetAbilitySystemComponent()->GetAttributeSet(UObsidianCommonAttributeSet::StaticClass()));
		UObsidianCommonAttributeSet* MutableSet = const_cast<UObsidianCommonAttributeSet*>(Set);
		MutableSet->SetHealth(Set->GetHealth() + 10.f);

		Destroy();
	}
}

void AObsidianGameplayEffectActor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}


