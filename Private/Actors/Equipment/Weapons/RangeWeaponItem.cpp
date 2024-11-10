#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Components/WeaponBarellComponent.h"
#include "GameCode/GameCodeTypes.h"
#include "Character/GCBaseCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraShakeBase.h"

ARangeWeaponItem::ARangeWeaponItem()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	WeaponBarell = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarell"));
	WeaponBarell->SetupAttachment(WeaponMesh, SocketWeaponMuzzle);

	ReticleType = EReticleType::Default;

	EquippedSocketName = SocketCharacterWeapon;

	bReplicates = true;
}

void ARangeWeaponItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARangeWeaponItem, Ammo);
}


void ARangeWeaponItem::StartFire()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(ShotTimer))
	{
		return;
	}
	bIsFiring = true;
	MakeShot();
}

void ARangeWeaponItem::StopFire()
{
	bIsFiring = false;
}

bool ARangeWeaponItem::IsFiring() const
{
	return bIsFiring;
}

void ARangeWeaponItem::StartAim()
{
	bIsAiming = true;
	CurrentAimingLookUpModifier = AimingLookUpModifier;
	CurrentAimingTurnModifier = AimingTurnModifier;
}

void ARangeWeaponItem::StopAim()
{
	bIsAiming = false;
	CurrentAimingLookUpModifier = 1.0f;
	CurrentAimingTurnModifier = 1.0f;
}

void ARangeWeaponItem::StartReload()
{
	AGCBaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!IsValid(CharacterOwner))
	{
		return;
	}

	if (Ammo == MaxAmmo || bIsReloading) 
	{
		return;
	}

	bIsReloading = true;
	if (IsValid(CharacterReloadMontage)) 
	{
		float MontageDuration = CharacterOwner->PlayAnimMontage(CharacterReloadMontage);
		PlayAnimMontage(WeaponReloadMontage);
		if (ReloadType == EReloadType::FullClip) 
		{
			GetWorld()->GetTimerManager().SetTimer(ReloadTimer, [this]() { EndReload(true); }, MontageDuration, false);
		}
	}
	else 
	{
		EndReload(true);
	}
}

void ARangeWeaponItem::EndReload(bool bIsSuccess)
{
	if (!bIsReloading) 
	{
		return;
	}

	AGCBaseCharacter* CharacterOwner = GetCharacterOwner();

	if (!bIsSuccess) 
	{
		if (IsValid(CharacterOwner))
		{
			CharacterOwner->StopAnimMontage(CharacterReloadMontage);
		}
		StopAnimMontage(WeaponReloadMontage);
	}

	if (ReloadType == EReloadType::ByBullet) 
	{
		UAnimInstance* CharAnimInstance = IsValid(CharacterOwner) ? CharacterOwner->GetMesh()->GetAnimInstance() : nullptr;
		if (IsValid(CharAnimInstance))
		{
			CharAnimInstance->Montage_JumpToSection(SectionMontageReloadEnd, CharacterReloadMontage);
		}

		UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
		if (IsValid(WeaponAnimInstance))
		{
			WeaponAnimInstance->Montage_JumpToSection(SectionMontageReloadEnd, WeaponReloadMontage);
		}
	}

	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);

	bIsReloading = false;
	if (bIsSuccess && OnReloadComplete.IsBound())
	{
		OnReloadComplete.Broadcast();
	}
}

void ARangeWeaponItem::Server_Reliable_StartReaload_Implementation()
{
	Multicast_Reliable_StartReload();
}

void ARangeWeaponItem::Multicast_Reliable_StartReload_Implementation()
{
	StartReload();
}

void ARangeWeaponItem::Server_PlayAnimMontage_Implementation()
{
	Multicast_PlayAnimMontage();
}

void ARangeWeaponItem::Multicast_PlayAnimMontage_Implementation()
{
	PlayAnimMontage(WeaponFireMontage);
}

bool ARangeWeaponItem::IsReloading() const
{
	return bIsReloading;
}

float ARangeWeaponItem::GetAimFOV() const
{
	return AimFOV;
}

float ARangeWeaponItem::GetAimMovementMaxSpeed() const
{
	return AimMovementMaxSpeed;
}

int32 ARangeWeaponItem::GetAmmo() const
{
	return Ammo;
}

