// Fill out your copyright notice in the Description page of Project Settings.


#include "HandWeapon.h"

#include "AssetTypeCategories.h"
#include "ProjectileGrenade.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AHandWeapon::AHandWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AHandWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AHandWeapon::Fire(const FVector& HitTarget)
{
	FVector StartLocation=GetActorLocation();
	FVector ToTarget=HitTarget-StartLocation;

	FVector ThrowVector=ToTarget.GetSafeNormal()*ThrowPower;
	
	APawn* InstigatorPawn=Cast<APawn>(GetOwner());
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator=InstigatorPawn;
	UWorld* World = GetWorld();
	if (!Grenadeclass||!World) return;
	FVector LaunchVelocity;
	 bool bHaveAimSolution = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
	 this,
	 				LaunchVelocity,
	 				StartLocation,
	 				HitTarget,
	 				0.f,        
	 				0.5f        // ArcParameter
	 );
	 if (bHaveAimSolution)
	 {
		AProjectileGrenade* Grenade = World->SpawnActor<AProjectileGrenade>(
			Grenadeclass,
			StartLocation,
			FRotator::ZeroRotator,
			SpawnParams
		);
		
		if (Grenade && Grenade->ProjectileMovementComponent)
		{
			Grenade->ProjectileMovementComponent->Velocity = LaunchVelocity;
			//Grenade->ProjectileMovementComponent->Velocity = GetActorForwardVector() * ThrowPower;
		}
	}
	else if (!bHaveAimSolution)
	{
		LaunchVelocity = (HitTarget - StartLocation).GetSafeNormal() * ThrowPower;

		AProjectileGrenade* Grenade = World->SpawnActor<AProjectileGrenade>(
	   Grenadeclass,
	   StartLocation,
	   FRotator::ZeroRotator,
	   SpawnParams
   );

		if (Grenade && Grenade->ProjectileMovementComponent)
		{
			Grenade->ProjectileMovementComponent->Velocity = LaunchVelocity;
		}
	}
	Destroy();
}


// Called every frame
void AHandWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

