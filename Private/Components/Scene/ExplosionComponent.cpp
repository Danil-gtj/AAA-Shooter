#include "Components/Scene/ExplosionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Net/UnrealNetwork.h"

void UExplosionComponent::Explore(AController* Controller)
{
	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_Explode(Controller);
	}

	if (GetOwner()->HasAuthority())
	{
		Explode(Controller);
	}
}

void UExplosionComponent::Explode(AController* Controller) 
{
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(GetOwner());
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), MaxDamage, MinDamage, GetComponentLocation(), InnerRadius, OuterRadius, DamageFallOf, DamageTypeClass, IgnoredActors, GetOwner(), Controller, ECC_Visibility);

	if (IsValid(ExplosionVFX) && IsValid(ExplosionSound) && IsValid(CShake)) 
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetComponentLocation());
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetComponentLocation());
		UGameplayStatics::PlayWorldCameraShake(this, CShake, GetComponentLocation(), 0, 900, 1.f, true);
	}

	if (OnExplosion.IsBound())
	{
		OnExplosion.Broadcast();
	}
}

void UExplosionComponent::Server_Explode_Implementation(AController* Controller)
{
	Explode(Controller);
}
