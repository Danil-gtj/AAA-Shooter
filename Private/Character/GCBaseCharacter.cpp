#include "Character/GCBaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Character/LedgeDetectorComponent.h"
#include "Curves/CurveVector.h"
#include "Actors/Environment/Ladder.h"
#include "Character/CharacterComponents/ActorAttributeComponent.h"
#include "GameCode/GameCodeTypes.h"
#include "Engine/DamageEvents.h"
#include "Character/CharacterComponents/CharacterEquipmentComponent.h"
#include "Actors/Equipment/Weapons/MeleeWeaponItem.h"
#include "AIController.h"
#include "GameCode/Environment/PlatformTrigger.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Character/UI/Widgets/World/GCAtrributeProgressBar.h"
#include "Components/CharacterComponents/CharacterInventoryComponent.h"
#include "Actors/Actors/DroneCharacter/GCDroneCharacter.h"
#include "Actors/Actors/Controllers/DroneController.h"
#include "Character/GCPlayerController.h"
#include "GameFramework/Controller.h"
#include "Character/UI/Widgets/PlayerHUDWidget.h"
#include "Zipline.h"
#include "Kismet/GameplayStatics.h"
#include "GameCode/GameCodeGameModeBase.h"
#include "Actors/Equipment/EquipableItem.h"
#include "Character/UI/Widgets/Multiplayer/NicknameWidget.h"
#include "PlayerStates/BasePlayerState.h"
#include "Actors/GCGameInstance.h"

AGCBaseCharacter::AGCBaseCharacter(const FObjectInitializer& ObjectInitialaizer)
	: Super(ObjectInitialaizer.SetDefaultSubobjectClass<UGCBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	GCBaseCharacterMovementComponent = StaticCast<UGCBaseCharacterMovementComponent*>(GetCharacterMovement());

	LedgeDetector = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("LedgeDetector"));

	GetMesh()->CastShadow = true;
	GetMesh()->bCastDynamicShadow = true;

	CharacterAttributeComponent = CreateDefaultSubobject<UActorAttributeComponent>("Attribute component");
	CharacterEquipmentComponent = CreateDefaultSubobject<UCharacterEquipmentComponent>("CharacterEquipment component");
	CharacterInventoryComponent = CreateDefaultSubobject<UCharacterInventoryComponent>("CharacterInventory component");
	PlayerNickWidgetComp = CreateDefaultSubobject<UWidgetComponent>("PlayerNickNameTag");

	HealthBarProgressComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health Bar ProgressComponent"));
	HealthBarProgressComponent->SetupAttachment(GetCapsuleComponent());

	bReplicates = true;
}

void AGCBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController->IsA<AAIController>())
	{
		AAIController* AIController = Cast<AAIController>(NewController);
		if (IsValid(AIController))
		{
			FGenericTeamId TeamId((uint8)Team);
			AIController->SetGenericTeamId(TeamId);
		}
	}
	SetOwner(NewController);
	InitializeHealthProgress();
}

void AGCBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	CharacterAttributeComponent->OnDeathEvent.AddUObject(this, &AGCBaseCharacter::OnDeath);
	CharacterAttributeComponent->OutOfStaminaEvent.AddUObject(this, &AGCBaseCharacter::IsOutStamina);
	InitializeHealthProgress();
	GameMode = Cast<AGameCodeGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	SetUpPlayerNick();
}

void AGCBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGCBaseCharacter, bIsMantling);
}

void AGCBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TryChangeSprintState(DeltaTime);
	UpdateIKSettings(DeltaTime);
}

void AGCBaseCharacter::ChangeCrouchState()
{
	if (GCBaseCharacterMovementComponent->IsCrouching())
	{
		UnCrouch();
	}
	else if(!GCBaseCharacterMovementComponent->IsFalling())
	{
		Crouch();
	}
}

void AGCBaseCharacter::ChangeProneState()
{
	if (GetCharacterMovement()->IsCrouching() && !GCBaseCharacterMovementComponent->IsProning())
	{
		Prone();
	}
	else if (!GetCharacterMovement()->IsCrouching() && GCBaseCharacterMovementComponent->IsProning()) 
	{
		UnProne(false);
	}
}

