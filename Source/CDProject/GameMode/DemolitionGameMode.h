// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoundGameMode.h"
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
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	void SetMatchTime(float c4ExplodeTime);
	void TeamWin(bool isRed);
	virtual void SetMatchState(FName NewState) override;

	FORCEINLINE void SetIsPlanted(bool tf) {_isPlanted = tf;}
protected:
	virtual void HandleMatchHasStarted() override;
	virtual void PlayerEliminated(class ACDPlayerController* VictimController, ACDPlayerController* AttackerController) override;
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController) override;
	
	void InitializeTeamCount();

	FTimerHandle SetupTimer;
	
	TSet<APlayerStart*> UsedStartPoints;

	//임시
	bool _isPlanted = false;

	//C4 Access
	UPROPERTY(VisibleAnywhere)
	class AC4Weapon* _c4Weapon;
	UPROPERTY(VisibleAnywhere)
	class AProjectileC4* _cProjectile;
};
