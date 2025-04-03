// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CDProject/PlayerState/CDPlayerState.h"
#include "GameFramework/GameState.h"
#include "CDGameState.generated.h"

/**
 * 
 */
UCLASS()
class CDPROJECT_API ACDGameState : public AGameState
{
	GENERATED_BODY()

	void RedTeamScore();
	void BlueTeamScore();

	TArray<ACDPlayerState*> RedTeam;
	TArray<ACDPlayerState*> BlueTeam;
	
};
