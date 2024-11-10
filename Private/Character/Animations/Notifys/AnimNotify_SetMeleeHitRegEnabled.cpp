// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animations/Notifys/AnimNotify_SetMeleeHitRegEnabled.h"
#include "Character/GCBaseCharacter.h"
#include "Character/CharacterComponents/CharacterEquipmentComponent.h"
#include "Actors/Equipment/Weapons/MeleeWeaponItem.h"

void UAnimNotify_SetMeleeHitRegEnabled::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	AGCBaseCharacter* CharacterOwner = Cast<AGCBaseCharacter>(MeshComp->GetOwner());
	if (!IsValid(CharacterOwner))
	{
		return;
	}
	AMeleeWeaponItem* MeleeWeapon = CharacterOwner->GetCharacterEquipmentComponent()->GetCurrentMeleeWeaponItem();
	if (MeleeWeapon)
	{
		MeleeWeapon->SetIsHitRegistrationEnabled(bIsHitRegistrationEnabled);
	}
}
