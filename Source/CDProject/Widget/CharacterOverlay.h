// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */

UCLASS()
class CDPROJECT_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	//Status
	UPROPERTY(meta=(BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* HealthText;
//Team Round Score
	UPROPERTY(meta=(BindWidget))
	UTextBlock* RedTeamScore;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* BlueTeamScore;
	
	
	//Weapon
	UPROPERTY(meta=(BindWidget))
	UTextBlock* WeaponName;
	
	UPROPERTY(meta=(BindWidget))
	class UImage* WeaponImage;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* WeaponAmmoAmount;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CarriedAmmoAmount;


	//Timer
	UPROPERTY(meta=(BindWidget))
	UTextBlock* MatchCountdownText;
};
