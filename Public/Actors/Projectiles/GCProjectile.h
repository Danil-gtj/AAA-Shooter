#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GCProjectile.generated.h"

class AGCProjectile;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnProjectileHit, AGCProjectile*, Projectile,const FHitResult&, Hit,const FVector&, Direction);

UCLASS()
class GAMECODE_API AGCProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AGCProjectile();

	UFUNCTION(BlueprintCallable)
	void LaunchProjectile(FVector Direction);

	virtual void OnProjectileLaunched();

	UPROPERTY(BlueprintAssignable)
	FOnProjectileHit OnProjectileHit;

	UFUNCTION(BlueprintNativeEvent)
	void SetProjectileActive(bool bIsProjectileActive);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components | VFX")
	UParticleSystem* BulletHitEffect;

	virtual void BeginPlay() override;

private: 
	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
