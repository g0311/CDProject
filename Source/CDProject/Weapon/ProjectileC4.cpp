// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileC4.h"

#include "Net/UnrealNetwork.h"


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
		//GameMode Red Team Win
	}
	Super::Destroyed();
}

void AProjectileC4::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProjectileC4, _isDefused);
}

void AProjectileC4::Defused()
{
	if (HasAuthority())
	{
		_isDefused = true;
		if (DestroyTimer.IsValid() && GetWorld()->GetTimerManager().IsTimerActive(DestroyTimer))
		{
			GetWorld()->GetTimerManager().ClearTimer(DestroyTimer);
		}
		//GameMode Blue Team Win
	}
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

