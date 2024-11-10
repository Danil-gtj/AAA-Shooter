#include "Actors/Actors/Controllers/DroneController.h"
#include "Blueprint/UserWidget.h"
#include "Actors/Actors/DroneCharacter/GCDroneCharacter.h"
#include "Actors/Actors/DroneCharacter/UI/DroneStatsWidget.h"

void ADroneController::BeginPlay()
{
	Super::BeginPlay();
	CachedBaseDrone = Cast<AGCDroneCharacter>(GetCharacter());
	CreateWidgetInit();
}

void ADroneController::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	CachedBaseDrone = Cast<AGCDroneCharacter>(NewOwner);
}

void ADroneController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (IsValid(DroneIntefaceWidget) && HasAuthority())
	{
		DroneIntefaceWidget->SetReticlePositionInViewport();
	}
}

void ADroneController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &ADroneController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ADroneController::MoveRight);
	InputComponent->BindAxis("Turn", this, &ADroneController::Turn);
	InputComponent->BindAxis("LookUp", this, &ADroneController::LookUp);
	InputComponent->BindAxis("TurnAtRate", this, &ADroneController::TurnAtRate);
	InputComponent->BindAxis("LookUpAtRate", this, &ADroneController::LookUpAtRate);
	InputComponent->BindAxis("MoveUp", this, &ADroneController::MoveUp);
	InputComponent->BindAxis("SwimUp", this, &ADroneController::MoveUp);
	InputComponent->BindAction("NextDrone", EInputEvent::IE_Pressed, this, &ADroneController::NextDrone);
	InputComponent->BindAction("PreviousDrone", EInputEvent::IE_Pressed, this, &ADroneController::PreviousDrone);
	InputComponent->BindAction("ReturnToPlayerControll", EInputEvent::IE_Pressed, this, &ADroneController::ReturnToPlayerControll);
	InputComponent->BindAction("SelfDestruction", EInputEvent::IE_Pressed, this, &ADroneController::SelfDestruction);
}

void ADroneController::MoveForward(float Value)
{
	if (CachedBaseDrone.IsValid())
	{
		CachedBaseDrone->MoveForward(Value);
		if (IsValid(DroneIntefaceWidget))
		{
			DroneIntefaceWidget->SetYReticlePosition(Value);
		}
	}
}

void ADroneController::MoveRight(float Value)
{
	if (CachedBaseDrone.IsValid())
	{
		CachedBaseDrone->MoveRight(Value);
		if (IsValid(DroneIntefaceWidget))
		{
			DroneIntefaceWidget->SetXReticlePosition(Value);
		}
	}
}

void ADroneController::Turn(float Value)
{
	if (CachedBaseDrone.IsValid())
	{
		CachedBaseDrone->Turn(Value);
	}
}

void ADroneController::LookUp(float Value)
{
	if (CachedBaseDrone.IsValid())
	{
		CachedBaseDrone->LookUp(Value);
	}
}

void ADroneController::TurnAtRate(float Value)
{
	if (CachedBaseDrone.IsValid())
	{
		CachedBaseDrone->TurnAtRate(Value);
	}
}

void ADroneController::LookUpAtRate(float Value)
{
	if (CachedBaseDrone.IsValid())
	{
		CachedBaseDrone->LookUpAtRate(Value);
	}
}

void ADroneController::MoveUp(float Value)
{
	if (CachedBaseDrone.IsValid())
	{
		CachedBaseDrone->MoveUp(Value);
	}
}

void ADroneController::NextDrone()
{
	if (CachedBaseDrone.IsValid())
	{
		CachedBaseDrone->NextDrone();
	}
}

void ADroneController::PreviousDrone()
{
	if (CachedBaseDrone.IsValid())
	{
		CachedBaseDrone->PreviousDrone();
	}
}

void ADroneController::ReturnToPlayerControll()
{
	if (CachedBaseDrone.IsValid())
	{
		CachedBaseDrone->ReturnToPlayerControll();
	}
}

void ADroneController::SelfDestruction()
{
	if (CachedBaseDrone.IsValid())
	{
		CachedBaseDrone->SelfDestruction();
	}
}

void ADroneController::CreateWidgetInit()
{
	if (!IsValid(DroneIntefaceWidget))
	{
		DroneIntefaceWidget = CreateWidget<UDroneStatsWidget>(GetWorld(), DroneHUDWidgetClass);
		if (IsValid(DroneIntefaceWidget))
		{
			DroneIntefaceWidget->AddToViewport();
		}
	}
}