// Fill out your copyright notice in the Description page of Project Settings.


#include "CDPlayerState.h"

#include "CDProject/Character/CDCharacter.h"
#include "Net/UnrealNetwork.h"

void ACDPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACDPlayerState, Team);
}

void ACDPlayerState::OnRep_Team()
{
	ACDCharacter* BCharacter=Cast<ACDCharacter>(GetPawn());
	if (BCharacter)
	{
		//BCharacter->SetTeamColor(Team);
	}
}

void ACDPlayerState::SetTeam(ETeam TeamToSet)
{
	Team=TeamToSet;
	ACDCharacter* BCharacter=Cast<ACDCharacter>(GetPawn());
	if (BCharacter)
	{
		//BCharacter->SetTeamColor(Team);
	}
}

