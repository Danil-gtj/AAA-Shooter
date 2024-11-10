#include "Blueprint/UserWidget.h"
#include "C:/Program Files/Epic Games/UE_5.2/Engine/Plugins/Runtime/SignificanceManager/Source/SignificanceManager/Public/SignificanceManager.h"
#include "Character/GCBaseCharacter.h"
#include "Character/GCPlayerController.h"
#include "Character/UI/Widgets/AmmoWidget.h"
#include "Character/UI/Widgets/PlayerHUDWidget.h"
#include "Character/UI/Widgets/ReticleWidget.h"
#include "GameCode/GameCodeTypes.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/GameplayStatics.h"
#include "Character/UI/Widgets/OptionsWidgets/OptionsMenuWidget.h"
#include "Character/UI/Widgets/TeamSelect/TeamSelectWidget.h"
#include "GameCode/GameCodeGameModeBase.h"
#include "GameFramework/Actor.h"
#include "Multiplayer/Spawns/MultiplayerPlayerStart.h"

void AGCPlayerController::BeginPlay()
{
	Super::BeginPlay();
	/*if (IsLocalPlayerController())
	{
		CreateChangeTeamWidget();
	}*/
	if (IsLocalPlayerController())
	{
		CreateAndInitializeWidtget();
	}
}

void AGCPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	CachedBaseCharacter = Cast<AGCBaseCharacter>(InPawn);
}

bool AGCPlayerController::GetIgnoreCameraPitch() const
{
	return bIgnoreCameraPitch;
}

void AGCPlayerController::SetIgnoreCameraPitch(bool bIgnoreCameraPitch_In)
{
	bIgnoreCameraPitch = bIgnoreCameraPitch_In;
}

void AGCPlayerController::Tick(float DeltaSenonds)
{
	Super::Tick(DeltaSenonds);
	USignificanceManager* SignificanceManager = USignificanceManager::Get(GetWorld());
	if (IsValid(SignificanceManager))
	{
		FVector ViewLocation;
		FRotator ViewRotation;
		GetPlayerViewPoint(ViewLocation, ViewRotation);
		FTransform ViewTransform(ViewRotation, ViewLocation);
		TArray<FTransform> Viewpoints = { ViewTransform };
		SignificanceManager->Update(Viewpoints);
	}
}

void AGCPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &AGCPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AGCPlayerController::MoveRight);
	InputComponent->BindAxis("Turn", this, &AGCPlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &AGCPlayerController::LookUp);
	InputComponent->BindAxis("TurnAtRate", this, &AGCPlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUpAtRate", this, &AGCPlayerController::LookUpAtRate);
	InputComponent->BindAxis("SwimForward", this, &AGCPlayerController::SwimForward);
	InputComponent->BindAxis("SwimRight", this, &AGCPlayerController::SwimRight);
	InputComponent->BindAxis("SwimUp", this, &AGCPlayerController::SwimUp);
	InputComponent->BindAxis("ClimbLadderUp", this, &AGCPlayerController::ClimbLadderUp);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &AGCPlayerController::ChangeCrouchState);
	InputComponent->BindAction("Mantle", EInputEvent::IE_Pressed, this, &AGCPlayerController::Mantle);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AGCPlayerController::Jump);
	InputComponent->BindAction("Prone", EInputEvent::IE_Pressed, this, &AGCPlayerController::ChangeProneState);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AGCPlayerController::StartSprint);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &AGCPlayerController::StopSprint);
	InputComponent->BindAction("InteractWithLadder", EInputEvent::IE_Pressed, this, &AGCPlayerController::InteractWithLadder);
	InputComponent->BindAction("InteractWithZipline", EInputEvent::IE_Pressed, this, &AGCPlayerController::InteractWirhZipline);
	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &AGCPlayerController::BeginFire);
	InputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &AGCPlayerController::FinishFire);
	InputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &AGCPlayerController::StartAiming);
	InputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &AGCPlayerController::StopAiming);
	InputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &AGCPlayerController::Reload);
	InputComponent->BindAction("NextItem", EInputEvent::IE_Pressed, this, &AGCPlayerController::NextItem);
	InputComponent->BindAction("PreviousItem", EInputEvent::IE_Pressed, this, &AGCPlayerController::PreviousItem);
	InputComponent->BindAction("EquipPrimaryItem", EInputEvent::IE_Pressed, this, &AGCPlayerController::EquipPrimaryItem);
	InputComponent->BindAction("PrimaryMeleeAttack", EInputEvent::IE_Pressed, this, &AGCPlayerController::PrimaryMeleeAttack);
	InputComponent->BindAction("SecondaryMeleeAttack", EInputEvent::IE_Pressed, this, &AGCPlayerController::SecondaryMeleeAttack);
	FInputActionBinding& ToggleMenuBinding = InputComponent->BindAction("ToggleMainMenu", EInputEvent::IE_Pressed, this, &AGCPlayerController::ToggleMainMenu);
	ToggleMenuBinding.bExecuteWhenPaused = true;
	InputComponent->BindAction("LaunchDrone", EInputEvent::IE_Pressed, this, &AGCPlayerController::LaunchDrone);
	InputComponent->BindAction("ConnectToDrone", EInputEvent::IE_Pressed, this, &AGCPlayerController::ConnectToDrone);
}

