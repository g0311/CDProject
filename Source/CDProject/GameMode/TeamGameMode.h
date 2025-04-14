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
};
