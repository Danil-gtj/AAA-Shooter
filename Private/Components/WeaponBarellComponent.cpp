#include "Components/WeaponBarellComponent.h"
#include "GameCode/GameCodeTypes.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "Actors/Projectiles/GCProjectile.h"
#include "Engine/DamageEvents.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Sound/SoundCue.h"

UWeaponBarellComponent::UWeaponBarellComponent()
{
	SetIsReplicatedByDefault(true);
}

void UWeaponBarellComponent::Shot(FVector ShotStart, FVector ShotDirection, float SpreadAngle)
{
	TArray<FShotInfo> ShotsInfo;
	for (int i = 0; i < BulletsPerShot; i++)
	{
		ShotDirection += GetBulletSpreadOffset(FMath::RandRange(0.0f, SpreadAngle), ShotDirection.ToOrientationRotator());
		ShotDirection = ShotDirection.GetSafeNormal();
		ShotsInfo.Emplace(ShotStart, ShotDirection);
	}

	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy) 
	{
		Server_Shot(ShotsInfo);
	}

	ShotInternal(ShotsInfo);
}

void UWeaponBarellComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	FDoRepLifetimeParams RepParams;
	RepParams.Condition = COND_SimulatedOnly;
	RepParams.RepNotifyCondition = REPNOTIFY_Always;
	DOREPLIFETIME_WITH_PARAMS(UWeaponBarellComponent, LastShotsInfo, RepParams);
	DOREPLIFETIME(UWeaponBarellComponent, ProjectilePool);
	DOREPLIFETIME(UWeaponBarellComponent, CurrentProjectileIndex);
}

bool UWeaponBarellComponent::HitScan(FVector ShotStart,OUT FVector& ShotEnd, FVector ShotDirection)
{
	FHitResult ShotResult;
	bool bHasHit = GetWorld()->LineTraceSingleByChannel(ShotResult, ShotStart, ShotEnd, ECC_Bullet);
	if (bHasHit)
	{
		ShotEnd = ShotResult.ImpactPoint;
		ProcessHit(ShotResult, ShotDirection);
	}
	return bHasHit;
}

void UWeaponBarellComponent::ShotInternal(const TArray<FShotInfo>& ShotsInfo)
{
	if (GetOwner()->HasAuthority()) 
	{ 
		LastShotsInfo = ShotsInfo;
	}

	FVector MuzzleLocation = GetComponentLocation();
	if (IsValid(MuzzleFlashFX))
	{
		
		FTransform Trans;
		Trans.SetLocation(MuzzleLocation);
		Trans.SetRotation(GetComponentQuat());
		Trans.SetScale3D(FVector(0.1f, 0.1f, 0.1f));
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlashFX, Trans);
	}

	if (IsValid(FireSFX))
	{ 
		UGameplayStatics::SpawnSoundAtLocation(this, FireSFX, GetComponentLocation(), GetComponentRotation(), true);
	}

	for (const FShotInfo& ShotInfo : ShotsInfo)
	{
		FVector ShotStart = ShotInfo.GetLocation();
		FVector ShotDirection = ShotInfo.GetDirection();
		FVector ShotEnd = ShotStart + FiringRange * ShotDirection;

		switch (HitRegestrationType)
		{
		case EHitRegestrationType::HitScan:
		{
			bool bHasHit = HitScan(ShotStart, ShotEnd, ShotDirection);
			if (IsValid(TraceFX))
			{
				UNiagaraComponent* TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TraceFX, MuzzleLocation, GetComponentRotation());
				if (IsValid(TraceFXComponent)) 
				{
					TraceFXComponent->SetVectorParameter(FXParamTraceEnd, ShotEnd);
				}
			}
			break;
		}
		case EHitRegestrationType::Projectile:
		{
			LaunchProjectile(ShotStart, ShotDirection);
			break;
		}
		}
	}
}

void UWeaponBarellComponent::Server_Shot_Implementation(const TArray<FShotInfo>& ShotsInfo)
{
	ShotInternal(ShotsInfo);
}

void UWeaponBarellComponent::OnRep_LastShotsInfo()
{
	ShotInternal(LastShotsInfo);
}

