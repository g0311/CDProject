// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"

#include "CDProject/Character/CDCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


// Sets default values
AHitScanWeapon::AHitScanWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	WeaponType = EWeaponType::EWT_Pistol;
}

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	APawn* OwnerPawn=Cast<APawn>(GetOwner());
	AController* InstigatorController=OwnerPawn->GetController();
	const USkeletalMeshSocket* MuzzleFlashSocket=GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform=MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start=SocketTransform.GetLocation();
		FVector End=Start+(HitTarget-Start)*1.25f;

		FHitResult FireHitResult;
		UWorld* World=GetWorld();
		if (World)
		{
			World->LineTraceSingleByChannel(
				FireHitResult,
				Start,
				End,
				ECC_Visibility);
		};
		FVector BeamEnd=End;
		if (FireHitResult.bBlockingHit)
		{
			BeamEnd=FireHitResult.ImpactPoint;
			ACDCharacter* CDCharacter=Cast<ACDCharacter>(FireHitResult.GetActor());
			if (CDCharacter)
			{
				UGameplayStatics::ApplyDamage(CDCharacter,
					Damage,
					InstigatorController,
					this,
					UDamageType::StaticClass()
					);
			}
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(World,
					ImpactParticles,
					FireHitResult.ImpactPoint,
					FireHitResult.ImpactNormal.Rotation());
			}
			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					HitSound,
					FireHitResult.ImpactPoint
					);
			}
			if (BeamParticleSystem)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					World,
					BeamParticleSystem,
					SocketTransform
					);
			}
			if (MuzzleFlash)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					World,
					MuzzleFlash,
					SocketTransform
					);
			}
			if (FireSound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					FireSound,
					SocketTransform.GetLocation()
					);
			}
		}
	}
}

// Called when the game starts or when spawned
void AHitScanWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}



