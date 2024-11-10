#include "Character/CharacterComponents/ActorAttributeComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Character/UI/Widgets/PlayerHUDWidget.h"
#include "Character/UI/Widgets/ReticleWidget.h"
#include "Character/UI/Widgets/AmmoWidget.h"
#include "Character/UI/Widgets/WidgetCharacterAttributes.h"
#include "Character/GCBaseCharacter.h"

UReticleWidget* UPlayerHUDWidget::GetReticleWidget()
{
	return WidgetTree->FindWidget<UReticleWidget>(ReticleWidgetName);
}

UAmmoWidget* UPlayerHUDWidget::GetAmmoWidget()
{
	return WidgetTree->FindWidget<UAmmoWidget>(AmmoWidgetName);
}

UWidgetCharacterAttributes* UPlayerHUDWidget::GetWidgetAttributes()
{
	return WidgetTree->FindWidget<UWidgetCharacterAttributes>(AttributesWidgetName);
}

float UPlayerHUDWidget::GetStaminaPercent() const
{
	float Result = 0.0f;
	APawn* Pawn = GetOwningPlayerPawn();
	AGCBaseCharacter* Character = Cast<AGCBaseCharacter>(Pawn);
	if (IsValid(Character))
	{
		const UActorAttributeComponent* CharacterAttributes = Character->GetChracterAttributesComponent();
		Result = CharacterAttributes->GetStaminaPercent();	
	}

	return Result;
}

float UPlayerHUDWidget::GetHealthPercent() const
{
	float Result = 0.0f;
	APawn* Pawn = GetOwningPlayerPawn();
	AGCBaseCharacter* Character = Cast<AGCBaseCharacter>(Pawn);
	if (IsValid(Character))
	{
		const UActorAttributeComponent* CharacterAttributes = Character->GetChracterAttributesComponent();
		Result = CharacterAttributes->GetHealthPercent();
	}

	return Result;
}