#include "Character/CharacterComponents/ActorAttributeComponent.h"
#include "Character/GCBaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameCode/GameCodeTypes.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Character/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

UActorAttributeComponent::UActorAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UActorAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UActorAttributeComponent, Health);
}


void UActorAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	
	checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT("UActorAttributeComponent::BeginPlay can be used only with AGCBaseCharacter"));
	
	CachedBaseCharacterOwner = StaticCast<AGCBaseCharacter*>(GetOwner());

	GCBaseCharacterMovementComponent = Cast<UGCBaseCharacterMovementComponent>(CachedBaseCharacterOwner->GetCharacterMovement());

	Health = MaxHealth;
	CurrentStamina = MaxStamina;
	Oxygen = MaxOxygen;


	if (GetOwner()->HasAuthority()) 
	{
		CachedBaseCharacterOwner->OnTakeAnyDamage.AddDynamic(this, &UActorAttributeComponent::OnTakeAnyDamage);
	}
}

void UActorAttributeComponent::UpdateStaminaValue(float Delta)
{
	if (GCBaseCharacterMovementComponent->IsOutOfStamina() || !GCBaseCharacterMovementComponent->IsSprinting())
	{
		if (CurrentStamina >= MaxStamina)
		{
			if (OutOfStaminaEvent.IsBound())
			{
				OutOfStaminaEvent.Broadcast(false);
			}
		}
		else
		{
			CurrentStamina += StaminaRestoreVelocity * Delta;
			CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);
			return;
		}
	}

	if (GCBaseCharacterMovementComponent->IsSprinting())
	{
		if (CurrentStamina <= 0.0f)
		{
			if (OutOfStaminaEvent.IsBound())
			{
				OutOfStaminaEvent.Broadcast(true);
			}
		}
		else if (!GCBaseCharacterMovementComponent->IsOutOfStamina())
		{
			CurrentStamina -= SprintStaminaConsumpVelocity * Delta;
			CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);
		}
	}
}

void UActorAttributeComponent::OnRep_Health()
{
	OnHealthChanged();
}

void UActorAttributeComponent::OnHealthChanged()
{
	if (OnHealthChangedEvent.IsBound())
	{
		OnHealthChangedEvent.Broadcast(GetHealthPercent());
	}

	if (Health <= 0.0f)
	{
		if (OnDeathEvent.IsBound())
		{
			OnDeathEvent.Broadcast();
		}
	}
}

float UActorAttributeComponent::GetHealthPercent() const
{
	return Health / MaxHealth;
}

float UActorAttributeComponent::GetStaminaPercent() const
{
	return CurrentStamina / MaxStamina;
}

float UActorAttributeComponent::GetOxygenPercent() const
{
	return Oxygen / MaxOxygen;
}

void UActorAttributeComponent::AddHealth(float HealthToAdd)
{
	Health = FMath::Clamp(Health + HealthToAdd, 0.0f, MaxHealth);
	OnHealthChanged();
}

void UActorAttributeComponent::RestoreFullStamina()
{
	CurrentStamina = MaxStamina;
}

void UActorAttributeComponent::OnLevelDeserialized_Implementation()
{
	OnHealthChanged();
}

void UActorAttributeComponent::UpdateOxygenValue(float Delta)
{
	FVector HeadLocation = CachedBaseCharacterOwner->GetMesh()->GetSocketLocation(FName("head"));

	if (CachedBaseCharacterOwner->GetCharacterMovement()->IsSwimming())
	{
		APhysicsVolume* Volume = CachedBaseCharacterOwner->GetCharacterMovement()->GetPhysicsVolume();
		float VolumeTopPlane = Volume->GetActorLocation().Z + Volume->GetBounds().BoxExtent.Z * Volume->GetActorScale3D().Z;

		if (VolumeTopPlane > HeadLocation.Z)
		{
			if (Oxygen <= 0.0f && bCanDamage)
			{
				GetWorld()->GetTimerManager().SetTimer(OxygenDamageTimer, this, &UActorAttributeComponent::UpdateOxygenDamage, Interval, false);
				bCanDamage = false;
			}
			else
			{
				Oxygen -= SwimOxygenConsumptionVelocity * Delta;
				Oxygen = FMath::Clamp(Oxygen, 0.0f, MaxOxygen);
			}
		}
	}
	else 
	{
		if (Oxygen == MaxOxygen)
		{
			return;
		}
		Oxygen += OxygenRestoreVelocity * Delta;
		Oxygen = FMath::Clamp(Oxygen, 0.0f, MaxOxygen);
	}
}

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
void UActorAttributeComponent::DebugDrawAttributes()
	{
		/*FVector TextLocationHealth = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 10.0f) * FVector::UpVector;
		DrawDebugString(GetWorld(), TextLocationHealth, FString::Printf(TEXT("Health: %.2f"), Health), nullptr, FColor::Green, 0.0f, true);

		FVector TextLocationStamina = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) * FVector::UpVector;
		DrawDebugString(GetWorld(), TextLocationStamina, FString::Printf(TEXT("Stamina: %.2f"), CurrentStamina), nullptr, FColor::Blue, 0.0f, true);

		FVector TextLocationOxygen = CachedBaseCharacterOwner->GetActorLocation() + (CachedBaseCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - 10.0f) * FVector::UpVector;
		DrawDebugString(GetWorld(), TextLocationOxygen, FString::Printf(TEXT("Oxygen: %.2f"), Oxygen), nullptr, FColor::Cyan, 0.0f, true);*/
	}
#endif 

void UActorAttributeComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!IsAlive())
	{
		return;
	}

	UE_LOG(LogDamage, Warning, TEXT("AGCBaseCharacter::TakeDamage %s recevied %.2f amount of damage from %s"), *CachedBaseCharacterOwner->GetName(), Damage, *DamageCauser->GetName());
	Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);

	OnHealthChanged();
	
}

void UActorAttributeComponent::UpdateOxygenDamage()
{
	if (CachedBaseCharacterOwner->GetCharacterMovement()->IsSwimming())
	{
		if (Health <= 0.0f)
		{
			CachedBaseCharacterOwner->OnDeath();
		}
		else
		{
			Health -= OxygenDamagePerSecond;
		}
	}
	bCanDamage = true;
}

void UActorAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateStaminaValue(DeltaTime);
	UpdateOxygenValue(DeltaTime);
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	DebugDrawAttributes();
#endif
}