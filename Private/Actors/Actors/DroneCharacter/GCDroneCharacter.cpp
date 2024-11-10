#include "Camera/CameraComponent.h"
#include "Actors/Actors/DroneCharacter/GCDroneCharacter.h"
#include "Components/Scene/ExplosionComponent.h"
#include "GameFramework/CharacterMovementCOmponent.h"
#include "Character/GCPlayerController.h"
#include "Character/GCBaseCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Actors/Actors/DroneCharacter/UI/DroneStatsWidget.h"
#include "Blueprint/UserWidget.h"

AGCDroneCharacter::AGCDroneCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Cam"));
	Camera->SetupAttachment(GetMesh());

	ExplosionComponent = CreateDefaultSubobject<UExplosionComponent>(TEXT("ExplosionComp"));
	ExplosionComponent->SetupAttachment(GetMesh());
	ExplosionComponent->SetIsReplicated(true);

	CurrentHealth = MaxHealth;

	GetCharacterMovement()->SetIsReplicated(true);
	bReplicates = true;
}

void AGCDroneCharacter::SetupPlayerInputComponent(UInputComponent* InputeComponent)
{
	Super::SetupPlayerInputComponent(InputeComponent);
	InputComponent = GetWorld()->GetFirstPlayerController()->FindComponentByClass<UInputComponent>();
	InputComponent->BindAxis("MoveForward", this, &AGCDroneCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AGCDroneCharacter::MoveRight);
	InputComponent->BindAxis("Turn", this, &AGCDroneCharacter::Turn);
	InputComponent->BindAxis("LookUp", this, &AGCDroneCharacter::LookUp);
	InputComponent->BindAxis("TurnAtRate", this, &AGCDroneCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUpAtRate", this, &AGCDroneCharacter::LookUpAtRate);
	InputComponent->BindAxis("MoveUp", this, &AGCDroneCharacter::MoveUp);
	InputComponent->BindAxis("SwimUp", this, &AGCDroneCharacter::MoveUp);
	InputComponent->BindAction("NextDrone", EInputEvent::IE_Pressed, this, &AGCDroneCharacter::NextDrone);
	InputComponent->BindAction("PreviousDrone", EInputEvent::IE_Pressed, this, &AGCDroneCharacter::PreviousDrone);
	InputComponent->BindAction("ReturnToPlayerControll", EInputEvent::IE_Pressed, this, &AGCDroneCharacter::ReturnToPlayerControll);
	InputComponent->BindAction("SelfDestruction", EInputEvent::IE_Pressed, this, &AGCDroneCharacter::SelfDestruction);
}

void AGCDroneCharacter::BeginPlay()
{
	Super::BeginPlay();
	OnTakeAnyDamage.AddDynamic(this, &AGCDroneCharacter::OnTakeDamage);
	CreateDroneWidget();
}

void AGCDroneCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGCDroneCharacter, bIsDead);
	DOREPLIFETIME(AGCDroneCharacter, TiltRotation);
}

void AGCDroneCharacter::MoveForward(float Value)
{
	if (bIsConnected == false)
	{
		return;
	}

	if (!FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
	
	if(GetLocalRole() == ROLE_AutonomousProxy)
	{
		FQuat TiltRot_In = FMath::Lerp(FQuat(GetMesh()->GetRelativeRotation()), FQuat(FRotator(0.0f, 0.0f, TiltAngle * Value)), TiltSpeedLerp);
		GetMesh()->SetRelativeRotation(TiltRot_In);
	}

	if (HasAuthority())
	{
		Server_SetRotationForward(Value);
	}
}

void AGCDroneCharacter::MoveRight(float Value)
{

	if (bIsConnected == false)
	{
		return;
	}

	if (!FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector	RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}

	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		FQuat TiltRot_In = FMath::Lerp(FQuat(GetMesh()->GetRelativeRotation()), FQuat(FRotator(0.0f, 0.0f, TiltAngle * Value)), TiltSpeedLerp);
		GetMesh()->SetRelativeRotation(TiltRot_In);
	}

	if (HasAuthority())
	{
		Server_SetRotationRight(Value);
	}
}

void AGCDroneCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AGCDroneCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AGCDroneCharacter::TurnAtRate(float Value)
{
	AddControllerYawInput(Value);
}

void AGCDroneCharacter::LookUpAtRate(float Value)
{
	AddControllerPitchInput(Value);
}

void AGCDroneCharacter::MoveUp(float Value)
{
	if (!FMath::IsNearlyZero(Value, 1e-6f))
	{
		AddMovementInput(FVector::UpVector, Value);
	}
}

void AGCDroneCharacter::NextDrone()
{
	if (bIsConnected == true)
	{
		TArray<AGCDroneCharacter*> DroneArray = CharacterOwner->GetDroneArray();
		if (DroneArray.Num() > 1)
		{
			int NextDrone = DroneArray.Num() + 1;
			if (DroneArray.Num() + 1 > DroneArray.Num())
			{
				NextDrone = 0;
			}

			AGCPlayerController* CurrentController = Cast<AGCPlayerController>(GetController());
			if (IsValid(CurrentController))
			{
				CurrentController->UnPossess();
				CurrentController->Possess(DroneArray[NextDrone]);
				SetDroneHUDVisibility(false);
				bIsConnected = false;
			}
		}
	}
}

