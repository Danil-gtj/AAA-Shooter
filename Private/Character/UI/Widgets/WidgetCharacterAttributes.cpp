// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/Widgets/WidgetCharacterAttributes.h"
#include "Character/GCBaseCharacter.h"
#include "Character/CharacterComponents/ActorAttributeComponent.h"

float UWidgetCharacterAttributes::GetStaminaPercent() const
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

float UWidgetCharacterAttributes::GetOxygenPercent() const
{
	float Result = 0.0f;
	APawn* Pawn = GetOwningPlayerPawn();
	AGCBaseCharacter* Character = Cast<AGCBaseCharacter>(Pawn);
	if (IsValid(Character))
	{
		const UActorAttributeComponent* CharacterAttributes = Character->GetChracterAttributesComponent();
		Result = CharacterAttributes->GetOxygenPercent();
	}

	return Result;
}

float UWidgetCharacterAttributes::GetHealthPercent() const
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
