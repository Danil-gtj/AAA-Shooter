#include "Character/Animations/Notifys/FootStepsNotifys/AnimNotify_FootStepSound.h"
#include "Character/GCBaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundCue.h"

void UAnimNotify_FootStepSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AGCBaseCharacter* CharacterOwner = Cast<AGCBaseCharacter>(MeshComp->GetOwner());

	if (!IsValid(CharacterOwner))
	{
		return;
	}

	FVector LeftFootStepSocketLocation = CharacterOwner->GetMesh()->GetSocketLocation("LeftFootStepSocket");

	if (IsValid(FootStepSound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, FootStepSound, LeftFootStepSocketLocation);
	}

	if (IsValid(FootStepVFX))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FootStepVFX, LeftFootStepSocketLocation);
	}
}