void AGCBaseCharacter::OnStartProne(float HeightAdjust, float ScaledHalfHeightAdjust)
{
	RecalculateBaseEyeHeight();

	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + HeightAdjust + GCBaseCharacterMovementComponent->CrouchedHalfHeight;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + HeightAdjust + GCBaseCharacterMovementComponent->CrouchedHalfHeight;
	}
}

void AGCBaseCharacter::OnEndProne(float HeightAdjust, float ScaledHeightAdjust)
{
	RecalculateBaseEyeHeight();

	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	const float HeightDifference  = GCBaseCharacterMovementComponent->bIsFullHeight ? 0.0f : GCBaseCharacterMovementComponent->CrouchedHalfHeight - GCBaseCharacterMovementComponent->PronedHalfHeight;
	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + HeightDifference;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + HeightDifference;
	}
}

void AGCBaseCharacter::StartSprint()
{
	bIsSprintRequested = true;
}

void AGCBaseCharacter::StopSprint()
{
	bIsSprintRequested = false;
}

void AGCBaseCharacter::Mantle(bool bForce)
{
	if (!CanMantle() || bForce)
	{
		return;
	} 

	UnCrouch();

	FLedgeDescription LedgeDescription;

	if (LedgeDetector->DetectLedge(LedgeDescription))
	{
		bIsMantling = true;

		FMantlingMovementParameters MantlingParameters;
		MantlingParameters.InitialLocation = GetActorLocation();
		MantlingParameters.InitialRotation = GetActorRotation();
		MantlingParameters.TargetLocation = LedgeDescription.Location;
		MantlingParameters.TargetRotation = LedgeDescription.Rotation;
		MantlingParameters.Component = LedgeDescription.Component;

		float MantlingHeight = (MantlingParameters.TargetLocation - MantlingParameters.InitialLocation).Z;
		const FMantlingSettings& MantlingSettings = GetMantlingSettings(MantlingHeight);

		float MinRange;
		float MaxRange;

		MantlingSettings.MantlingCurve->GetTimeRange(MinRange, MaxRange);
		MantlingParameters.Duration = MaxRange - MinRange;
		MantlingParameters.MantlingCurve = MantlingSettings.MantlingCurve;

		FVector2D SourceRange(MantlingSettings.MinHeight, MantlingSettings.MaxHeight);
		FVector2D TargetRange(MantlingSettings.MinHeightStartTime, MantlingSettings.MaxHeightStratTime);
		MantlingParameters.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, MantlingHeight);

		MantlingParameters.InitialAnimationLocation = MantlingParameters.TargetLocation - MantlingSettings.AnimationCorrectionZ * FVector::UpVector + MantlingSettings.AnimationCorrectionXY * LedgeDescription.LedgeNoramal;

		if (IsLocallyControlled() || GetLocalRole() == ROLE_Authority) 
		{
			GCBaseCharacterMovementComponent->StartMantle(MantlingParameters);
		}

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MantlingSettings.MantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingParameters.StartTime);
		OnMantle(MantlingSettings, MantlingParameters.StartTime);
	}
}

void AGCBaseCharacter::OnRep_IsMantling(bool bWasMantling)
{
	if (GetLocalRole() == ROLE_SimulatedProxy && !bWasMantling && bIsMantling)
	{
		Mantle(true);
	}
}

void AGCBaseCharacter::ClimbLadderUp(float Value)
{
	if(GetBaseCharacterMovementComponent()->IsOnLadder() && !FMath::IsNearlyZero(Value))
	{
		FVector LadderUpVector = GetBaseCharacterMovementComponent()->GetCurrenLadder()->GetActorUpVector();
		AddMovementInput(LadderUpVector, Value);
	}
}

void AGCBaseCharacter::RegisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvalibleInterectiveActors.AddUnique(InteractiveActor);
}

void AGCBaseCharacter::UnregisterInteractiveActor(AInteractiveActor* InteractiveActor)
{
	AvalibleInterectiveActors.RemoveSingleSwap(InteractiveActor);
}

const ALadder* AGCBaseCharacter::GetAvailableLadder() const
{
	const ALadder* Result = nullptr;
	for (const AInteractiveActor* InteractiveActor : AvalibleInterectiveActors)
	{
		if (InteractiveActor->IsA<ALadder>())
		{
			Result = StaticCast<const ALadder*>(InteractiveActor);
			break;
		}
	}
	return Result;
}

