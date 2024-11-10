#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterComponents/CharacterEquipmentComponent.h"
#include "GenericTeamAgentInterface.h"
#include "UObject/ScriptInterface.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GCBaseCharacter.generated.h"

DECLARE_DELEGATE_OneParam(FOnInteractableObjectFound, FName);

class AGameCodeGameModeBase;
class AGCDroneCharacter;
USTRUCT(BlueprintType)
struct FMantlingSettings
{
	GENERATED_BODY();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* MantlingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* FPSMantlingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UCurveVector* MantlingCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimationCorrectionXY = 65.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimationCorrectionZ = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxHeight = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MinHeight = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxHeightStratTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MinHeightStartTime = 0.5f;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAimingStateChanged, bool)

class UGCCharacterAttributeSet;
class UInventoryItem;
class UCharacterInventoryComponent;
class UWidgetComponent;
class AInteractiveActor;
class UGCBaseCharacterMovementComponent;
class UCharacterEquipmentComponent;
class UActorAttributeComponent;
UCLASS(Abstract, NotBlueprintable)
class GAMECODE_API AGCBaseCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AGCBaseCharacter(const FObjectInitializer& ObjectInitialaizer);

	virtual void PossessedBy(AController* NewController) override;

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;

	float GetIKRightFootOffset() const { return IKRightFootOffset; }

	float GetIKLeftFootOffset() const { return IKLeftFootOffset; }

	float GetIKPelvisBoneOffset() const { return IKPelvisBoneOffset; }

	virtual void MoveForward(float Value) {};
	virtual void MoveRight(float Value) {};
	virtual	void Turn(float Value) {};
	virtual void LookUp(float Value) {};
	virtual	void TurnAtRate(float Value) {};
	virtual	void LookUpAtRate(float Value) {};
	virtual void SwimForward(float Value) {};
	virtual void SwimRight(float Value) {};
	virtual void SwimUp(float Value) {};

	virtual void ChangeCrouchState();
	virtual void ChangeProneState();

	virtual void OnStartProne(float HeightAdjust, float ScaledHalfHeightAdjust);
	virtual void OnEndProne(float HeightAdjust, float ScaledHeightAdjust);

	virtual void StartSprint();
	virtual void StopSprint();

	UFUNCTION(BlueprintCallable)
	virtual void Mantle(bool bForce = false);

	UPROPERTY(ReplicatedUsing=OnRep_IsMantling)
	bool bIsMantling;

	UFUNCTION()
	void OnRep_IsMantling(bool bWasMantling);

	virtual void ClimbLadderUp(float Value);
	virtual void InteractWithLadder();
	virtual void InteractWithZipline();
	const class ALadder* GetAvailableLadder() const;
	const class AZipline* GetAvaliableZipline() const;

	float CalculateIKPelvisOffset();

	virtual bool CanJumpInternal_Implementation() const override;

	FORCEINLINE UGCBaseCharacterMovementComponent* GetBaseCharacterMovementComponent() const { return GCBaseCharacterMovementComponent; }

	void RegisterInteractiveActor(AInteractiveActor* InteractiveActor);
	void UnregisterInteractiveActor(AInteractiveActor* InteractiveActor);

	UFUNCTION(BlueprintCallable, Category = "Character | Prone")
	virtual void Prone(bool bClientSimulation = false);

	UFUNCTION(BlueprintCallable, Category = "Character | Prone")
	virtual void UnProne(bool bIsFullHeight, bool bClientSimulation = false);

	UFUNCTION(BlueprintCallable, Category = "Character | Prone")
	virtual bool CanProne() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Prone")
	bool bIsProned = false;

	const UCharacterEquipmentComponent* GetCharacterEquipmentComponent() const;
	
	UCharacterEquipmentComponent* GetCharacterEquipmentComponent_Mutable() const;

	virtual void Falling() override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void NotifyJumpApex() override;

	virtual void OnDeath();

	void StartFire();
	void StopFire();

	void StartAiming();
	void StopAiming();

	FRotator GetAimOffset();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStartAiming();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStopAiming();

	bool IsAiming() const;

	FOnAimingStateChanged OnAimingStateChanged;

	float GetAimingMovementSpeed() const;

	void Reload() const;

	void NextItem();
	void PreviousItem();

	void EquipPrimaryItem();

	void PrimaryMeleeAttack();
	void SecondaryMeleeAttack();

	UActorAttributeComponent* GetChracterAttributesComponent() const;
	
	/**IGenericTeamAgentInterface **/

	virtual FGenericTeamId GetGenericTeamId() const override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Character | Components")
	UWidgetComponent* HealthBarProgressComponent;

	void InitializeHealthProgress();

	const UGCCharacterAttributeSet* GetGCCharacterAttributeSet() const;

	void Die();

	void RespawnPlayer();

	void DestroyPlayer();

	void LaunchDrone();

	void SpawnDrone();

	UFUNCTION(Server, Reliable)
	void Server_SpawnDrone();

	void ConnectToDrone();

	TArray<AGCDroneCharacter*> GetDroneArray();
	
	void SetPlayerHUDVisibility(bool bVisible);

	void EnableMovement();

	virtual void Destroyed() override;

	UFUNCTION(Server, Reliable)
	void Server_PlayAnimMontage(UAnimMontage* AnimMontage);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayAnimMontage(UAnimMontage* AnimMontage);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Team")
	ETeams Team = ETeams::TeamA;

	void SetUpPlayerNick();

	UFUNCTION(Server, Reliable)
	void ServerSetPlayerNickname(const FText& Nick);

	UPROPERTY(ReplicatedUsing = OnRep_CustomPlayerNick)
	FText CustomPlayerNick;

	UFUNCTION()
	virtual void OnRep_CustomPlayerNick();
protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Character | Movement")
	void OnSprintStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Character | Movement")
	void OnSprintEnd();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Controll")
	float BaseTurnRate = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Controll")
	float BaseLookUpRate = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character | Movement")
	float SprintSpeed = 800.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
	class UActorAttributeComponent* CharacterAttributeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | IK")
	FName RightFootSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | IK")
	FName LeftFootSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | IK")
	float IKInterpSpeed = 20.0f;

	virtual bool CanSprint();

	bool CanMantle() const;

	virtual void OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimtaionStartTime);

	void TryChangeSprintState(float DeltaTime);

	class UGCBaseCharacterMovementComponent* GCBaseCharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Movement")
	class ULedgeDetectorComponent* LedgeDetector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling")
	FMantlingSettings HeightMantleSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling")
	FMantlingSettings LowMantleSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Movement | Mantling", meta =(Clamp = 0.0f, UIMin = 0.0f))
	float LowMantleMaxHeight = 125.0f;

	void IsOutStamina(bool bIsOutOfStamina);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Animations")
	class UAnimMontage* OnDeathAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Attributes")
	class UCurveFloat* FallDamageCurve;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
	class UCharacterEquipmentComponent* CharacterEquipmentComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character | Components")
	class UCharacterInventoryComponent* CharacterInventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character | Components")
	class UWidgetComponent* PlayerNickWidgetComp;

	virtual void OnStartAimingInternal();
	virtual void OnStopAimingInternal();

	// ~Drone Settings

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AGCDroneCharacter> DroneCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class ADroneController> DroneControllerClass;

	ADroneController* DroneController;

	AGCDroneCharacter* Drone;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	int MaxDroneAmount = 3;

	TArray<AGCDroneCharacter*> DronesArray;

	// ~DroneSettings

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Falling")
	float HardFallHeight = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Falling")
	class UAnimMontage* HardLandAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Death")
	float TimeToRespawn;

private:
	float GetIKOffsetForASocket(const FName SocketName);

	float IKRightFootOffset = 0.0f;
	float IKLeftFootOffset = 0.0f;
	float IKPelvisBoneOffset = 0.0f;

	float IKTraceDistance = 45.0f;
	float IKScale;

	bool bIsSprintRequested = false;

	float CurrentStamina;

	void UpdateIKSettings(float DeltaTime);

	const FMantlingSettings& GetMantlingSettings(float LedgeHeight) const;

	TArray<AInteractiveActor*> AvalibleInterectiveActors;

	void EnableRagdoll();

	FVector CurrentFallApex;

	bool bIsAiming;

	float CurrentAimingMovementSpeed;

	bool bIsDead = false;

	FTimerHandle HardLandTimer;

	FTimerHandle ToRespawnTimer;

	AGameCodeGameModeBase* GameMode;
};
