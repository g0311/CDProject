#include "CDPlayerState.h"

#include "CookOnTheFly.h"
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
	if (Team != ETeam::ET_NoTeam)
		SetTeam(Team);
}

FCDMatchStats ACDPlayerState::GetPRecordInput() const
{
	FCDMatchStats MatchStats;
	MatchStats.Kill = Kills;
	MatchStats.Death = Deaths;
	MatchStats.shot = TotalShot;
	MatchStats.Headshot = HeadShot;
	return MatchStats;
}

ETeam ACDPlayerState::GetPTeam() const
{
	return MatchTeam;
}

FString ACDPlayerState::GetPUsername() const
{
	return Name;
}

void ACDPlayerState::SetPTeam(ETeam team)
{
	MatchTeam = team;
}

void ACDPlayerState::SetPName(const FString& name)
{
	Name = name;
}

void ACDPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACDPlayerState, Team);
	DOREPLIFETIME(ACDPlayerState, MatchTeam);
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

void ACDPlayerState::SetGold(int32 Amount)
{
	Gold = Amount;
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

ETeam ACDPlayerState::GetTeam_Implementation() const
{
	return Team; 
}

void ACDPlayerState::SetTeam(ETeam NewTeam)
{
	Team = NewTeam;
	OnRep_Team();
}

void ACDPlayerState::SetMatchTeam(ETeam NewTeam)
{
	MatchTeam = NewTeam;
}

void ACDPlayerState::SwitchTeam()
{
	if (Team == ETeam::ET_BlueTeam)
	{
		SetTeam(ETeam::ET_RedTeam);
	}
	else if (Team == ETeam::ET_RedTeam)
	{
		SetTeam(ETeam::ET_BlueTeam);
	}
}

void ACDPlayerState::OnRep_Team()
{
	APawn* OwnerPawn = nullptr;
	if (AController* OwnerController = Cast<AController>(GetOwner()))
	{
		OwnerPawn = OwnerController->GetPawn();
	}

	if (ACDCharacter* Character = Cast<ACDCharacter>(OwnerPawn))
	{
		Character->SetTeam(Team);
		UE_LOG(LogTemp, Warning, TEXT("SetTeam Called"));
	}
}


void ACDPlayerState::OnRep_Gold()
{
	//UE_LOG(LogTemp, Display, TEXT("Gold Updated: %d"), Gold);
	OnGoldUpdated.Broadcast(Gold);
	
	//델리게이트 방식으로 리팩토링 필요
	
	if(ACDPlayerController* ACDPC = Cast<ACDPlayerController>(GetPlayerController()))
	{
		ACDPC->SetGold(Gold);
	}
}

void ACDPlayerState::OnRep_Kills()
{
	OnScoreUpdated.Broadcast();
}

void ACDPlayerState::OnRep_Deaths()
{
	OnScoreUpdated.Broadcast();
}
