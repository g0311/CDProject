#include "CDGameState.h"
#include "Net/UnrealNetwork.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "CDProject/GameMode/RoundGameMode.h"
#include "GameFramework/GameMode.h"

void ACDGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACDGameState, RedTeamScore);
	DOREPLIFETIME(ACDGameState, BlueTeamScore);
}

void ACDGameState::UpdateTeamScore(bool bIsRedTeam)
{
	if (bIsRedTeam)
	{
		RedTeamScore++;
		OnRep_RedTeamScore();
	}
	else
	{
		BlueTeamScore++;
		OnRep_BlueTeamScore();
	}
}

void ACDGameState::CheckTeamElimination()
{
	bool bRedTeamEliminated = AliveRedTeam.Num() == 0;
	bool bBlueTeamEliminated = AliveBlueTeam.Num() == 0;
	bool bScoreUpdated = false;

	if (bRedTeamEliminated)
	{
		UpdateTeamScore(false);
		bScoreUpdated = true;
	}

	if (bBlueTeamEliminated)
	{
		UpdateTeamScore(true);
		bScoreUpdated = true;
	}
	// if (bScoreUpdated)
	// {
	// 	ARoundGameMode* GameMode = Cast<ARoundGameMode>(GetWorld()->GetAuthGameMode());
	// 	if (GameMode)
	// 	{
	// 		GameMode->SetCurMatchState(ECurMatchState::EMS_CoolDown);
	// 	}
	// }
}

void ACDGameState::OnRep_RedTeamScore()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACDPlayerController* PC = Cast<ACDPlayerController>(It->Get());
		if (PC)
		{
			PC->SetHUDRedTeam(RedTeamScore);
		}
	}
}

void ACDGameState::OnRep_BlueTeamScore()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACDPlayerController* PC = Cast<ACDPlayerController>(It->Get());
		if (PC)
		{
			PC->SetHUDBlueTeam(BlueTeamScore);
		}
	}
}