int32 ARangeWeaponItem::GetMaxAmmo() const
{
	return MaxAmmo;
}

void ARangeWeaponItem::SetAmmo(int32 NewAmmo) 
{
	Ammo = NewAmmo;
	if (OnAmmoChanged.IsBound())
	{
		OnAmmoChanged.Broadcast(Ammo);
	}
}

EAmunitionType ARangeWeaponItem::GetAmmoType() const
{
	return AmmoType;
}

bool ARangeWeaponItem::CanShoot() const
{
	return Ammo > 0;
}

FTransform ARangeWeaponItem::GetForeGripTransform() const
{
	return WeaponMesh->GetSocketTransform(SocketWeaponForeGrip);
}

float ARangeWeaponItem::GetAimingTurnModifier() const
{
	return CurrentAimingTurnModifier;
}

float ARangeWeaponItem::GetAimingLookUpModifier() const
{
	return CurrentAimingLookUpModifier;
}

EReticleType ARangeWeaponItem::GetReticleType() const
{
	return bIsAiming ? AimReticleType : ReticleType;
}

void ARangeWeaponItem::OnLevelDeserialized_Implementation()
{
	SetActorRelativeTransform(FTransform(FRotator::ZeroRotator, FVector::ZeroVector));
	if (OnAmmoChanged.IsBound())
	{
		OnAmmoChanged.Broadcast(Ammo);
	}
}

void ARangeWeaponItem::BeginPlay()
{
	Super::BeginPlay();
	SetAmmo(MaxAmmo);
}

float ARangeWeaponItem::GetCurrentSpreadAngle() const
{
	float AngleInDegres = bIsAiming ? AimSpreadAngle : SpreadAngle;
	return FMath::DegreesToRadians(AngleInDegres);
}

float ARangeWeaponItem::GetShotTimerInterval() const
{
	return 60.0f / RateOfFire;
}


float ARangeWeaponItem::PlayAnimMontage(UAnimMontage* AnimMontage)
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	float Result = 0.0f;
	if (IsValid(WeaponAnimInstance))
	{
		Result = WeaponAnimInstance->Montage_Play(AnimMontage);
	}
	return Result;
}

void ARangeWeaponItem::MakeShot()
{
	AGCBaseCharacter* CharacterOwner = GetCharacterOwner();
	if (!IsValid(CharacterOwner)) 
	{
		return;
	}

	if (!CanShoot()) 
	{
		StopFire();
		if (Ammo == 0 && bAutoReload)
		{
			CharacterOwner->Reload();
		}
		return;
	}

	EndReload(false);
	CharacterOwner->Server_PlayAnimMontage(CharacterFireMontage);
	Server_PlayAnimMontage();

	FVector ShotLocation;
	FRotator ShotRotation;
	if (CharacterOwner->IsPlayerControlled())
	{
		APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
		Controller->GetPlayerViewPoint(ShotLocation, ShotRotation);
		ShotLocation = WeaponBarell->GetComponentLocation();
		ShotRotation = CharacterOwner->GetBaseAimRotation();
	}
	else
	{
		ShotLocation = WeaponBarell->GetComponentLocation();
		ShotRotation = CharacterOwner->GetBaseAimRotation();
	}

	FVector ShotDirection = ShotRotation.RotateVector(FVector::ForwardVector);

	UGameplayStatics::PlayWorldCameraShake(this, CShake, CharacterOwner->GetActorLocation(), 0, 300, 1.f, false);

	SetAmmo(Ammo - 1);
	WeaponBarell->Shot(ShotLocation, ShotDirection, GetCurrentSpreadAngle());

	GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ARangeWeaponItem::OnShotTimerElapsed, GetShotTimerInterval(), false);
}

void ARangeWeaponItem::OnShotTimerElapsed()
{
	if (!bIsFiring) 
	{
		return;
	}

	switch (WeaponFireMode)
	{
		case EWeaponFireMode::Single:
		{
			StopFire();
			break;
		}
		case EWeaponFireMode::FullAuto: 
		{
			MakeShot();
		}
	}
}

void ARangeWeaponItem::StopAnimMontage(UAnimMontage* AnimMontage, float BlendOutTime)
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	if (IsValid(WeaponAnimInstance))
	{
		WeaponAnimInstance->Montage_Stop(BlendOutTime, AnimMontage);
	}
}
