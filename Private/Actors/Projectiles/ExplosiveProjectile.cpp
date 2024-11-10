#include "Actors/Projectiles/ExplosiveProjectile.h"
#include "Components/Scene/ExplosionComponent.h"

AExplosiveProjectile::AExplosiveProjectile()
{
	ExplosionComponent = CreateDefaultSubobject<UExplosionComponent>("ExplosionComponent");
	ExplosionComponent->SetupAttachment(GetRootComponent());
}

void AExplosiveProjectile::OnProjectileLaunched()
{
	Super::OnProjectileLaunched();
	GetWorld()->GetTimerManager().SetTimer(DetonationTimer, this, &AExplosiveProjectile::DetonationTimerElapsed, false);
}

void AExplosiveProjectile::DetonationTimerElapsed()
{
	ExplosionComponent->Explode(GetController());
}

AController* AExplosiveProjectile::GetController()
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	return IsValid(PawnOwner) ? PawnOwner->GetController() : nullptr;
}
