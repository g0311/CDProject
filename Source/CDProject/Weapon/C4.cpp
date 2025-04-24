// Fill out your copyright notice in the Description page of Project Settings.


#include "C4.h"

#include "VectorTypes.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Component/CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

AC4::AC4()
{
	PrimaryActorTick.bCanEverTick = true;
	WeaponType = EWeaponType::EWT_C4;
}

void AC4::Fire(const FVector& HitTarget)
{//Called By Server
	if (HasAuthority())
	{
		if (UE::Geometry::Distance(GetActorLocation(), HitTarget) >= 100.f)
			return;
		
		if (GetWorld()->GetTimerManager().IsTimerActive(_plantTimer))
		{
			ACDCharacter* character = Cast<ACDCharacter>(GetOwner());
			if (!character)
				return;
			character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			GetWorld()->GetTimerManager().ClearTimer(_plantTimer);
		}
		else
		{
			ACDCharacter* character = Cast<ACDCharacter>(GetOwner());
			if (!character)
				return;
			character->GetCharacterMovement()->DisableMovement();
			GetWorld()->GetTimerManager().SetTimer(_plantTimer, this, &AC4::Plant, _plantingTime);
		}		
	}
}

void AC4::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// DOREPLIFETIME()
}

void AC4::Plant()
{
	GetWorld()->GetTimerManager().SetTimer(_explodeTimer, this, &AC4::Explode, _explodeTime);	
	//GameMode->
	ACDCharacter* character = Cast<ACDCharacter>(GetOwner());
	if (!character)
		return;
	character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	//character->RequestDropWeapon();
}

void AC4::Explode()
{
	
}
