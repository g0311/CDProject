// Fill out your copyright notice in the Description page of Project Settings.


#include "CDPlayerState.h"

#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "Net/UnrealNetwork.h"

void ACDPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACDPlayerState, Team);
	DOREPLIFETIME(ACDPlayerState, Gold);
	DOREPLIFETIME(ACDPlayerState, Kills);
	DOREPLIFETIME(ACDPlayerState, Deaths);
	DOREPLIFETIME(ACDPlayerState, Name);
}

ACDPlayerState::ACDPlayerState()
{
	bReplicates=true;
}

void ACDPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	//For Clietn to Set Team
		//If Team Alreadey Set, Rep Func doesn't Call
	SetTeam(Team);
}

void ACDPlayerState::AddGold(int32 Amount)
{
	Gold+=Amount;
	OnRep_Gold();
}

bool ACDPlayerState::SpendGold(int32 Amount)
{
	if (Gold>=Amount)
	{
		Gold-=Amount;
		OnRep_Gold();
		return true;
	}
	return false;
}

void ACDPlayerState::OnRep_Team()
{
	ACDCharacter* BCharacter=Cast<ACDCharacter>(GetPawn());
	if (BCharacter)
	{
		BCharacter->SetTeam(Team);
	}
}

void ACDPlayerState::OnRep_Gold()
{
	UE_LOG(LogTemp, Display, TEXT("Gold = %d"), Gold);
	ACDPlayerController* PC=Cast<ACDPlayerController>(GetOwningController());
	//클라의 컨트롤러가 아닌 PS의 컨트롤러 => On_Rep이 클라에서 호출 되면 항상 PC는 nullptr
	if (PC)
		PC->SetGold();
	return;
}

void ACDPlayerState::SetTeam(ETeam TeamToSet)
{
	//UE_LOG(LogTemp, Warning, TEXT("TeamToSet = %d"), TeamToSet);
	Team=TeamToSet;

	ACDCharacter* BCharacter=Cast<ACDCharacter>(GetPawn());
	if (BCharacter)
	{
		BCharacter->SetTeam(Team);
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Set Team Character NULL"));
		GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this, TeamToSet]()
		{
			if (IsValid(this))
			{
				SetTeam(TeamToSet);
			}
		}));
	}
}



