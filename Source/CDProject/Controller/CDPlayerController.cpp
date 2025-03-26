// Fill out your copyright notice in the Description page of Project Settings.


#include "CDPlayerController.h"

#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Widget/CDHUD.h"
#include "CDProject/Widget/CharacterOverlay.h"
#include "CDProject/Widget/CharacterStateOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"


ACDPlayerController::ACDPlayerController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// void ACDPlayerController::SetHUD()
// {
// 	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
// 	if (CDHUD&&CDHUD->CharacterOverlay)
// 	{
// 		SetHUDHealth();
// 		SetHUDKill();
// 		SetHUDDeath();
// 		SetHUDWeaponAmmo(Ammo);
// 		SetHUDCarriedAmmo(Ammo);
// 		
// 	}
// }

void ACDPlayerController::BeginPlay()
{
	Super::BeginPlay();
	CDHUD=Cast<ACDHUD>(GetHUD());
}


bool ACDPlayerController::EnsureHUD()
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	return CDHUD&&CDHUD->CharacterOverlay;
}

void ACDPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	if (EnsureHUD())
	{
		const float HealthPercent = Health/MaxHealth;
		CDHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText=FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		CDHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	
}

void ACDPlayerController::SetHUDKill(float killcount)
{
	if (EnsureHUD()&&CDHUD->StateOverlay->KillCount)
	{
		FString KillCount=FString::Printf(TEXT("%d"), FMath::CeilToInt(killcount));
		CDHUD->StateOverlay->KillCount->SetText(FText::FromString(KillCount));
	}
}

void ACDPlayerController::SetHUDDeath(float deathcount)
{
	if (EnsureHUD()&&CDHUD->StateOverlay->DeathCount)
	{
		FString DeathCount=FString::Printf(TEXT("%d"), FMath::CeilToInt(deathcount));
		CDHUD->StateOverlay->DeathCount->SetText(FText::FromString(DeathCount));
	}
}

void ACDPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	if (EnsureHUD() && CDHUD->CharacterOverlay->WeaponAmmoAmount)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		CDHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void ACDPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	if (EnsureHUD() && CDHUD->CharacterOverlay->CarriedAmmoAmount)
	{
		FString CarriedText = FString::Printf(TEXT("%d"), Ammo);
		CDHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(CarriedText));
	}
}

void ACDPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ACDCharacter* CDCharacter=Cast<ACDCharacter>(InPawn);
	if (CDCharacter)
	{
		CachedCharacter=CDCharacter;
		if (CDCharacter)
		{
			//SetHUDHealth(CDCharacter->GetHealth(), CDCharacter->GetMaxHealth());
			//Character Edit Need
		}
		SetHUD();
	}
}

