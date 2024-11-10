#include "AI/Characters/Turret.h"
#include "Components/WeaponBarellComponent.h"
#include "AIController.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AISense_Damage.h"
#include "Components/Scene/ExplosionComponent.h"

ATurret::ATurret()
{
	PrimaryActorTick.bCanEverTick = true;
	
	USceneComponent* TurretRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TurretRoot"));
	SetRootComponent(TurretRoot);

	TurretBaseComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TurretBase"));
	TurretBaseComponent->SetupAttachment(TurretRoot);

	TurretBarrelComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TurretBarell"));
	TurretBarrelComponent->SetupAttachment(TurretBaseComponent);

	WeaponBarrel = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarrel"));
	WeaponBarrel->SetupAttachment(TurretBarrelComponent);

	ExplosionComponent = CreateDefaultSubobject<UExplosionComponent>(TEXT("ExplosionComp"));
	ExplosionComponent->SetupAttachment(RootComponent);
	ExplosionComponent->SetIsReplicated(true);

	SetReplicates(true);
}

void ATurret::BeginPlay()
{
	Super::BeginPlay();
	OnTakeAnyDamage.AddDynamic(this, &ATurret::OnTakeDamage);

	Health = MaxHealth;
}

void ATurret::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AAIController* AIController = Cast<AAIController>(NewController);
	if (IsValid(AIController))
	{
		FGenericTeamId TeamId((uint8)Team);
		AIController->SetGenericTeamId(TeamId);
	}
}

void ATurret::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATurret, CurrentTarget);
}

void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDead) 
	{
		return;
	}

	switch (CurrentTurretState)
	{
		case ETurretState::Searching:
		{
			SearchingMovement(DeltaTime);
			break;
		}
		case ETurretState::Firing:
		{
			FiringMovement(DeltaTime);
			break;
		}
	}
}

void ATurret::SetCurrentTarget()
{
	ETurretState NewState = IsValid(CurrentTarget) ? ETurretState::Firing : ETurretState::Searching;
	SetCurrentTurretState(NewState);
}

FVector ATurret::GetPawnViewLocation() const
{
	return WeaponBarrel->GetComponentLocation();
}

FRotator ATurret::GetViewRotation() const
{
	return WeaponBarrel->GetComponentRotation();
}

void ATurret::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UAISense_Damage* Sence_Damage;
	FVector EventLocation = DamageCauser->GetActorLocation();
	Sence_Damage->ReportDamageEvent(GetWorld(), DamagedActor, DamageCauser, Damage, EventLocation, EventLocation);
	Health -= Damage;
	if (Health <= 0.0f && bDead == false)
	{
		SetActorHiddenInGame(true);
		bDead = true;
		ExplosionComponent->Explode(GetController());
		GetWorld()->GetTimerManager().SetTimer(TimerDestroy, this, &ATurret::DestroyTurret, TimeToDestroy, false);
	}
}

void ATurret::SearchingMovement(float DeltaTime)
{
	FRotator TurretBaseRotaion = TurretBaseComponent->GetRelativeRotation();
	TurretBaseRotaion.Yaw += DeltaTime * BaseSearchingRotationRate;
	TurretBaseComponent->SetRelativeRotation(TurretBaseRotaion);

	FRotator TurretBarellRotaion = TurretBarrelComponent->GetRelativeRotation();
	TurretBarellRotaion.Pitch = FMath::FInterpTo(TurretBarellRotaion.Pitch, 0.0f, DeltaTime, BarellPitchRotationRate);
	TurretBarrelComponent->SetRelativeRotation(TurretBarellRotaion);
}

void ATurret::FiringMovement(float DeltaTime)
{
	FVector	BaseLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBaseComponent->GetComponentLocation()).GetSafeNormal2D();
	FQuat LookAtQuat = BaseLookAtDirection.ToOrientationQuat();
	FQuat TargetQuat = FMath::QInterpTo(TurretBaseComponent->GetComponentQuat(), LookAtQuat, DeltaTime, BaseFiringInterpSpeed);
	TurretBaseComponent->SetWorldRotation(TargetQuat);

	FVector BarellLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBarrelComponent->GetComponentLocation()).GetSafeNormal();
	float LookAtPitchAngle = BarellLookAtDirection.ToOrientationRotator().Pitch;
	
	FRotator BarellLocalRotation = TurretBarrelComponent->GetRelativeRotation();
	BarellLocalRotation.Pitch = FMath::FInterpTo(BarellLocalRotation.Pitch, LookAtPitchAngle, DeltaTime, BarellPitchRotationRate);
	TurretBarrelComponent->SetRelativeRotation(BarellLocalRotation);
}

void ATurret::SetCurrentTurretState(ETurretState NewState)
{
	bool bIsStateChanged = NewState != CurrentTurretState;
	CurrentTurretState = NewState; 
	if (!bIsStateChanged)
	{
		return;
	}

	switch (CurrentTurretState)
	{
		case ETurretState::Searching:
		{
			GetWorld()->GetTimerManager().ClearTimer(ShotTimer);
			break;
		}
		case ETurretState::Firing:
		{
			GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ATurret::MakeShot, GetFireInterval(), true, FireDelayTime);
			break;
		}
	}
}

float ATurret::GetFireInterval() const
{
	return 60.0f / RateOfFire;
}

void ATurret::MakeShot()
{
	if (bDead)
	{
		return;
	}

	FVector ShotLocation = WeaponBarrel->GetComponentLocation();
	FVector ShotDirection = WeaponBarrel->GetComponentRotation().RotateVector(FVector::ForwardVector);
	float SpredAngle = FMath::DegreesToRadians(BulletSpreadAngle);
	WeaponBarrel->Shot(ShotLocation, ShotDirection, SpredAngle);
}

void ATurret::OnRep_CurrentTarget()
{
	SetCurrentTarget();
}

void ATurret::DestroyTurret()
{
	Destroy();
}