const AZipline* AGCBaseCharacter::GetAvaliableZipline() const
{
	const AZipline* Result = nullptr;
	for (const AInteractiveActor* InteractiveActor : AvalibleInterectiveActors)
	{
		if (InteractiveActor->IsA<AZipline>())
		{
			Result = StaticCast<const AZipline*>(InteractiveActor);
			break;
		}
	}
	return Result;
}

void AGCBaseCharacter::InteractWithLadder()
{
	if (GetBaseCharacterMovementComponent()->IsOnLadder()) 
	{
		GetBaseCharacterMovementComponent()->DetachFromladder(EDetachFromLadderMethod::JumpOff);
	}
	else 
	{
		const ALadder* AvailableLadder = GetAvailableLadder();
		if (IsValid(AvailableLadder))
		{
			if (AvailableLadder->GetIsOnTop()) 
			{
				PlayAnimMontage(AvailableLadder->GetAttachFromTopAnimMontage());
			}
			GetBaseCharacterMovementComponent()->AttachToLadder(AvailableLadder);
		}
	}
}

void AGCBaseCharacter::InteractWithZipline()
{
	if (GetBaseCharacterMovementComponent()->IsOnZipline())
	{
		GetBaseCharacterMovementComponent()->DetachFromZipline();
	}
	else
	{
		const AZipline* AvailableZipline = GetAvaliableZipline();
		if (IsValid(AvailableZipline))
		{
			GetBaseCharacterMovementComponent()->AttachToZipline(AvailableZipline);
		}
	}
}

bool AGCBaseCharacter::CanSprint()
{
	return !GCBaseCharacterMovementComponent->IsOutOfStamina() && !GCBaseCharacterMovementComponent->IsCrouching() && !GCBaseCharacterMovementComponent->IsSprinting() && !bIsAiming;
}

bool AGCBaseCharacter::CanMantle() const
{
	return !GetBaseCharacterMovementComponent()->IsOnLadder() && !GetBaseCharacterMovementComponent()->IsMantling();
}

void AGCBaseCharacter::Prone(bool bClientSimulation)
{
	if (GCBaseCharacterMovementComponent) 
	{
		if (CanProne())
		{
			GCBaseCharacterMovementComponent->bWantsToProne = true;
		}
	}
}

void AGCBaseCharacter::UnProne(bool bIsFullHeight, bool bClientSimulation)
{
	if (GCBaseCharacterMovementComponent) 
	{
		GCBaseCharacterMovementComponent->bWantsToProne = false;
		GCBaseCharacterMovementComponent->bIsFullHeight = bIsFullHeight;
	}
}

bool AGCBaseCharacter::CanProne() const
{
	return !bIsProned && GCBaseCharacterMovementComponent && GetRootComponent() && !GetRootComponent()->IsSimulatingPhysics();
}

const UCharacterEquipmentComponent* AGCBaseCharacter::GetCharacterEquipmentComponent() const
{
	return CharacterEquipmentComponent;
}

UCharacterEquipmentComponent* AGCBaseCharacter::GetCharacterEquipmentComponent_Mutable() const
{
	return CharacterEquipmentComponent;
}

void AGCBaseCharacter::Falling()
{
	GetBaseCharacterMovementComponent()->bNotifyApex = true;
}

void AGCBaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (CurrentFallApex.Z > GetActorLocation().Z && CurrentFallApex.Z - GetActorLocation().Z > HardFallHeight)
	{
		PlayAnimMontage(HardLandAnimMontage);
		GetCharacterMovement()->DisableMovement(); 
		float Duration = PlayAnimMontage(HardLandAnimMontage);
		GetWorld()->GetTimerManager().SetTimer(HardLandTimer, this, &AGCBaseCharacter::EnableMovement, Duration, false);
	}

	float FallHeight = (CurrentFallApex - GetActorLocation()).Z * 0.01f;
	if (IsValid(FallDamageCurve)) 
	{
		float DamageAmount = FallDamageCurve->GetFloatValue(FallHeight);
		TakeDamage(DamageAmount, FDamageEvent(), GetController(), Hit.GetActor());
	}
}

void AGCBaseCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	CurrentFallApex = GetActorLocation();
}

void AGCBaseCharacter::OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimtaionStartTime)
{
}

void AGCBaseCharacter::OnDeath()
{
	GetCharacterMovement()->DisableMovement();
	float Duration = PlayAnimMontage(OnDeathAnimMontage);
	if (Duration == 0.0f)
	{
		EnableRagdoll();
	}
	bIsDead = true;
	GetWorld()->GetTimerManager().SetTimer(ToRespawnTimer, this, &AGCBaseCharacter::RespawnPlayer, TimeToRespawn, false);
}

void AGCBaseCharacter::StartFire()
{
	if(bIsDead)
	{
		return;
	}

	if (CharacterEquipmentComponent->IsSelectingWeapon())
	{
		return;
	}

	if (CharacterEquipmentComponent->IsEquipping())
	{
		return;
	}

	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRrangeWeapon();
	if (IsValid(CurrentRangeWeapon) && !GCBaseCharacterMovementComponent->IsFalling() && !GCBaseCharacterMovementComponent->IsSwimming() && !GCBaseCharacterMovementComponent->IsMantling() && !GCBaseCharacterMovementComponent->IsOnLadder())
	{
		CurrentRangeWeapon->StartFire();
	}
}

void AGCBaseCharacter::StopFire()
{
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRrangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopFire();
	}
}

void AGCBaseCharacter::StartAiming()
{
	ARangeWeaponItem* CurrenRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRrangeWeapon();
	if (!IsValid(CurrenRangeWeapon)) 
	{
		return;
	}
	bIsAiming = true;	
	CurrentAimingMovementSpeed = CurrenRangeWeapon->GetAimMovementMaxSpeed();
	CurrenRangeWeapon->StartAim();
	OnStartAiming();

	OnSprintEnd();
	GCBaseCharacterMovementComponent->StopSprint();
}

void AGCBaseCharacter::StopAiming()
{
	if (!bIsAiming) 
	{
		return;
	}

	ARangeWeaponItem* CurrenRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRrangeWeapon();
	if(IsValid(CurrenRangeWeapon))
	{
		CurrenRangeWeapon->StopAim();
	}

	bIsAiming = false;
	CurrentAimingMovementSpeed = 0.0f;
	CurrenRangeWeapon->StopAim();
	OnStopAiming();
}

FRotator AGCBaseCharacter::GetAimOffset()
{
	FVector AimDirectionWorld = GetBaseAimRotation().Vector();
	FVector AimDirectionLocal = GetTransform().InverseTransformVectorNoScale(AimDirectionWorld);
	FRotator Result = AimDirectionLocal.ToOrientationRotator();
	
	return Result;
}

void AGCBaseCharacter::OnStartAiming_Implementation()
{
	OnStartAimingInternal();
}

void AGCBaseCharacter::OnStopAiming_Implementation()
{
	OnStopAimingInternal();
}

bool AGCBaseCharacter::IsAiming() const
{
	return bIsAiming;
}

float AGCBaseCharacter::GetAimingMovementSpeed() const
{
	return CurrentAimingMovementSpeed;
}

void AGCBaseCharacter::Reload() const
{
	if (IsValid(CharacterEquipmentComponent->GetCurrentRrangeWeapon()))
	{
		CharacterEquipmentComponent->ReloadCurrenWeapon();
	}
}

void AGCBaseCharacter::NextItem()
{
	CharacterEquipmentComponent->EquipNextItem();
}

void AGCBaseCharacter::PreviousItem()
{
	CharacterEquipmentComponent->EquipPreviousItem();

}

void AGCBaseCharacter::EquipPrimaryItem()
{
	CharacterEquipmentComponent->EquipItemInSlot(EEquipmentSlots::PrimaryItemSlot);
}

void AGCBaseCharacter::PrimaryMeleeAttack()
{
	AMeleeWeaponItem* CurrentMeleeWeapon = CharacterEquipmentComponent->GetCurrentMeleeWeaponItem();
	if (IsValid(CurrentMeleeWeapon)) 
	{
		CurrentMeleeWeapon->StartAttack(EMeleeAttackTypes::PrimaryAttack);
	}
}

