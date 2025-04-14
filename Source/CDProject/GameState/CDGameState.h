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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_RedTeamScore();
	
	UFUNCTION()
	void OnRep_BlueTeamScore();

	void RedTeamScoreAdd();
	void BlueTeamScoreAdd();

	UPROPERTY(ReplicatedUsing=OnRep_RedTeamScore)
	float RedTeamScore=0.f;

	UPROPERTY(ReplicatedUsing=OnRep_BlueTeamScore)
	float BlueTeamScore=0.f;
public:
	TArray<ACDPlayerState*> RedTeam;//TArray -> AddUnique, Contains, Remove
	TArray<ACDPlayerState*> BlueTeam;
	
};
