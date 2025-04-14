// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamGameMode.h"

#include "CDProject/GameState/CDGameState.h"
#include "Kismet/GameplayStatics.h"

ATeamGameMode::ATeamGameMode()
{
	bTeamsMatch=true;
}

void ATeamGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ACDGameState* BGameState=Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		ACDPlayerState* BPState=NewPlayer->GetPlayerState<ACDPlayerState>();
		if (BPState&&BPState->GetTeam()==ETeam::ET_NoTeam)
		{
			if (BGameState->BlueTeam.Num()>=BGameState->RedTeam.Num())
			{
				BGameState->RedTeam.AddUnique(BPState);
				BPState->SetTeam(ETeam::ET_RedTeam);
			}
			else
			{
				BGameState->BlueTeam.AddUnique(BPState);
				BPState->SetTeam(ETeam::ET_BlueTeam);
			}
		}
	}
}

void ATeamGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	ACDGameState* BGameState=Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	ACDPlayerState* BPState=Exiting->GetPlayerState<ACDPlayerState>();
	if (BGameState&&BPState)
	{
		if (BGameState->RedTeam.Contains(BPState))
		{
			BGameState->RedTeam.Remove(BPState);
		}
		if (BGameState->BlueTeam.Contains(BPState))
		{
			BGameState->BlueTeam.Remove(BPState);
		}
	}
}

void ATeamGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	ACDGameState* BGameState=Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		for (auto PlayerState: BGameState->PlayerArray)//GameState->PlayerArray 가져올 수 있음.
		{
			ACDPlayerState* BPState=Cast<ACDPlayerState>(PlayerState.Get());
			if (BPState&&BPState->GetTeam()==ETeam::ET_NoTeam)
			{
				if (BGameState->BlueTeam.Num()>=BGameState->RedTeam.Num())
				{
					BGameState->RedTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_RedTeam);
				}
				else
				{
					BGameState->BlueTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_BlueTeam);
				}
			}
		}
	}
}
