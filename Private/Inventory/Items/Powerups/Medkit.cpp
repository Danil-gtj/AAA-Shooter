// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Items/Powerups/Medkit.h"
#include "Character/CharacterComponents/ActorAttributeComponent.h"
#include "Character/GCBaseCharacter.h"

bool UMedkit::Consume(AGCBaseCharacter* ConsumeTarget)
{
    UActorAttributeComponent* CharacterAttributeComponent = ConsumeTarget->GetChracterAttributesComponent();
    CharacterAttributeComponent->AddHealth(Health);
    this->ConditionalBeginDestroy();
    return true;
}
