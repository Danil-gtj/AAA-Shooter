#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectiles/GCProjectile.h"
#include "ExplosiveProjectile.generated.h"

class UExplosionComponent;
UCLASS()
class GAMECODE_API AExplosiveProjectile : public AGCProjectile
{
	GENERATED_BODY()
	
public:	
	AExplosiveProjectile();

	virtual void OnProjectileLaunched() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UExplosionComponent* ExplosionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="Explosion")
	float DetonationTime = 4.0f;


private:
	void DetonationTimerElapsed();
	
	AController* GetController();

	FTimerHandle DetonationTimer;
};
