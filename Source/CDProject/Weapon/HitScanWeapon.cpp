// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"

#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Field/FieldSystemNodes.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
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
		FVector ExtendedEnd = Start + Direction * 10000.f;
		
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
		
		// {
		// 	DrawDebugLine(
		// 	   GetWorld(),
		// 	   Start,
		// 	   ExtendedEnd,
		// 	   FColor::Green,
		// 	   false, 2.f, 0, 1.f
		//    );
		// 	DrawDebugSphere(
		// 		GetWorld(),
		// 		FireHitResult.Location,
		// 		10,
		// 		20,
		// 		FColor::Green,
		// 		false,
		// 		2.f
		// 	);
		// }
		
		FVector BeamEnd=ExtendedEnd;
		if (FireHitResult.bBlockingHit)
		{
			if (FireHitResult.GetActor())
				UE_LOG(LogTemp, Log, TEXT("Hit Actor Name: %s"), *FireHitResult.GetActor()->GetName());
			if (FireHitResult.GetComponent())
				UE_LOG(LogTemp, Log, TEXT("Hit Actor Name: %s"), *FireHitResult.GetComponent()->GetName());

			BeamEnd=FireHitResult.ImpactPoint;
			ACharacter* CDCharacter=Cast<ACharacter>(FireHitResult.GetActor());
			if (CDCharacter && HasAuthority())
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

FVector AHitScanWeapon::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget)
{
	FVector ToTargetNormalized=(HitTarget-TraceStart).GetSafeNormal();
	FVector SphereCenter=TraceStart+DistanceToSphere*ToTargetNormalized;
	FVector RandomVector=UKismetMathLibrary::RandomUnitVector()*FMath::FRandRange(0.f,SphereRadius);
	FVector EndLoc=SphereCenter+RandomVector;
	FVector ToEndLoc=EndLoc-TraceStart;

	// DrawDebugSphere(GetWorld(),SphereCenter,SphereRadius,12,FColor::Red,true);
	// DrawDebugSphere(GetWorld(), EndLoc, 4.f, 12, FColor::Orange, true);
	// DrawDebugLine(
	// 	GetWorld(),
	// 	TraceStart,
	// 	FVector(TraceStart + ToEndLoc * 80000.f / ToEndLoc.Size()),
	// 	FColor::Cyan,
	// 	true);
	return FVector(TraceStart + ToEndLoc * 80000.f / ToEndLoc.Size());
}

void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FVector End = bUseScatter ? TraceEndWithScatter(TraceStart, HitTarget) : TraceStart + (HitTarget - TraceStart) * 1.25f;
 
		World->LineTraceSingleByChannel(
			OutHit,
			TraceStart,
			End,
			ECC_GameTraceChannel1
		);
		FVector BeamEnd = End;
		if (OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;
		}
		if (BeamParticleSystem)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				World,
				BeamParticleSystem,
				TraceStart,
				FRotator::ZeroRotator,
				true
			);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}//Particle need BeamEnd
		}
	}
}



