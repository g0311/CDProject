// Fill out your copyright notice in the Description page of Project Settings.


#include "CDGameState.h"

#include "CDProject/Controller/CDPlayerController.h"
#include "Net/UnrealNetwork.h"

void ACDGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACDGameState, RedTeamScore);
	DOREPLIFETIME(ACDGameState, BlueTeamScore);
	
}

void ACDGameState::OnRep_RedTeamScore()
{
	ACDPlayerController* PC=Cast<ACDPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		PC->SetHUDRedTeam(RedTeamScore);
	}
}

void ACDGameState::OnRep_BlueTeamScore()
{
	ACDPlayerController* PC=Cast<ACDPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		PC->SetHUDRedTeam(BlueTeamScore);
	}
}

void ACDGameState::RedTeamScoreAdd()
{
	RedTeamScore++;
	ACDPlayerController* PC=Cast<ACDPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		PC->SetHUDRedTeam(RedTeamScore);
	}
}

void ACDGameState::BlueTeamScoreAdd()
{
	BlueTeamScore++;
	ACDPlayerController* PC=Cast<ACDPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		PC->SetHUDRedTeam(BlueTeamScore);
	}
}

