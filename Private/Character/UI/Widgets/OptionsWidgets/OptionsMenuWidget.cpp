#include "GenericPlatform/GenericWindow.h"
#include "Character/UI/Widgets/OptionsWidgets/OptionsMenuWidget.h"
#include "Character/GCPlayerController.h"
#include "Kismet/GameplayStatics.h"

void UOptionsMenuWidget::CloseOptionMenu()
{
	AGCPlayerController* PlayerController = Cast<AGCPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	PlayerController->ToggleMainMenu();
}
