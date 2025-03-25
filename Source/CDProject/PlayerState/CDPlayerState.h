// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CDPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class CDPROJECT_API ACDPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	int32 CurrentHealth;
	UPROPERTY(VisibleAnywhere)
	int32 MaxHealth;
	UPROPERTY(VisibleAnywhere)
	int32 KillCount;
	UPROPERTY(VisibleAnywhere)
	int32 DeathCount;
	UPROPERTY(VisibleAnywhere)
	int32 Level;

	UPROPERTY(EditAnywhere)
	int Ammo;
	UPROPERTY(EditAnywhere)
	int MaxAmmo;
	UPROPERTY(EditAnywhere)
	int CarriedAmmo;

	UPROPERTY()
	class ACDCharacter* Character;
	UPROPERTY()
	class ACDPlayerController* Controller;

	UPROPERTY(VisibleAnywhere)
	EWeaponType WeaponType;
	UPROPERTY(VisibleAnywhere)
	EWeaponState WeaponState;
	
};
