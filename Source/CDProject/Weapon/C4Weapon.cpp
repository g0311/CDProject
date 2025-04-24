// Fill out your copyright notice in the Description page of Project Settings.


#include "C4Weapon.h"

#include "Projectile.h"
#include "VectorTypes.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Component/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

AC4Weapon::AC4Weapon()
{
	PrimaryActorTick.bCanEverTick = true;
	WeaponType = EWeaponType::EWT_C4;
}

void AC4Weapon::Fire(const FVector& HitTarget)
{
	ACDCharacter* character = Cast<ACDCharacter>(GetOwner());
	if (!character || !character->HasAuthority())
		return;

	UE_LOG(LogTemp, Log, TEXT("Called"));
	
	//Spawn C4 Projectile
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = character;
	SpawnParams.Instigator=character;
	UWorld* World = GetWorld();
	FVector _plantLocation =
		character->GetActorLocation() - FVector(0, 0, character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	if (!_projectileClass||!World) return;
	AProjectile* Grenade = World->SpawnActor<AProjectile>(
		_projectileClass,
		_plantLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);
	
	Destroy();
}