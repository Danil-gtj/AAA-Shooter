// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animations/Notifys/AnimNotify_LaunchThrowable.h"
#include "Character/GCBaseCharacter.h"

void UAnimNotify_LaunchThrowable::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AGCBaseCharacter* CharacterOwner = Cast<AGCBaseCharacter>(MeshComp->GetOwner());
	if (!IsValid(CharacterOwner))
	{
		return;
	}
	CharacterOwner->GetCharacterEquipmentComponent_Mutable()->LaunchCurrentThrowableItem();
}
