// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CDPlayerController.generated.h"

UCLASS()
class CDPROJECT_API ACDPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACDPlayerController();

	bool EnsureHUD();
	//void SetHUD();

	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDKill(float killcount);
	void SetHUDDeath(float deathcount);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	virtual void OnPossess(APawn* InPawn) override;
	

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class ACDHUD* CDHUD;

	UPROPERTY()
	class UGameStateOverlay* GameStateOverlay;
	
	UPROPERTY()
	class UCharacterOverlay* Overlay;

	//ACDCharacter* CachedCharacter;

};
