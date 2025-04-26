// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileC4.h"


// Sets default values
AProjectileC4::AProjectileC4()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("C4 Mesh");
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	bReplicates = true;
}

void AProjectileC4::Destroyed()
{
	if (HasAuthority())
	{
		ExplodeDamage();
	}
	Super::Destroyed();
}

// Called when the game starts or when spawned
void AProjectileC4::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnTrailSystem();
	if (HasAuthority())
		StartDestroyTimer();

}
// Called every frame
void AProjectileC4::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

