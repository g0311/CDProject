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
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void UpdateTeamScore(bool bIsRedTeam);
	void CheckTeamElimination();

	UFUNCTION()
	void OnRep_RedTeamScore();
	
	UFUNCTION()
	void OnRep_BlueTeamScore();
	

	UPROPERTY(ReplicatedUsing=OnRep_RedTeamScore)
	float RedTeamScore=0.f;

	UPROPERTY(ReplicatedUsing=OnRep_BlueTeamScore)
	float BlueTeamScore=0.f;

	

public:
	TArray<ACDPlayerState*> RedTeam;//TArray -> AddUnique, Contains, Remove
	TArray<ACDPlayerState*> BlueTeam;

	TArray<ACDPlayerState*> AliveRedTeam;
	TArray<ACDPlayerState*> AliveBlueTeam;
	
};
