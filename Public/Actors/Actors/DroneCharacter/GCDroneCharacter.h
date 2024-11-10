// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameCode/GameCodeTypes.h"
#include "GenericTeamAgentInterface.h"
#include "GCDroneCharacter.generated.h"

class AGCBaseCharacter;
class USpringArmComponent;
UCLASS()
class GAMECODE_API AGCDroneCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* InputeComponent) override;

	AGCDroneCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void MoveForward(float Value);
	virtual void MoveRight(float Value);
	virtual void Turn(float Value);
	virtual void LookUp(float Value);
	virtual void TurnAtRate(float Value);
	virtual void LookUpAtRate(float Value);
	virtual void MoveUp(float Value);

	void NextDrone();

	void PreviousDrone();

	void ReturnToPlayerControll();

	void SelfDestruction();

	void Dead();

	void Exploring();

	UFUNCTION()
	void OnTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void SetPlayerOwner(AGCBaseCharacter* CharacterOwner_In);

	UFUNCTION(Server, Reliable)
	void Server_Dead();

	UPROPERTY(ReplicatedUsing=OnRep_IsDead)
	bool bIsDead = false;

	UFUNCTION()
	void OnRep_IsDead(bool bWasDead);

	UFUNCTION(Server, Reliable)
	void Server_SetRotationRight(float Value);

	UFUNCTION(Server, Reliable)
	void Server_SetRotationForward(float Value);

	UPROPERTY(Replicated)
	FQuat TiltRotation;

	bool bIsConnected;

	void CreateDroneWidget();

	void SetDroneHUDVisibility(bool bVisible);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drone | Health")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drone | Controll Settings")
	float TiltAngle = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drone | Controll Settings")
	float TiltSpeedLerp = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drone | Explosion Settings")
	float TimeToExplosion = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drone | Team")
	ETeams Team = ETeams::Enemy;

	// Team
	virtual FGenericTeamId GetGenericTeamId() const override;

	//~Team

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<class UDroneStatsWidget> DroneHUDWidgetClass;

private:
	AGCBaseCharacter* CharacterOwner;

	float CurrentHealth;

	TWeakObjectPtr<class ADroneController> DroneController;

	FTimerHandle ExplosionTimerHandle;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
	class UExplosionComponent* ExplosionComponent;

	UDroneStatsWidget* DroneHUDWidget = nullptr;

};
