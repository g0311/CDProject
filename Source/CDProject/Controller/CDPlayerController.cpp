// Fill out your copyright notice in the Description page of Project Settings.


#include "CDPlayerController.h"

#include "AbilitySystemComponent.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Character/CDCharacterAttributeSet.h"
#include "CDProject/Component/CombatComponent.h"
#include "CDProject/HUD/CDHUD.h"
#include "CDProject/Weapon/Weapon.h"
#include "CDProject/Widget/CharacterOverlay.h"
#include "CDProject/Widget/KDOverlay.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"


ACDPlayerController::ACDPlayerController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ACDPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	SetHUDTime();
}

void ACDPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// CDHUD=Cast<ACDHUD>(GetHUD());
	// {
	// 	CDHUD->AddCharacterOverlay(); 
	// }
}



void ACDPlayerController::HandleCooldown()
{
}


bool ACDPlayerController::EnsureHUD()
{
	CDHUD=CDHUD==nullptr?Cast<ACDHUD>(GetHUD()):CDHUD;
	return CDHUD&&CDHUD->CharacterOverlay;
}

void ACDPlayerController::SetHUDTime()
{
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

void ACDPlayerController::SetHUDShield(float Shield, float MaxShield)
{
	if (EnsureHUD())
	{
		// const float ShieldPercent = Shield/MaxShield;
		// //CDHUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);
		// FString HealthText=FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		// CDHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	
}

// void ACDPlayerController::SetHUDKill(float killcount)
// {
// 	if (EnsureHUD()&&CDHUD->CharacterOverlay->KillCount)
// 	{
// 		FString KillCount=FString::Printf(TEXT("%d"), FMath::CeilToInt(killcount));
// 		CDHUD->CharacterOverlay->KillCount->SetText(FText::FromString(KillCount));
// 	}
// }

// void ACDPlayerController::SetHUDDeath(float deathcount)
// {
// 	if (EnsureHUD()&&CDHUD->CharacterOverlay->DeathCount)
// 	{
// 		FString DeathCount=FString::Printf(TEXT("%d"), FMath::CeilToInt(deathcount));
// 		CDHUD->CharacterOverlay->DeathCount->SetText(FText::FromString(DeathCount));
// 	}
// }

void ACDPlayerController::SetHUDWeaponAmmo(int32 WeaponAmmo)
{
	if (EnsureHUD() && CDHUD->CharacterOverlay->WeaponAmmoAmount)
	{
		FString WeaponAmmoText = FString::Printf(TEXT("%d"), WeaponAmmo);
		CDHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(WeaponAmmoText));
	}
}

void ACDPlayerController::SetHUDWeaponInfo(AWeapon* Weapon)
{
	if (Weapon == nullptr) return;

	if (EnsureHUD() && CDHUD->CharacterOverlay)
	{
		FString WeaponName = Weapon->WeaponName;
		if (!WeaponName.IsEmpty() && CDHUD->CharacterOverlay->WeaponName)
		{
			CDHUD->CharacterOverlay->WeaponName->SetText(FText::FromString(WeaponName));
		}

		UTexture2D* WeaponImage = Weapon->WeaponImage;
		if (WeaponImage && CDHUD->CharacterOverlay->WeaponImage)
		{
			CDHUD->CharacterOverlay->WeaponImage->SetBrushFromTexture(WeaponImage);
		}
	}
}

void ACDPlayerController::SetHUDCarriedAmmo(int32 CarriedAmmo)
{
	if (EnsureHUD() && CDHUD->CharacterOverlay->CarriedAmmoAmount)
	{
		FString CarriedText = FString::Printf(TEXT("%d"), CarriedAmmo);
		CDHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(CarriedText));
	}
}

void ACDPlayerController::SetHUDCount(float CountdownTime)
{
	if (EnsureHUD() && CDHUD->CharacterOverlay->MatchCountdownText)
	{
		FString CountdownText = FString::Printf(TEXT("%f"), CountdownTime);
		CDHUD->CharacterOverlay->MatchCountdownText;
	}
}

void ACDPlayerController::InitializeHUD()
{
	if (EnsureHUD())
	{
		if (bInitializeHealth)SetHUDHealth(HUDHealth, HUDMaxHealth);
		if (bInitializeCarriedAmmo)SetHUDCarriedAmmo(HUDCarriedAmmo);
		if (bInitializeWeaponAmmo)SetHUDWeaponAmmo(HUDWeaponAmmo);
		//if (bInitializeShield)SetHUDShield(HUDShield);
		// if (bInitializeKill)SetHUDKill(HUDKillCount);
		// if (bInitializeDeath)SetHUDDeath(HUDDeathCount);
		
	}
}

void ACDPlayerController::AcknowledgePossession(class APawn* P)
{ //Client Part
	Super::AcknowledgePossession(P);
	UE_LOG(LogTemp, Warning, TEXT("ACK Called"));
	ACDCharacter* CDCharacter = Cast<ACDCharacter>(P);
	if (CDCharacter && CDCharacter->GetAbilitySystemComponent())
	{
		CDCharacter->GetAbilitySystemComponent()->InitAbilityActorInfo(CDCharacter, this);
	}
}

void ACDPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	// ACDCharacter* CDCharacter=Cast<ACDCharacter>(InPawn);
	// if (CDCharacter&&CDCharacter->GetAbilitySystemComponent())
	// {
	// 	const UCDCharacterAttributeSet* AttributeSet = CDCharacter->GetAbilitySystemComponent()->GetSet<UCDCharacterAttributeSet>();
	// 	SetHUDHealth(AttributeSet->GetHealth(), AttributeSet->GetMaxHealth());
	// 	CDCharacter->GetCombatComponent()->GetCurWeapon()->SetHUDAmmo();
	// }
}

void ACDPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}

void ACDPlayerController::OnMatchStateSet(FName State)
{
	MatchState=State;
	if (MatchState==MatchState::InProgress)
	{
		CDHUD=Cast<ACDHUD>(GetHUD());
		if (CDHUD)
		{
			CDHUD->AddCharacterOverlay();
		}
	}
}

void ACDPlayerController::HandleMatchHasStarted()
{
}

