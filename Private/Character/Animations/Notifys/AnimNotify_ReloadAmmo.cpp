// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animations/Notifys/AnimNotify_ReloadAmmo.h"
#include "Character/GCBaseCharacter.h"
#include "Character/CharacterComponents/CharacterEquipmentComponent.h"

void UAnimNotify_ReloadAmmo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	AGCBaseCharacter* CharacterOwner = Cast<AGCBaseCharacter>(MeshComp->GetOwner());
	if (!IsValid(CharacterOwner))
	{
		return;
	}
	CharacterOwner->GetCharacterEquipmentComponent_Mutable()->ReloadAmmoInCurrentWeapon(NumberOfAmmo);
}
