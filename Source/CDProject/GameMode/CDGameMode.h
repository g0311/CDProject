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
	virtual void PlayerEliminated(class ACDCharacter* ElimmedCharacter,
		class ACDPlayerController* VictimController,
		ACDPlayerController* AttackerController
		);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);
};