void AGCBaseCharacter::SecondaryMeleeAttack()
{
	AMeleeWeaponItem* CurrentMeleeWeapon = CharacterEquipmentComponent->GetCurrentMeleeWeaponItem();
	if (IsValid(CurrentMeleeWeapon))
	{
		CurrentMeleeWeapon->StartAttack(EMeleeAttackTypes::SecondaryAttack);
	}
}

UActorAttributeComponent* AGCBaseCharacter::GetChracterAttributesComponent() const
{
	return CharacterAttributeComponent;
}

FGenericTeamId AGCBaseCharacter::GetGenericTeamId() const
{
	return FGenericTeamId((uint8)Team);
}

void AGCBaseCharacter::InitializeHealthProgress()
{
	UGCAtrributeProgressBar* Widget = Cast<UGCAtrributeProgressBar>(HealthBarProgressComponent->GetUserWidgetObject());
	if (!IsValid(Widget)) 
	{
		HealthBarProgressComponent->SetVisibility(false);
		return;
	}

	if (IsPlayerControlled() && IsLocallyControlled())
	{
		HealthBarProgressComponent->SetVisibility(false);
	}

}

void AGCBaseCharacter::Die()
{
	OnDeath();
}

void AGCBaseCharacter::RespawnPlayer()
{	
	/*AGCPlayerController* PC = Cast<AGCPlayerController>(GetController());
	PC->SpawnPlayer("TeamA");*/
	DestroyPlayer();
}

void AGCBaseCharacter::DestroyPlayer()
{
	Destroy();
	TArray<AActor*> Childs;
	GetAttachedActors(Childs);
	for (AActor* Attached : Childs)
	{
		Attached->Destroy();
	}
}

void AGCBaseCharacter::LaunchDrone()
{
	if (MaxDroneAmount != 0)
	{
		if (IsValid(DroneCharacterClass))
		{
			if (HasAuthority())
			{
				SpawnDrone();
			}

			if (GetLocalRole() == ROLE_AutonomousProxy)
			{
				Server_SpawnDrone();
			}

			DronesArray.Add(Drone);
			MaxDroneAmount--;
			AGCPlayerController* CurrentController = Cast<AGCPlayerController>(GetController());
			if (IsValid(CurrentController))
			{
				SetPlayerHUDVisibility(false);
				Drone->SetPlayerOwner(this);
				CurrentController->UnPossess();
				CurrentController->Possess(Drone);
				Drone->bIsConnected = true;
			}
		}
	}
}

void AGCBaseCharacter::SpawnDrone()
{
	FVector SpawnLoc = GetRootComponent()->GetForwardVector() * 50.0f + GetRootComponent()->GetComponentLocation();	
	Drone = GetWorld()->SpawnActor<AGCDroneCharacter>(DroneCharacterClass, FTransform(SpawnLoc));	
}

void AGCBaseCharacter::Server_SpawnDrone_Implementation()
{
	SpawnDrone();
}

void AGCBaseCharacter::ConnectToDrone()
{
	if (DronesArray.Num() != 0)
	{
		AGCPlayerController* CurrentController = Cast<AGCPlayerController>(GetController());
		for (int i = 0; i < DronesArray.Num(); i++)
		{
			SetPlayerHUDVisibility(false);
			Drone = DronesArray[i];
			CurrentController->UnPossess();
			CurrentController->Possess(Drone);
			Drone->SetPlayerOwner(this);
			Drone->bIsConnected = true;
			Drone->SetDroneHUDVisibility(true);
		}
	}
}

TArray<AGCDroneCharacter*> AGCBaseCharacter::GetDroneArray()
{
	return DronesArray;
}