void AGCPlayerController::MoveForward(float Value)
{
	if (CachedBaseCharacter.IsValid()) 
	{
		CachedBaseCharacter->MoveForward(Value);
	}
}

void AGCPlayerController::MoveRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveRight(Value);
	}
}

void AGCPlayerController::Turn(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Turn(Value);
	}
}

void AGCPlayerController::LookUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUp(Value);
	}
}

void AGCPlayerController::TurnAtRate(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->TurnAtRate(Value);
	}
}

void AGCPlayerController::LookUpAtRate(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUpAtRate(Value);
	}
}

void AGCPlayerController::ChangeCrouchState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeCrouchState();
	}
}

void AGCPlayerController::Mantle()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Mantle();
	}
}

void AGCPlayerController::ClimbLadderUp(float Value)
{
	if (CachedBaseCharacter.IsValid()) 
	{
		CachedBaseCharacter->ClimbLadderUp(Value);
	}
}

void AGCPlayerController::InteractWithLadder()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithLadder();
	}
}

void AGCPlayerController::InteractWirhZipline()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithZipline();
	}
}

void AGCPlayerController::Jump()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Jump();
	}
}

void AGCPlayerController::ChangeProneState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeProneState();
	}
}

void AGCPlayerController::SwimForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimForward(Value);
	}
}

void AGCPlayerController::SwimRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimRight(Value);
	}
}

void AGCPlayerController::SwimUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimUp(Value);
	}
}

void AGCPlayerController::StartSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartSprint();
	}
}

void AGCPlayerController::StopSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopSprint();
	}
}

void AGCPlayerController::BeginFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartFire();
	}
}

void AGCPlayerController::FinishFire()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopFire();
	}
}

void AGCPlayerController::StartAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartAiming();
	}
}

void AGCPlayerController::StopAiming()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopAiming();
	}
}

void AGCPlayerController::Reload()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Reload();
	}
}

void AGCPlayerController::NextItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->NextItem();
	}
}

void AGCPlayerController::PreviousItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PreviousItem();
	}
}

void AGCPlayerController::PrimaryMeleeAttack()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PrimaryMeleeAttack();
	}
}

void AGCPlayerController::SecondaryMeleeAttack()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SecondaryMeleeAttack();
	}
}

void AGCPlayerController::EquipPrimaryItem()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->EquipPrimaryItem();
	}
}

