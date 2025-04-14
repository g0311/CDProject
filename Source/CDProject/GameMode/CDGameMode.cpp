// Fill out your copyright notice in the Description page of Project Settings.


#include "CDGameMode.h"

#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "CDProject/PlayerState/CDPlayerState.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

namespace MatchState
{
	const FName Cooldown=FName("Cooldown");
}

ACDGameMode::ACDGameMode()
{
	DefaultPawnClass = ACDCharacter::StaticClass();
	bDelayedStart=true;
}

void ACDGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (MatchState==MatchState::WaitingToStart)
	{
		Countdown=FMath::CeilToInt(WarmUpTime+LevelStartingTime-GetWorld()->GetTimeSeconds());
		UE_LOG(LogTemp,Display,TEXT("%f"), Countdown);
		if (Countdown==-1)
		{
			StartMatch();
		}
	}
	else if (MatchState==MatchState::InProgress)
	{
		Countdown=WarmUpTime+MatchTime+LevelStartingTime-GetWorld()->GetTimeSeconds();
		if (Countdown<=0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState==MatchState::Cooldown)
	{
		Countdown= CooldownTime + WarmUpTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (Countdown<=0.f)
		{
			RestartGame();
		}
	}
}

void ACDGameMode::BeginPlay()
{
	Super::BeginPlay();
	//LevelStartingTime=GetWorld()->GetTimeSeconds();
}

void ACDGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator PCIter = GetWorld()->GetPlayerControllerIterator();PCIter;++PCIter)
	{
		ACDPlayerController* PlayerController=Cast<ACDPlayerController> (*PCIter);
		if (PlayerController)
		{
			PlayerController->OnMatchStateSet(MatchState, bTeamsMatch);
		}
	}
}

void ACDGameMode::PlayerEliminated(class ACDCharacter* ElimmedCharacter, class ACDPlayerController* VictimController,
                                   ACDPlayerController* AttackerController)
{
	if (AttackerController==nullptr||AttackerController->PlayerState==nullptr) return;
	if (VictimController==nullptr||VictimController->PlayerState==nullptr) return;
	ACDPlayerState* AttackerPlayerState=AttackerController?Cast<ACDPlayerState>(AttackerController->PlayerState):nullptr;
	ACDPlayerState* VictimPlayerState=VictimController?Cast<ACDPlayerState>(VictimController->PlayerState):nullptr;

	if (ElimmedCharacter)
	{
		//ElimmedCharacter->Elim(); Need
	}
	
}

void ACDGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	//request()
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedController->Destroyed();
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 SelectionPlayerStartingPoint=FMath::RandRange(0,PlayerStarts.Num()-1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[SelectionPlayerStartingPoint]);
	}
	
}




