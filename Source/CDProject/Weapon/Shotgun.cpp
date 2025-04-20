// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"

#include "BaseGizmos/HitTargets.h"
#include "CDProject/Character/CDCharacter.h"
#include "Engine/SkeletalMeshSocket.h"


// Sets default values
AShotgun::AShotgun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AShotgun::Fire(const FVector& HitTarget)
{
	AWeapon::Fire(HitTarget);

	const USkeletalMeshSocket* MuzzleFashSocket=GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFashSocket)
	{
		FTransform SocketTransform=MuzzleFashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start=SocketTransform.GetLocation();
		TMap<ACDCharacter*, uint32> HitMap;
		for (uint32 i=0;i<NumberOfPellets;i++)
		{
			FVector End=TraceEndWithScatter(Start,HitTarget);
			// FHitResult FireHit;
			// WeaponTraceHit(Start, HitTarget, FireHit);
		}
	}
}

