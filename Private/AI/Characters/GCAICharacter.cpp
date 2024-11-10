#include "Components/CharacterComponents/AIPatrollingComponent.h"
#include "AI/Characters/GCAICharacter.h"

AGCAICharacter::AGCAICharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	AIPatrollingComponent = CreateDefaultSubobject<UAIPatrollingComponent>(TEXT("AIPatrolling"));
}

UAIPatrollingComponent* AGCAICharacter::GetPatrollingComponent() const
{
	return AIPatrollingComponent;
}

UBehaviorTree* AGCAICharacter::GetBehaviorTree() const
{
	return BehaviorTree;
}
