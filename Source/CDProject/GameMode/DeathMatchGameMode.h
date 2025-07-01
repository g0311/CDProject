// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoundGameMode.h"
#include "DeathMatchGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CDPROJECT_API ADeathMatchGameMode : public ARoundGameMode
{
	GENERATED_BODY()
public:
	ADeathMatchGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;

	void SpawnBot();
	void BalancedBot();
	void KickBot();
	void UpdateAlivePlayers();
	// void InitBot(ACDCharacter* BotCharacter);
	virtual void PlayerEliminated(class AController* VictimController, AController* AttackerController) override;
	virtual void SetCurMatchState(ECurMatchState NewState, bool IsInit = false);

protected:
	void InitializeTeamCount();
	
	virtual void HandleMatchHasStarted() override;
	virtual void RestartMatch(bool isInit = false) override;
	//virtual void PlayerEliminated(class ACDPlayerController* VictimController, ACDPlayerController* AttackerController) override;

	int32 BotCount=0;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACDCharacter> AIBot;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AAIController> CDAIController;
	UPROPERTY(EditAnywhere)
	class UShopOverlay* ShopOverlay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Data")
	UDataTable* WeaponDataTable;

};