void AGCPlayerController::ToggleMainMenu()
{
	if (!IsValid(OptionsWidget))
	{
		return;
	}

	if (!IsValid(PlayerHUDWidget))
	{
		return;
	}
	if (IsLocalPlayerController())
	{
		if (OptionsWidget->IsVisible())
		{
			if (OptionsWidget)
			{
				OptionsWidget->RemoveFromParent();
			}
			PlayerHUDWidget->AddToViewport();
			SetInputMode(FInputModeGameOnly{});
			bShowMouseCursor = false;
		}
		else
		{
			if (PlayerHUDWidget)
			{
				PlayerHUDWidget->RemoveFromParent();
			}
			OptionsWidget->AddToViewport();
			SetInputMode(FInputModeUIOnly{});
			bShowMouseCursor = true;
		}
	}
}


void AGCPlayerController::SpawnPlayer(FName TeamTag, TSubclassOf<AGCBaseCharacter> CahracterToSpawn)
{
	AGameCodeGameModeBase* GameMode = Cast<AGameCodeGameModeBase>(GetWorld()->GetAuthGameMode());

	TArray<AActor*> FoundSpawns;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AMultiplayerPlayerStart::StaticClass(), TeamTag, FoundSpawns);

	/*FTransform PlayerSpawnTrans;

	if (AGCBaseCharacter* PawnIn = GameMode->SpawnDefaultPawnFor(this, );
	{
		Possess(PawnIn);
		AGCBaseCharacter* CharacterIn = Cast<AGCBaseCharacter>(PawnIn);
		CreateAndInitializeWidtget();
	}*/
}

void AGCPlayerController::LaunchDrone()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LaunchDrone();
	}
}

void AGCPlayerController::ConnectToDrone()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ConnectToDrone();
	}
}

void AGCPlayerController::CreateChangeTeamWidget()
{
	if (!IsValid(TeamSelectWidget))
	{
		TeamSelectWidget = CreateWidget<UTeamSelectWidget>(GetWorld(), TeamSelectWidgetClass);
		if (IsValid(TeamSelectWidget))
		{
			TeamSelectWidget->AddToViewport();
			SetInputMode(FInputModeUIOnly{});
			bShowMouseCursor = true;
		}
	}
}

UPlayerHUDWidget* AGCPlayerController::GetPlayerHUDWidget()
{
	return PlayerHUDWidget;
}

void AGCPlayerController::CreateAndInitializeWidtget()
{
	if (!IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);
		if (IsValid(PlayerHUDWidget)) 
		{
			PlayerHUDWidget->AddToViewport();
		}
	}

	if (!IsValid(OptionsWidget)) 
	{
		OptionsWidget = CreateWidget<UOptionsMenuWidget>(GetWorld(), OptionsWidgetClass);
	}
	  
	if (IsValid(PlayerHUDWidget) && CachedBaseCharacter.IsValid())
	{
		UReticleWidget* ReticleWidget = PlayerHUDWidget->GetReticleWidget();
		if (IsValid(ReticleWidget)) 
		{
			CachedBaseCharacter->OnAimingStateChanged.AddUFunction(ReticleWidget, FName("OnAimingStateChanged"));
			UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
			CharacterEquipment->OnEquippedItemChanged.AddUFunction(ReticleWidget, FName("OnEquippedItemChanged"));
		}

		UAmmoWidget* AmmoWidget = PlayerHUDWidget->GetAmmoWidget();
		if (IsValid(AmmoWidget)) 
		{
			UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
			CharacterEquipment->OnCurrentWeaponAmmoChangedEvent.AddUFunction(AmmoWidget, FName("UpdateAmmoCount")); 
		}

		if (IsValid(AmmoWidget))
		{
			UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent_Mutable();
			CharacterEquipment->OnCurrentFragGranadesChangedEvent.AddUFunction(AmmoWidget, FName("UpdateFragGranadesCount"));
		}
	}

	SetInputMode(FInputModeGameOnly{});
	bShowMouseCursor = false;
}