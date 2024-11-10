#include "Character/Animations/Notifys/AnimNotify_AttachEquippedItem.h"
#include "Character/GCBaseCharacter.h"
#include "Character/CharacterComponents/CharacterEquipmentComponent.h"

void UAnimNotify_AttachEquippedItem::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AGCBaseCharacter* CharacterOwner = Cast<AGCBaseCharacter>(MeshComp->GetOwner());

	if (!IsValid(CharacterOwner)) 
	{
		return;
	}
	CharacterOwner->GetCharacterEquipmentComponent_Mutable()->AttachCurrentItemToEquippedSocket();
}
