// Fill out your copyright notice in the Description page of Project Settings.


#include "C4Weapon.h"

#include "Projectile.h"
#include "VectorTypes.h"
//#include "AssetTypeActions/AssetDefinition_SoundBase.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Component/CombatComponent.h"
#include "CDProject/GameMode/DemolitionGameMode.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AC4Weapon::AC4Weapon()
{
	PrimaryActorTick.bCanEverTick = true;
	WeaponType = EWeaponType::EWT_C4;
}

void AC4Weapon::Fire(const FVector& HitTarget)
{
	if (IsValid(_plantedSound))
		UGameplayStatics::PlaySound2D(this, _plantedSound);

	ACDCharacter* character = Cast<ACDCharacter>(GetOwner());
	if (character && character->HasAuthority())
	{
		//Spawn C4 Projectile
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = character;
		SpawnParams.Instigator=character;
		UWorld* World = GetWorld();
		FVector _plantLocation =
			character->GetActorLocation() - FVector(0, 0, character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		if (!_projectileClass||!World) return;
		AProjectile* c4Projectile = World->SpawnActor<AProjectile>(
			_projectileClass,
			_plantLocation,
			FRotator::ZeroRotator,
			SpawnParams
		);
		//GameMode Set Time
		if (GetWorld()->GetAuthGameMode())
		{
			ADemolitionGameMode* teamGameMode = Cast<ADemolitionGameMode>(GetWorld()->GetAuthGameMode());
			if (teamGameMode)
			{
				teamGameMode->SetMatchTime(c4Projectile->GetDestroyTime());

				teamGameMode->AddDestroyableActor(c4Projectile);
			}
		}
		Destroy();
	}
}