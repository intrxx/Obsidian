// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Plugin


#include "OOSInventoryItemsManagerComponent.generated.h"

class UOOSInventoryItemsInputDefinition;

/**
 * Component that manages Inventory Items related input and actions, like dragging items.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OOSINVENTORYITEMS_API UOOSInventoryItemsManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOOSInventoryItemsManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "OOSInventoryItems|ManagerComponent")
	void InitializePlayerInventoryInput(UInputComponent* InputComponent);

	/** Returns the hero component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "Obsidian|HeroComp")
	static UOOSInventoryItemsManagerComponent* FindInventoryItemsManagerComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UOOSInventoryItemsManagerComponent>() : nullptr);
	}

	/** Gets the pawn that owns the component, this will always be valid during gameplay but can return null in the editor */
	template <class T>
	T* GetPawn() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value, "'T' template parameter to GetPawn must be derived from APawn");
		return Cast<T>(GetOwner());
	}

	/** Gets the controller that owns the component, this will usually be null on clients */
	template <class T>
	T* GetController() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, AController>::Value, "'T' template parameter to GetController must be derived from AController");
		return GetPawnChecked<APawn>()->GetController<T>();
	}

	template <class T>
	T* GetPawnChecked() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value, "'T' template parameter to GetPawnChecked must be derived from APawn");
		return CastChecked<T>(GetOwner());
	}
	
protected:
	virtual void BeginPlay() override;

	void Input_DropItem();
	void Input_ReleaseUsingItem();

	void Input_WeaponSwap();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "OOSInventoryItems|Input")
	TObjectPtr<UOOSInventoryItemsInputDefinition> InventoryItemsInputDefinition;
};
