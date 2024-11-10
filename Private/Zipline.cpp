#include "Zipline.h"
#include "Components/BoxComponent.h"
#include "GameCode/GameCodeTypes.h"
#include "Kismet/KismetMathLibrary.h"

AZipline::AZipline()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ZiplineRoot"));

	FirstPillar = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FirstPillar"));
	FirstPillar->SetupAttachment(RootComponent);

	SecondPillar = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SecondPillar"));
	SecondPillar->SetupAttachment(RootComponent);

	Cable = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ZiplineCable"));
	Cable->SetupAttachment(RootComponent);

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction volume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(CollisionProfilePawmInteraction);
	InteractionVolume->SetGenerateOverlapEvents(true);
}

void AZipline::OnConstruction(const FTransform& Transform)
{
	FirstPillar->SetRelativeLocation(FirstTrans);
	SecondPillar->SetRelativeLocation(SecondTrans);

	FVector FirstVector = FirstPillar->GetRelativeLocation();
	FVector SecondVector = SecondPillar->GetRelativeLocation();

	float CableSize = sqrt(pow(SecondVector.Z, 2) + pow(SecondVector.Y, 2));
	FVector CableVector = (SecondVector + FirstVector) / 2;
	Cable->SetRelativeScale3D(FVector(1.0f, CableSize * 0.01f, 1.0f));
	Cable->SetRelativeLocation(CableVector);
	Cable->SetRelativeRotation(FRotator(0.0f, 0.0f, atan(SecondVector.Z / SecondVector.Y) * -60));

	if (FirstVector.Z >= SecondVector.Z)
	{
		InteractionVolume->SetRelativeLocation(FirstVector);
		StartZiplineVector = UKismetMathLibrary::TransformLocation(GetTransform(), FirstVector);
		EndZiplineVector = UKismetMathLibrary::TransformLocation(GetTransform(), SecondVector);
	}
	else
	{
		InteractionVolume->SetRelativeLocation(SecondVector);
		StartZiplineVector = UKismetMathLibrary::TransformLocation(GetTransform(), SecondVector); 
		EndZiplineVector = UKismetMathLibrary::TransformLocation(GetTransform(), FirstVector);
	}
}

void AZipline::BeginPlay()
{
	Super::BeginPlay();
}

void AZipline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



