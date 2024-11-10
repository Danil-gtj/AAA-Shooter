#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "GameCode/GameCodeTypes.h"
#include "CharacterEquipmentComponent.generated.h"

typedef TArray<class AEquipableItem*, TInlineAllocator<(uint32)EEquipmentSlots::MAX>> TItemsArray;

typedef TArray<int32, TInlineAllocator<(uint32)EAmunitionType::MAX>> TAmunitionArray;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentWeaponAmmoChangedEvent, int32, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCurrentFragGranadesChangedEvent, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquippedItemChanged, const AEquipableItem*);

class UWeaponWheelWidget;
class UEquipmentViewWidget;
class AThrowableItem;
class ARangeWeaponItem;
class AMeleeWeaponItem;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMECODE_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCharacterEquipmentComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	EEquipableItemType GetCurrentEquippedItemType() const;

	ARangeWeaponItem* GetCurrentRrangeWeapon() const;

	AMeleeWeaponItem* GetCurrentMeleeWeaponItem() const;

	bool IsEquipping() const;

	void ReloadCurrenWeapon();

	void ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo = 0, bool bCheckIsFull = false);

	FOnCurrentWeaponAmmoChangedEvent OnCurrentWeaponAmmoChangedEvent;

	FOnCurrentFragGranadesChangedEvent OnCurrentFragGranadesChangedEvent;

	FOnEquippedItemChanged OnEquippedItemChanged;

	void EquipItemInSlot(EEquipmentSlots Slot);

	void AttachCurrentItemToEquippedSocket();

	void LaunchCurrentThrowableItem();

	void UnEquipCurrentItem();

	void EquipNextItem();
	void EquipPreviousItem();

	bool AddEquipmentItem(const TSubclassOf<AEquipableItem> EquipableItemClass, int32 SlotIndex);
	void RemoveItemFromSlot(int32 SlotIndex);

	void CloseViewEquipment();
	bool IsViewVisible() const;

	bool IsSelectingWeapon() const;

	void ConfirmWeaponSelection();

	const TArray<AEquipableItem*>& GetItems() const;

	virtual void OnLevelDeserialized_Implementation();

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EAmunitionType, int32> MaxAmunitionAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EEquipmentSlots, TSubclassOf<class AEquipableItem>> ItemsLoadout;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSet<EEquipmentSlots> IgnoreSlotWhileSwitching;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	EEquipmentSlots AutoEquipItemInSlot = EEquipmentSlots::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loadout")
	int32 FragGranades = 3;

private:
	UFUNCTION(Server, Reliable)
	void Server_LaunchCurrentThrowableItem();

	UFUNCTION(Server, Reliable)
	void Server_EquipItemInSlot(EEquipmentSlots Slot);

	UPROPERTY(Replicated, SaveGame)
	TArray<int32> AmunitionArray;
	
	UPROPERTY(ReplicatedUsing=OnRep_ItemsArray)
	TArray<AEquipableItem*> ItemsArray;

	UFUNCTION()
	void OnRep_ItemsArray();

	void CreateLoadout();

	void AutoEquip();

	void EquipAnimationFinished();

	int32 GetAvalibleAmunitionForWeapon();

	uint32 NextItemsArraySlotIndex(uint32 CurrentSlotIndex);
	uint32 PreviousItemsSlotIndex(uint32 CurrentSlotIndex);

	bool bIsEquipping = false;

	UFUNCTION()
	void OnWeaponReloadComplite();

	UFUNCTION()
	void OnCurrentWeaponAmmoChanged(int32 Ammo);

	UFUNCTION()
	void OnCurrentFragGranadesChanged(int32 FragGranades_In);

	FDelegateHandle OnCurrentWeaponAmmoChangedHandle;
	FDelegateHandle OnCurrentWeaponReloadHandle;

	EEquipmentSlots PreviusEquippedSlot;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentEquipSlot)
	EEquipmentSlots CurrentEquippedSlot;

	UFUNCTION()
	void OnRep_CurrentEquipSlot(EEquipmentSlots CurrentEquipSlotOld);

	AEquipableItem* CurrentEquippedItem = nullptr;
	ARangeWeaponItem* CurrentEquippedWeapon = nullptr;
	AThrowableItem* CurrentThrowableItem = nullptr;
	AMeleeWeaponItem* CurrentMeleeWeapon = nullptr;

	TWeakObjectPtr<class AGCBaseCharacter> CachedBaseCharacter;

	FTimerHandle EquipTimer;

	UEquipmentViewWidget* ViewWidget;
	UWeaponWheelWidget* WeaponWheelWidget;
};
