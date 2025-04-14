// Fill out your copyright notice in the Description page of Project Settings.


#include "CDGameState.h"

#include "Net/UnrealNetwork.h"

void ACDGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
}

void ACDGameState::OnRep_RedTeamScore()
{
}

void ACDGameState::OnRep_BlueTeamScore()
{
}

void ACDGameState::RedTeamScoreAdd()
{
	RedTeamScore++;
}

void ACDGameState::BlueTeamScoreAdd()
{
	BlueTeamScore++;
}

