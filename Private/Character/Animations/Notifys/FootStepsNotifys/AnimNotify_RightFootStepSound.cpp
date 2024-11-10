#include "Character/Animations/Notifys/FootStepsNotifys/AnimNotify_RightFootStepSound.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundCue.h"
#include "Character/GCBaseCharacter.h"

void UAnimNotify_RightFootStepSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AGCBaseCharacter* CharacterOwner = Cast<AGCBaseCharacter>(MeshComp->GetOwner());

	if (!IsValid(CharacterOwner))
	{
		return;
	}

	FVector RightFootStepSocketLocation = CharacterOwner->GetMesh()->GetSocketLocation("RightFootStepSocket");

	if (IsValid(FootStepSound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, FootStepSound, RightFootStepSocketLocation);
	}

	if (IsValid(FootStepVFX))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FootStepVFX, RightFootStepSocketLocation);
	}
}