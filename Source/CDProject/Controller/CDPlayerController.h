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

	void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	//HUD

	void SetHUDTime();
	void SetHUDHealth(float Health);
	void SetHUDShield(float Shield);
	void SetHUDKill(float killcount);
	void SetHUDDeath(float deathcount);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDWeaponInfo(class AWeapon* weapon);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDMatchCount(float CountdownTime);
	void SetHUDAnnouncementCountdown(float Countdown);
	void SetTeamScore();
	void SetMinimap();
	void SetGold();
	void SetKDOverlayUI();
	void UpdateKDOverlayData();

	UFUNCTION(Client,Reliable)
	void Client_ShowStoreWidget(bool IsActivate);

	//bShowOverlay
	
	void ShowStoreWidget(bool bShow);
	void RetryShowStoreWidget(bool bActivate);
	//Weapon
	void ShowSniperScope();
	void ShowC4PlantingProgress(bool isPlanting, float duration = 0.f);
	void ShowC4DefusingProgress(bool isDefusing, float duration = 0.f);

	//HUD initialize
	void InitializeHUD();

	//TeamMatch Controller
	void HideRoundScore(bool IsHide);
	void SetHUDRedTeam(int32 RedScore);
	void SetHUDBlueTeam(int32 BlueScore);
	
	//MatchState
	virtual void AcknowledgePossession(class APawn* P) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void ReceivedPlayer() override;
	virtual float GetServerTime();

	//KDO Overlay
	void ShowKDOverlay(bool isShowing);
	
	void OnMatchStateSet(FName State, bool bTeamsMatch=false);
	void HandleMatchHasStarted(bool bTeamsMatch=false);
	void HandleCooldown();

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime);

	
	

	
protected:
	virtual void BeginPlay() override;

//Sync Time
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);

	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta=0.f;

	UPROPERTY(EditAnywhere, Category=Time)
	float TimeSyncFrequency=5.f;

	float TimeSyncRunningTime=0.f;
	void CheckTimeSync(float DeltaTime);

	
private:
	UPROPERTY()
	class ACDHUD* CDHUD;

	UPROPERTY()
	class ACDPlayerState* PS;
	
	UPROPERTY(EditAnywhere, Category="HUD")
	TSubclassOf<class UKDOverlay> KDOverlay;
	
	UPROPERTY(EditAnywhere, Category="HUD")
	TSubclassOf<class UCharacterOverlay> CharacterOverlay;

	UPROPERTY(ReplicatedUsing=OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();
	UFUNCTION()
	void OnRep_HUDGoldCount();
	UFUNCTION()
	void OnRep_HUDKillCount();
	UFUNCTION()
	void OnRep_HUDDeathCount();
	
	//MatchVariable
	float LevelStartingTime=0.f;//Purchase Item Time
	float MatchTime=120.f;
	float WarmupTime=0.f;
	float CooldownTime=0.f;
	int32 CountdownInt=0;
	
	//State Variable
	float HUDHealth;
	float HUDMaxHealth;
	float HUDShield;
	float HUDCarriedAmmo;
	float HUDWeaponAmmo;
	class Aweapon* HUDWeaponInfo;
	


	
	//Match KDState
	UPROPERTY(ReplicatedUsing=OnRep_HUDGoldCount)
	float HUDGoldCount;
	UPROPERTY(ReplicatedUsing=OnRep_HUDKillCount)
	float HUDKillCount;
	UPROPERTY(ReplicatedUsing=OnRep_HUDDeathCount)
	float HUDDeathCount;
	UPROPERTY(Replicated)
	FName HUDCharID;

	//bool Initialize
	bool bInitializeHealth=false;
	bool bInitializeShield=false;
	bool bInitializeKill=false;
	bool bInitializeDeath=false;
	bool bInitializeCarriedAmmo=false;
	bool bInitializeWeaponAmmo=false;
	bool bInitializeWeaponInfo=false;
	bool bInitializeGold=false;
	

};