void AGCBaseCharacter::SetPlayerHUDVisibility(bool bVisible)
{
	AGCPlayerController* CurrentController = Cast<AGCPlayerController>(GetController());
	if (bVisible && IsValid(CurrentController->GetPlayerHUDWidget()))
	{
		CurrentController->GetPlayerHUDWidget()->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CurrentController->GetPlayerHUDWidget()->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AGCBaseCharacter::EnableMovement()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void AGCBaseCharacter::Destroyed()
{
	Super::Destroyed();
	RespawnPlayer();
}

void AGCBaseCharacter::Multicast_PlayAnimMontage_Implementation(UAnimMontage* AnimMontage)
{
	PlayAnimMontage(AnimMontage);
}

void AGCBaseCharacter::Server_PlayAnimMontage_Implementation(UAnimMontage* AnimMontage)
{
	Multicast_PlayAnimMontage(AnimMontage);
}

void AGCBaseCharacter::IsOutStamina(bool bIsOutOfStamina)
{
	if (bIsOutOfStamina)
	{
		GCBaseCharacterMovementComponent->SetIsOutOfStamina(true);
		GCBaseCharacterMovementComponent->SetJumpAllowed(false);
	}
	else
	{
		GCBaseCharacterMovementComponent->SetIsOutOfStamina(false);
		GCBaseCharacterMovementComponent->SetJumpAllowed(true);
	}
}

void AGCBaseCharacter::OnStartAimingInternal()
{
	if (OnAimingStateChanged.IsBound())
	{
		OnAimingStateChanged.Broadcast(true);
	}
}

void AGCBaseCharacter::OnStopAimingInternal()
{
	if (OnAimingStateChanged.IsBound())
	{
		OnAimingStateChanged.Broadcast(false);
	}
}

void AGCBaseCharacter::SetUpPlayerNick()
{
	if (IsLocallyControlled())
	{
		UGCGameInstance* GS = Cast<UGCGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		ServerSetPlayerNickname(GS->PlayerNickname);
	}
}

void AGCBaseCharacter::ServerSetPlayerNickname_Implementation(const FText& Nick)
{
	CustomPlayerNick = Nick;
}
 
void AGCBaseCharacter::OnRep_CustomPlayerNick()
{
	if (IsValid(PlayerNickWidgetComp))
	{
		UNicknameWidget* NickWidget = Cast<UNicknameWidget>(PlayerNickWidgetComp);
		NickWidget->SetPlayerName(CustomPlayerNick);
		PlayerNickWidgetComp->SetHiddenInGame(false);
	}
	else
	{
		OnRep_CustomPlayerNick();
	}
}

float AGCBaseCharacter::GetIKOffsetForASocket(const FName SocketName)
{
	float Result = 0.0f;

	float CapsuleHalf = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	FVector SocketLocation = GetMesh()->GetSocketLocation(SocketName);
	FVector TraceStart(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z);
	FVector TraceEnd = TraceStart - (CapsuleHalf + IKTraceDistance) * FVector::UpVector;

	FHitResult HitResult;
	const ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), TraceStart, TraceEnd, TraceType, true, TArray<AActor*>(), EDrawDebugTrace::None, HitResult, true)) 
	{
		Result = TraceStart.Z - CapsuleHalf - HitResult.Location.Z;
	}

	return Result;
}

float AGCBaseCharacter::CalculateIKPelvisOffset()
{
	return IKRightFootOffset > IKLeftFootOffset ? -IKRightFootOffset : -IKLeftFootOffset;
}

bool AGCBaseCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation() && !GetBaseCharacterMovementComponent()->IsMantling();
}

void AGCBaseCharacter::TryChangeSprintState(float DeltaTime)
{
	if(bIsSprintRequested && CanSprint())
	{
			OnSprintStart();
			GCBaseCharacterMovementComponent->StartSprint();
	}

	if (!bIsSprintRequested && GCBaseCharacterMovementComponent->IsSprinting() && !CanSprint())
	{
			OnSprintEnd();
			GCBaseCharacterMovementComponent->StopSprint();
	}
}

void AGCBaseCharacter::UpdateIKSettings(float DeltaTime)
{
	IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, GetIKOffsetForASocket(RightFootSocketName), DeltaTime, IKInterpSpeed);
	IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, GetIKOffsetForASocket(LeftFootSocketName), DeltaTime, IKInterpSpeed);
	IKPelvisBoneOffset = FMath::FInterpTo(IKPelvisBoneOffset, CalculateIKPelvisOffset(), DeltaTime, IKInterpSpeed);
}

const FMantlingSettings& AGCBaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
	return LedgeHeight > LowMantleMaxHeight ? HeightMantleSettings : LowMantleSettings;
}

void AGCBaseCharacter::EnableRagdoll()
{
	GetMesh()->SetCollisionProfileName(CollisionProfileRagdoll);
	GetMesh()->SetSimulatePhysics(true);
}