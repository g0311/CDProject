// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameModeBase.h"
#include "CDGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CDPROJECT_API ACDGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ACDGameMode();
	virtual void PlayerEliminated(class ACDCharacter* ElimmedCharacter,
		class ACDPlayerController* VictimController,
		ACDPlayerController* AttackerController
		);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);

	//InGame Variable
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	UPROPERTY(EditDefaultsOnly)
	float WarmUpTime=10.f;

	float LevelStartingTime=0.f;
	
protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;
};
