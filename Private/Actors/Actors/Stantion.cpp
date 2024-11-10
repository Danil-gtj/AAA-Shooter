// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Actors/Stantion.h"

// Sets default values
AStantion::AStantion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStantion::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStantion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

