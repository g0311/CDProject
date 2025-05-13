// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"

//#include "BaseGizmos/HitTargets.h"
#include "CDProject/Character/CDCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AShotgun::AShotgun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AShotgun::Fire(const FVector& HitTarget)
{
	AWeapon::Fire(HitTarget);
	APawn* OwnerPawn=Cast<APawn>(GetOwner());
	AController* InstigatorController=OwnerPawn->GetController();
	
	const USkeletalMeshSocket* MuzzleFashSocket=GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFashSocket)
	{
		FTransform SocketTransform=MuzzleFashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start=SocketTransform.GetLocation();
		TMap<ACDCharacter*, uint32> HitMap;
		for (uint32 i=0;i<NumberOfPellets;i++)
		{
			FVector End=TraceEndWithScatter(Start,HitTarget);
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);
			ACDCharacter* CDCharacter=Cast<ACDCharacter>(FireHit.GetActor());
			if (HasAuthority()&&CDCharacter)
			{
				if (HitMap.Contains(CDCharacter))
				{
					HitMap[CDCharacter]++;
				}
				else
				{
					HitMap.Emplace(CDCharacter,1);
				}
			}
			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					FireHit.ImpactPoint,
					FireHit.ImpactNormal.Rotation());
			}
			for (auto HitPair:HitMap)
			{
				if (HitPair.Key&&HasAuthority())
				{
					UGameplayStatics::ApplyDamage(
						HitPair.Key,
						Damage*HitPair.Value,
						InstigatorController,
						this,
						UDamageType::StaticClass());
				}
			}
		}
	}
}

void AShotgun::Reload()
{
	CarriedAmmo=GetCarriedAmmo();
	int32 ReloadAmount=1;
	if (Ammo < AmmoCapacity)
	{
		Ammo+=ReloadAmount;
		SpendCarriedAmmo(ReloadAmount);
		SetHUDAmmo();
	}
}

