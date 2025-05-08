#include "CDPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"

ACDPlayerState::ACDPlayerState()
{
	bReplicates = true;
}

void ACDPlayerState::BeginPlay()
{
	Super::BeginPlay();
	SetTeam(Team);
}

void ACDPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACDPlayerState, Team);
	DOREPLIFETIME(ACDPlayerState, Gold);
	DOREPLIFETIME(ACDPlayerState, Kills);
	DOREPLIFETIME(ACDPlayerState, Deaths);
	DOREPLIFETIME(ACDPlayerState, Name);
}

void ACDPlayerState::AddKill()
{
	Kills++;
	OnRep_Kills();
}

void ACDPlayerState::AddDeath()
{
	Deaths++;
	OnRep_Deaths();
}

void ACDPlayerState::AddGold(int32 Amount)
{
	Gold += Amount;
	OnRep_Gold();
}

bool ACDPlayerState::SpendGold(int32 Amount)
{
	if (Gold >= Amount)
	{
		Gold -= Amount;
		OnRep_Gold();
		return true;
	}
	return false;
}

void ACDPlayerState::SetTeam(ETeam NewTeam)
{
	Team = NewTeam;
	OnRep_Team();
}

void ACDPlayerState::OnRep_Team()
{
	ACDCharacter* Character = Cast<ACDCharacter>(GetPawn());
	if (Character)
	{
		Character->SetTeam(Team);
	}
}

void ACDPlayerState::OnRep_Gold()
{
	//UE_LOG(LogTemp, Display, TEXT("Gold Updated: %d"), Gold);
	OnGoldUpdated.Broadcast(Gold);
}

void ACDPlayerState::OnRep_Kills()
{
	OnScoreUpdated.Broadcast();
}

void ACDPlayerState::OnRep_Deaths()
{
	OnScoreUpdated.Broadcast();
}
