// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"

#include "BaseGizmos/HitTargets.h"
#include "CDProject/Character/CDCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AShotgun::AShotgun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AShotgun::Fire(const FVector& HitTraget)
{
	AWeapon::Fire(HitTraget);

	const USkeletalMeshSocket* MuzzleFashSocket=GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFashSocket)
	{
		FTransform SocketTransform=MuzzleFashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start=SocketTransform.GetLocation();
		TMap<ACDCharacter*, uint32> HitMap;
		for (uint32 i=0;i<NumberOfPellets;i++)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start,HitTraget,FireHit);
			ACDCharacter* CDCharacter=Cast<ACDCharacter>(FireHit.GetActor());
			if (CDCharacter&&HasAuthority()&&InstigatorController)
			{
				if (HitMap.Contains(CDCharacter))
				{
					HitMap[CDCharacter]++;
				}
				else
				{
					HitMap.Emplace(CDCharacter,1);
				}
				if (ImpactParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(
					 GetWorld(),
					 ImpactParticles,
					 FireHit.ImpactPoint,
					 FireHit.ImpactNormal.Rotation()
				 );
				}
				if (HitSound)
				{
					UGameplayStatics::PlaySoundAtLocation(
					 this,
					 HitSound,
					 FireHit.ImpactPoint,
					 .5f,
					 FMath::FRandRange(-.5f, .5f)
				 );
				}
			}
			for (auto HitPair:HitMap)
			{
				if (HitPair.Key&&HasAuthority()&&InstigatorController)
				{
					UGameplayStatics::ApplyDamage(
						HitPair.Key,
						Damage*HitPair.Value,
						InstigatorController,
						this,
						UDamageType::StaticClass()
						);
				}
			}
		}
	}
}

