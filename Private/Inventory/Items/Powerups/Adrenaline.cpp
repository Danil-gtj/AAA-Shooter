// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Items/Powerups/Adrenaline.h"
#include "Character/CharacterComponents/ActorAttributeComponent.h"
#include "Character/GCBaseCharacter.h"

bool UAdrenaline::Consume(AGCBaseCharacter* ConsumeTarget)
{
    UActorAttributeComponent* CharacterAttributeComponent = ConsumeTarget->GetChracterAttributesComponent();
    CharacterAttributeComponent->RestoreFullStamina();
    this->ConditionalBeginDestroy();
    return true;
}
