#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActorAttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeathEventSignature);
DECLARE_MULTICAST_DELEGATE_OneParam(FOutOfStaminaEventSignature, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMECODE_API UActorAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UActorAttributeComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FOnDeathEventSignature OnDeathEvent;
	
	FOutOfStaminaEventSignature OutOfStaminaEvent;

	FOnHealthChanged OnHealthChangedEvent;

	bool IsAlive() { return Health > 0.0f; }

	void UpdateOxygenValue(float Delta);

	float GetHealthPercent() const;

	float GetStaminaPercent() const;

	float GetOxygenPercent() const;

	void AddHealth(float HealthToAdd);

	void RestoreFullStamina();

	virtual void OnLevelDeserialized_Implementation();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health", meta = (UMin = 0.0f))
	float MaxHealth = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (UMin = 0.0f))
	float MaxStamina;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (UMin = 0.0f))
	float StaminaRestoreVelocity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stamina", meta = (UMin = 0.0f))
	float SprintStaminaConsumpVelocity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Oxygen", meta = (UMin = 0.0f))
	float MaxOxygen = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Oxygen", meta = (UMin = 0.0f))
	float OxygenRestoreVelocity = 15.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Oxygen", meta = (UMin = 0.0f))
	float SwimOxygenConsumptionVelocity = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Oxygen", meta = (UMin = 0.0f))
	float OxygenDamagePerSecond = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Oxygen", meta = (UMin = 0.0f))
	float Interval = 2.0f;

	UPROPERTY()
	class UGCBaseCharacterMovementComponent* GCBaseCharacterMovementComponent;

	void UpdateStaminaValue(float Delta);

private:
	UPROPERTY(ReplicatedUsing=OnRep_Health)
	float Health = 0.0f;

	UFUNCTION()
	void OnRep_Health();

	void OnHealthChanged();

	float CurrentStamina;

	float Oxygen;

	bool bIsOutOfStamina;

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	void DebugDrawAttributes();
#endif

	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	TWeakObjectPtr<class AGCBaseCharacter> CachedBaseCharacterOwner;

	FTimerHandle OxygenDamageTimer;

	void UpdateOxygenDamage();

	bool bCanDamage = true;
};
