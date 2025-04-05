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
	APawn* InstigatorPawn=Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleFlashSocket=GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	UE_LOG(LogTemp,Display,TEXT("Fire"));
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform=MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		if (ProjectileClass&&InstigatorPawn)
		{
			UE_LOG(LogTemp,Display,TEXT("Projectile spawn"));
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator=InstigatorPawn;
			UWorld* World = GetWorld();
			if (World)
			{
				AProjectile* Projectile=World->SpawnActor<AProjectile>(
					ProjectileClass,
					SocketTransform.GetLocation()+SocketTransform.GetRotation().GetForwardVector()*100.f,
					TargetRotation,
					SpawnParams
					);
				if (Projectile)
				{
					UE_LOG(LogTemp, Warning, TEXT("Projectile spawn2"));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Projectile failed!"));
				}
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
				if (Projectile)
				{
					FVector SpawnLocation = Projectile->GetActorLocation();
					UE_LOG(LogTemp, Warning, TEXT("Projectile Spawned At: X=%.2f Y=%.2f Z=%.2f"), SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);
				}
			
			}
		}
		
	}
}


