// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"

#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
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
		FVector Direction = (HitTarget - Start).GetSafeNormal();
		FVector ExtendedEnd = Start + Direction * 10000.f; // 예: 10000cm = 100m

		
		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(this);
		queryParams.AddIgnoredActor(GetOwner());
		FHitResult FireHitResult;
		UWorld* World=GetWorld();
		if (World)
		{
			World->LineTraceSingleByChannel(
				FireHitResult,
				Start,
				ExtendedEnd,
				ECC_GameTraceChannel1,
				queryParams);
		};
		
		{
			DrawDebugLine(
			   GetWorld(),
			   Start,
			   ExtendedEnd,
			   FColor::Green,
			   false, 2.f, 0, 1.f
		   );
			DrawDebugSphere(
				GetWorld(),
				FireHitResult.Location,
				10,
				20,
				FColor::Green,
				false,
				2.f
			);
		}
		
		FVector BeamEnd=ExtendedEnd;
		if (FireHitResult.bBlockingHit)
		{
			if (FireHitResult.GetActor())
				UE_LOG(LogTemp, Log, TEXT("Hit Actor Name: %s"), *FireHitResult.GetActor()->GetName());

			BeamEnd=FireHitResult.ImpactPoint;
			ACDCharacter* CDCharacter=Cast<ACDCharacter>(FireHitResult.GetActor());
			if (CDCharacter && OwnerController)
			{
				UGameplayStatics::ApplyPointDamage(
                			CDCharacter,
                			Damage,
                			GetActorForwardVector(),
                			FireHitResult,
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
			USoundCue* SoundToPlay = FireHitResult.GetActor() ? HitBodySound : HitSurfaceSound;
			if (SoundToPlay)
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					SoundToPlay,
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

bool AHitScanWeapon::bShowSniperScope()
{
	ACDCharacter* CDCharacter=Cast<ACDCharacter>(GetOwner());
	if (HasAuthority()&&CDCharacter&&GetWeaponType()==EWeaponType::EWT_Sniper)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Called when the game starts or when spawned
void AHitScanWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHitScanWeapon::ShowSniperScope()
{
	ACDCharacter* CDCharacter=Cast<ACDCharacter>(GetOwner());
	ACDPlayerController* PC=Cast<ACDPlayerController>(CDCharacter->GetController());
	if (bShowSniperScope())
	{
		//PC->SetHUDSniperScope();
	}
}