void AGCDroneCharacter::PreviousDrone()
{
	if (bIsConnected == true)
	{
		TArray<AGCDroneCharacter*> DroneArray = CharacterOwner->GetDroneArray();
		if (DroneArray.Num() > 1)
		{
			int Previous = DroneArray.Num() - 1;
			if (DroneArray.Num() - 1 < DroneArray.Num())
			{
				Previous = DroneArray.Num() - 1;
			}

			AGCPlayerController* CurrentController = Cast<AGCPlayerController>(GetController());
			if (IsValid(CurrentController))
			{
				CurrentController->UnPossess();
				CurrentController->Possess(DroneArray[Previous]);
				SetDroneHUDVisibility(false);
				bIsConnected = false;
			}
		}
	}
}

void AGCDroneCharacter::ReturnToPlayerControll()
{
	if (IsValid(CharacterOwner))
	{
		AGCPlayerController* CurrentController = Cast<AGCPlayerController>(GetController());
		if (IsValid(CurrentController))
		{
			bIsConnected = false;
			CurrentController->UnPossess();
			SetDroneHUDVisibility(false);
			CurrentController->Possess(CharacterOwner);
		}
	}
}

void AGCDroneCharacter::SelfDestruction()
{
	if (bIsDead == false && bIsConnected == true)
	{
		bIsDead = true;

		if (HasAuthority())
		{
			Dead();
		}

		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			Server_Dead();
		}
	}
}

void AGCDroneCharacter::Dead()
{
	GetCharacterMovement()->CustomMovementMode = 0;
	GetCharacterMovement()->MaxWalkSpeed = 0.0f;
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	ReturnToPlayerControll();
	GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &AGCDroneCharacter::Exploring, TimeToExplosion, false);
}

void AGCDroneCharacter::Exploring()
{
	ExplosionComponent->Explore(GetController());
	GetMesh()->SetVisibility(false);
	Destroy();
}

void AGCDroneCharacter::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	CurrentHealth -= Damage;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
	if (CurrentHealth == 0.0f && bIsDead == false)
	{
		bIsDead = true;

		if (HasAuthority())
		{
			Dead();
		}
		if (GetLocalRole() == ROLE_AutonomousProxy)
		{
			Server_Dead();
		}
		UE_LOG(LogTemp, Warning, TEXT("Damaged"));
	}
}

void AGCDroneCharacter::SetPlayerOwner(AGCBaseCharacter* CharacterOwner_In)
{
	CharacterOwner = CharacterOwner_In;
}

FGenericTeamId AGCDroneCharacter::GetGenericTeamId() const
{ 
	return FGenericTeamId((uint8)Team);
}

/*  ~ServerFunctions~  */
void AGCDroneCharacter::Server_Dead_Implementation()
{
	Dead();
}


void AGCDroneCharacter::Server_SetRotationRight_Implementation(float Value)
{
	FQuat TiltRot_In = FMath::Lerp(FQuat(GetMesh()->GetRelativeRotation()), FQuat(FRotator(0.0f, 0.0f, TiltAngle * Value)), TiltSpeedLerp);
	GetMesh()->SetRelativeRotation(TiltRot_In);
	GetController()->SetControlRotation(GetMesh()->GetRelativeRotation());
	TiltRotation = FQuat(GetController()->GetControlRotation());
}

void AGCDroneCharacter::Server_SetRotationForward_Implementation(float Value)
{
	FQuat TiltRot_In = FMath::Lerp(FQuat(GetMesh()->GetRelativeRotation()), FQuat(FRotator(-TiltAngle * Value, 0.0f,0.0f)), TiltSpeedLerp);
	GetMesh()->SetRelativeRotation(TiltRot_In);
	GetController()->SetControlRotation(GetMesh()->GetRelativeRotation());
	TiltRotation = FQuat(GetController()->GetControlRotation());
}

/* ~RepFunctions~ */
void AGCDroneCharacter::OnRep_IsDead(bool bWasDead)
{
	if(bIsDead)
	{
		Dead();
	}
}

void AGCDroneCharacter::CreateDroneWidget()
{
	if (!IsValid(DroneHUDWidget))
	{
		DroneHUDWidget = CreateWidget<UDroneStatsWidget>(GetWorld(), DroneHUDWidgetClass);
		if (IsValid(DroneHUDWidget))
		{
			DroneHUDWidget->AddToViewport();
		}
	}
}

void AGCDroneCharacter::SetDroneHUDVisibility(bool bVisibility)
{
	if (!IsValid(DroneHUDWidget))
	{
		return;
	}
	if (bVisibility)
	{
		DroneHUDWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		DroneHUDWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}


