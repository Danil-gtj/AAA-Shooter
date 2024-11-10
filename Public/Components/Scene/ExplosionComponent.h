// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ExplosionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExplosion);

class UCameraShakeBase;
class USoundCue;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMECODE_API UExplosionComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintCallable)
	void Explore(AController* Controller);

	UFUNCTION(BlueprintCallable)
	void Explode(AController* Controller);

	UFUNCTION(Server, Reliable)
	void Server_Explode(AController* Controller);

	UPROPERTY(BlueprintAssignable)
	FOnExplosion OnExplosion;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | VFX")
	UParticleSystem* ExplosionVFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Sound")
	USoundCue* ExplosionSound;

	UPROPERTY(EditAnywhere, Category = "Explosion | VFX")
	TSubclassOf<UCameraShakeBase> CShake;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Damage")
	float MaxDamage = 75.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Damage")
	float MinDamage = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Damage")
	float DamageFallOf = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Damage")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Radius")
	float InnerRadius = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Radius")
	float OuterRadius = 500.0f;

	

};
