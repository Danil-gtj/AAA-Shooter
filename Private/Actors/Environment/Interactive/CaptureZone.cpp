// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Environment/Interactive/CaptureZone.h"
#include "GameStates/CaptureSharpensGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Character/GCBaseCharacter.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ACaptureZone::ACaptureZone()
{
	PrimaryActorTick.bCanEverTick = true;
	InteractionVolume = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IntVolume"));
}

void ACaptureZone::BeginPlay()
{
	Super::BeginPlay();
	InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &ACaptureZone::OnOverlapBegin);
	InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &ACaptureZone::OnOverlapEnd);
}

void ACaptureZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsOverlappingCharacterCapsule(OtherActor, OtherComp))
	{
		return;
	}
	AGCBaseCharacter* BaseCharacter = Cast<AGCBaseCharacter>(OtherActor);
	ACaptureSharpensGameState* GameState = Cast<ACaptureSharpensGameState>(UGameplayStatics::GetGameState(GetWorld()));
	GameState->RegisterlayerToZone(BaseCharacter);
}

void ACaptureZone::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsOverlappingCharacterCapsule(OtherActor, OtherComp))
	{
		return;
	}
	AGCBaseCharacter* BaseCharacter = Cast<AGCBaseCharacter>(OtherActor);
	ACaptureSharpensGameState* GameState = Cast<ACaptureSharpensGameState>(UGameplayStatics::GetGameState(GetWorld()));
	GameState->UnregisterPlayerFromZone(BaseCharacter);
}

bool ACaptureZone::IsOverlappingCharacterCapsule(AActor* OtherActor, UPrimitiveComponent* OtherComp)
{
	AGCBaseCharacter* BaseCharacter = Cast<AGCBaseCharacter>(OtherActor);
	if (!IsValid(BaseCharacter))
	{
		return false;
	}

	if (Cast<UCapsuleComponent>(OtherComp) != BaseCharacter->GetCapsuleComponent())
	{
		return false;
	}
	return true;
}


