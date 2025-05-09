#include "CDGameState.h"
#include "Net/UnrealNetwork.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "CDProject/GameMode/RoundGameMode.h"
#include "GameFramework/GameMode.h"

void ACDGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACDGameState, TeamAScore);
	DOREPLIFETIME(ACDGameState, TeamBScore);
	DOREPLIFETIME(ACDGameState, IsSecondHalf);
}

void ACDGameState::UpdateTeamScore(bool bIsTeamA)
{
	if (bIsTeamA)
	{
		TeamAScore++;
		OnRep_TeamAScore();
	}
	else
	{
		TeamBScore++;
		OnRep_TeamBScore();
	}
}

void ACDGameState::UpdateIsSecondHalf(bool bIsSecondHalf)
{
	IsSecondHalf = bIsSecondHalf;
	OnRep_IsSecondHalf();
}

void ACDGameState::OnRep_TeamAScore()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACDPlayerController* PC = Cast<ACDPlayerController>(It->Get());
		if (PC)
		{
			PC->SetHUDATeam(TeamAScore);
		}
	}
}

void ACDGameState::OnRep_TeamBScore()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACDPlayerController* PC = Cast<ACDPlayerController>(It->Get());
		if (PC)
		{
			PC->SetHUDBTeam(TeamBScore);
		}
	}
}

void ACDGameState::OnRep_IsSecondHalf()
{
	if (IsSecondHalf)
		UE_LOG(LogGameMode, Warning, TEXT("IS SECOND REP"));
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACDPlayerController* PC = Cast<ACDPlayerController>(It->Get());
		if (PC)
		{
			PC->SetTeamUIColor();
		}
	}
}
