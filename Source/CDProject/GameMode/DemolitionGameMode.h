// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoundGameMode.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/AI/CDAIController.h"
#include "AIController.h"
#include "CDProject/Widget/ShopOverlay.h"
#include "DemolitionGameMode.generated.h"


/**
 * 
 */
UCLASS()
class CDPROJECT_API ADemolitionGameMode : public ARoundGameMode
{
	GENERATED_BODY()
public:
	ADemolitionGameMode();
	void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
	void SetMatchTime(float c4ExplodeTime);
	void RoundWin(bool isRedTeam);
	void SetSecondHalf();
	void SpawnBot();
	void InitiateBot();
	void KickBot();
	virtual void SetCurMatchState(ECurMatchState NewState, bool IsInit = false) override;

	void SetC4Planted(bool tf);
//
protected:
	virtual void HandleMatchHasStarted() override;
	virtual void RestartMatch(bool isInit = false) override;
	virtual void PlayerEliminated(class AController* VictimController, AController* AttackerController) override;
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController) override;
	
	void InitializeTeamCount();

	FTimerHandle SetupTimer;
	
	TSet<class APlayerStart*> UsedStartPoints;

	UPROPERTY(VisibleAnywhere)
	int32 BotCount=0;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACDCharacter> AIBot;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AAIController> CDAIController;
	UPROPERTY(EditAnywhere)
	UShopOverlay* ShopOverlay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Data")
	UDataTable* WeaponDataTable;

	//C4 Access
	UPROPERTY(VisibleAnywhere)
	class AC4Weapon* _c4Weapon;
	UPROPERTY(VisibleAnywhere)
	class AProjectileC4* _cProjectile;

private:
	UPROPERTY(VisibleAnywhere)
	bool bIsPlanted = false;
	
};
