// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "InventoryItem.generated.h"

class UInventoryItem;
class AGCBaseCharacter;
class APickableItem;
class AEquipableItem;
USTRUCT(BlueprintType)
struct FInventoryItemDescription : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	UTexture2D* Icon;
};

USTRUCT(BlueprintType)
struct FWeaponTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon view")
	TSubclassOf<class APickableItem> PickableActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon view")
	TSubclassOf<class AEquipableItem> EquipableActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon view")
	FInventoryItemDescription WeaponItemDescription;
};

USTRUCT(BlueprintType)
struct FItemTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item view")
	TSubclassOf<class APickableItem> PickableActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon view")
	FInventoryItemDescription InventoryItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon view")
	TSubclassOf<class UInventoryItem> InventoryItemClass;
};

UCLASS(Blueprintable)
class GAMECODE_API UInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(FName DataTableID_In, const FInventoryItemDescription& Description_In);

	FName GetDataTableID() const;
	const FInventoryItemDescription& GetDescription() const;	

	virtual bool IsEquipable() const;
	virtual bool IsConsumable() const;

	virtual bool Consume(AGCBaseCharacter* ConsumeTarget) PURE_VIRTUAL(UInventoryItem::Cunsume, return false;);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category = "Inventory Item")
	FName DataTableID = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Item")
	FInventoryItemDescription Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Item")
	bool bIsEquipable = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory Item")
	bool bIsConsumable = false;

private:
	bool bIsInitialized = false;
};
