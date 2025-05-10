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
	
	void UpdateTeamScore(bool bIsTeamA);
	void UpdateIsSecondHalf(bool bIsSecondHalf);

	UFUNCTION()
	void OnRep_TeamAScore();
	
	UFUNCTION()
	void OnRep_TeamBScore();
	
	UFUNCTION()
	void OnRep_IsSecondHalf();
	
	UPROPERTY(ReplicatedUsing=OnRep_TeamAScore, VisibleAnywhere)
	int TeamAScore=0.f;

	UPROPERTY(ReplicatedUsing=OnRep_TeamBScore, VisibleAnywhere)
	int TeamBScore=0.f;

	UPROPERTY(ReplicatedUsing=OnRep_IsSecondHalf, VisibleAnywhere)
	bool IsSecondHalf = false;
public:
	UPROPERTY(VisibleAnywhere)
	TArray<ACDPlayerState*> ATeam;//TArray -> AddUnique, Contains, Remove
	UPROPERTY(VisibleAnywhere)
	TArray<ACDPlayerState*> BTeam;

	UPROPERTY()
	TArray<ACDPlayerState*> AliveATeam;
	UPROPERTY()
	TArray<ACDPlayerState*> AliveBTeam;
	
};
