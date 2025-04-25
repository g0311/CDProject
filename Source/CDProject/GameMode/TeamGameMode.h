// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CDGameMode.h"
#include "TeamGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CDPROJECT_API ATeamGameMode : public ACDGameMode
{
	GENERATED_BODY()
public:
	ATeamGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
protected:
	virtual void HandleMatchHasStarted() override;
	virtual void PlayerEliminated(class ACDCharacter* ElimmedCharacter, class ACDPlayerController* VictimController, ACDPlayerController* AttackerController) override;
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController) override;
	
	void InitializeTeamCount();

	TSet<APlayerStart*> UsedStartPoints;
};
