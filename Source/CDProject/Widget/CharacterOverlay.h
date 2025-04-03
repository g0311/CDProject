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

	//Weapon
	UPROPERTY(meta=(BindWidget))
	class UImage* WeaponImage;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* WeaponAmmoAmount;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CarriedAmmoAmount;

	//Tab Bind Text
	UPROPERTY(meta=(BindWidget))
	UTextBlock* KillCount;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* DeathCount;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* GoldAmount;

	//Timer
	UPROPERTY(meta=(BindWidget))
	UTextBlock* MatchCountdownText;
};
