// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"

#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AProjectileWeapon::AProjectileWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectileWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	if (!HasAuthority()) return;
	APawn* InstigatorPawn=Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleFlashSocket=GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket)
	{
		
		FTransform SocketTransform=MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector MuzzleLocation = SocketTransform.GetLocation();
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		if (ProjectileClass&&InstigatorPawn)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator=InstigatorPawn;
			UWorld* World = GetWorld();
			if (World)
			{
				AProjectile* Projectile=World->SpawnActor<AProjectile>(
					ProjectileClass,
					MuzzleLocation,
					TargetRotation,
					SpawnParams
					);
				if (Projectile)
				{
					DrawDebugSphere(
						GetWorld(),
						Projectile->GetActorLocation(),
						10.f, 
						12,   
						FColor::Red,
						false, 
						5.0f,  
						0,
						2.0f   
					);
				}
				FVector StartLocation = Projectile->GetActorLocation();
				FVector EndLocation = StartLocation + Projectile->GetActorForwardVector() * 700.f; 
				DrawDebugDirectionalArrow(
					GetWorld(),
					StartLocation,
					EndLocation,
					100.f,  
					FColor::Blue,
					false,
					5.0f,
					0,
					2.0f
				);
			
			
			}
		}
		
	}
}


