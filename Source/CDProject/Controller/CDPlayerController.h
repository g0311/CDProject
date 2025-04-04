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
	virtual void Tick(float DeltaSeconds) override;
	//HUD
	bool EnsureHUD();
	void SetHUDTime();
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDShield(float Shield, float MaxShield);
	void SetHUDKill(float killcount);
	void SetHUDDeath(float deathcount);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDCount(float CountdownTime);

	//HUD initialize
	void InitializeHUD();
	
	//MatchState
	virtual void OnPossess(APawn* InPawn) override;
	virtual void ReceivedPlayer() override;
	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();
	void HandleCooldown();
	
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class ACDHUD* CDHUD;

	UPROPERTY()
	class UKDOverlay* GameStateOverlay;
	
	UPROPERTY()
	class UCharacterOverlay* Overlay;

	UPROPERTY()
	FName MatchState;

	//MatchVariable
	float LevelStartingTime=0.f;
	float MatchTime=0.f;
	float WarmupTime=0.f;
	float CooldownTime=0.f;
	
	//State Variable
	float HUDHealth;
	float HUDMaxHealth;
	float HUDShield;
	float HUDCarriedAmmo;
	float HUDWeaponAmmo;

	
	//Match KDState
	float HUDGoldCount;
	float HUDKillCount;
	float HUDDeathCount;
	FName HUDCharID;

	//bool Initialize
	bool bInitializeHealth=false;
	bool bInitializeShield=false;
	bool bInitializeKill=false;
	bool bInitializeDeath=false;
	bool bInitializeCarriedAmmo=false;
	bool bInitializeWeaponAmmo=false;
	

};

