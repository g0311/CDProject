// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"


// Sets default values
AProjectileRocket::AProjectileRocket()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RocketMesh=CreateDefaultSubobject<UStaticMeshComponent>("RocketMesh");
	SetRootComponent(RocketMesh);
	
}

// Called when the game starts or when spawned
void AProjectileRocket::BeginPlay()
{
	Super::BeginPlay();
	
}