APawn* UWeaponBarellComponent::GetOwningPawner() const
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	if (IsValid(PawnOwner))
	{
		PawnOwner = Cast<APawn>(GetOwner()->GetOwner());
	}
	return PawnOwner;
}

AController* UWeaponBarellComponent::GetController() const
{
	APawn* PawnOwner = GetOwningPawner();
	return IsValid(PawnOwner) ? PawnOwner->GetController() : nullptr;
}

void UWeaponBarellComponent::ProcessProjectileHit(AGCProjectile* Projectile, const FHitResult& HitResult, const FVector& Direction)
{
	Projectile->SetProjectileActive(false);
	Projectile->SetActorLocation(ProjectilePoolLocation);
	Projectile->SetActorRotation(FRotator::ZeroRotator);
	Projectile->OnProjectileHit.RemoveAll(this);
	ProcessHit(HitResult, Direction);
}

void UWeaponBarellComponent::ProcessHit(const FHitResult& HitResult, const FVector& Direction)
{
	AActor* HitActor = HitResult.GetActor();
	if (IsValid(HitActor) && GetOwner()->HasAuthority())
	{
		float DamageOfDistance = DamageAmount;
		/*if (IsValid(FalloffDiagram))
		{
			float Distantion = (ShotEnd - ShotStart).Length();
			float DamageCoefficient = FalloffDiagram->GetFloatValue(Distantion);
			DamageOfDistance = DamageAmount * DamageCoefficient;
		}*/
		FPointDamageEvent DamageEvent;
		DamageEvent.HitInfo = HitResult;
		DamageEvent.ShotDirection = Direction;
		DamageEvent.DamageTypeClass = DamageTypeClass;
		HitActor->TakeDamage(DamageOfDistance, DamageEvent, GetController(), GetOwner());
	}

	UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DefaultDecalInfo.DecalMaterial, DefaultDecalInfo.DecalSize, HitResult.ImpactPoint, HitResult.ImpactNormal.ToOrientationRotator());
	if (IsValid(DecalComponent))
	{
		DecalComponent->SetFadeScreenSize(0.0001f);
		DecalComponent->SetFadeOut(DefaultDecalInfo.DecalLifeTime, DefaultDecalInfo.DecalFladeOutTime);
	}
}

void UWeaponBarellComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() < ROLE_Authority) 
	{
		return;
	}

	if (!IsValid(ProjectileClass))
	{
		return;
	}

	ProjectilePool.Reserve(ProjectilePoolSize);

	for (int32 i = 0; i < ProjectilePoolSize; i++)
	{
		AGCProjectile* Projectile = GetWorld()->SpawnActor<AGCProjectile>(ProjectileClass, ProjectilePoolLocation, FRotator::ZeroRotator);
		Projectile->SetOwner(GetOwningPawner());
		Projectile->SetProjectileActive(false);
		ProjectilePool.Add(Projectile);
	}
}

void UWeaponBarellComponent::LaunchProjectile(const FVector& LaunchStart, const FVector& LaunchDirection)
{
	AGCProjectile* Projectile = ProjectilePool[CurrentProjectileIndex];
	Projectile->SetActorLocation(LaunchStart);
	Projectile->SetActorRotation(LaunchDirection.ToOrientationRotator());
	Projectile->SetProjectileActive(true);
	Projectile->OnProjectileHit.AddDynamic(this, &UWeaponBarellComponent::ProcessProjectileHit);
	Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
	++CurrentProjectileIndex;
	if (CurrentProjectileIndex == ProjectilePool.Num())
	{
		CurrentProjectileIndex = 0;
	}
}

FVector UWeaponBarellComponent::GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const
{
	float SpreadSize = FMath::Tan(Angle);
	float RotationAngle = FMath::RandRange(0.0f, 2 * PI);

	float SpreadY = FMath::Cos(RotationAngle);
	float SpreadZ = FMath::Sin(RotationAngle);

	FVector Result = (ShotRotation.RotateVector(FVector::UpVector) * SpreadZ + ShotRotation.RotateVector(FVector::RightVector) * SpreadY) * SpreadSize;

	return Result;;
}

