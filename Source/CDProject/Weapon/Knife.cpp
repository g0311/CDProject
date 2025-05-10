// Fill out your copyright notice in the Description page of Project Settings.


#include "Knife.h"

#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AKnife::AKnife()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	WeaponType = EWeaponType::EWT_Knife;
}

void AKnife::Fire(const FVector& HitTarget)
{
	Slash();
	if (SlashMontage)
	{
		if (OwnerCharacter && OwnerCharacter->IsLocallyControlled())
		{
			if (GetWeaponMesh())
			{
				GetWeaponMesh()->PlayAnimation(SlashMontage, false);
			}
		}
		else
		{
			if (GetWeaponMesh3p())
			{
				GetWeaponMesh3p()->PlayAnimation(SlashMontage, false);
			}
		}
	}
}

// Called when the game starts or when spawned
void AKnife::BeginPlay()
{
	Super::BeginPlay();
	
}

void AKnife::Slash()
{
	if (!OwnerCharacter) return;
	FVector Start=OwnerCharacter->GetActorLocation();
	FVector FWD=OwnerCharacter->GetActorForwardVector();
	FVector End=Start+FWD*SlashRange;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(OwnerCharacter);

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params))
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.f, 0, 1.f);
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			UGameplayStatics::ApplyDamage(HitActor, SlashDamage, OwnerController, this, nullptr);
		}
	}
}

