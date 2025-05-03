// Fill out your copyright notice in the Description page of Project Settings.


#include "RoundGameMode.h"

#include "EngineUtils.h"
#include "DemolitionGameMode.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "CDProject/HUD/CDHUD.h"
#include "CDProject/PlayerState/CDPlayerState.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

namespace MatchState
{
	const FName PreInProgress=FName("PreProgress");
	const FName Cooldown=FName("Cooldown");
	const FName ModeSelect=FName("ModeSelect");
}

ARoundGameMode::ARoundGameMode()
{
	DefaultPawnClass = ACDCharacter::StaticClass();
	bDelayedStart=true;
}

void ARoundGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (MatchState==MatchState::WaitingToStart || MatchState==MatchState::PreInProgress)
	{
		Countdown=FMath::CeilToInt(WaitingStartTime + WarmUpTime-GetWorld()->GetTimeSeconds());
		if (Countdown==-1)
		{
			StartMatch();
		}
	}
	else if (MatchState==MatchState::InProgress)
	{
		Countdown=MatchStartTime + MatchTime-GetWorld()->GetTimeSeconds();
		if (Countdown<=0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState==MatchState::Cooldown)
	{
		Countdown=CooldownStartTime + CooldownTime-GetWorld()->GetTimeSeconds();
		//UE_LOG(LogGameMode, Log, TEXT("%f %f %f"), CooldownTime, CooldownStartTime, GetWorld()->GetTimeSeconds());
		if (Countdown<=0.f)
		{
			UE_LOG(LogGameMode, Log, TEXT("Restart Called"));
			RestartGame();
		}
	}
}

void ARoundGameMode::BeginPlay()
{
	Super::BeginPlay();
	//LevelStartingTime=GetWorld()->GetTimeSeconds();
	
}

void ARoundGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();
	
	for (FConstPlayerControllerIterator PCIter = GetWorld()->GetPlayerControllerIterator();PCIter;++PCIter)
	{
		ACDPlayerController* PlayerController=Cast<ACDPlayerController> (*PCIter);
		if (PlayerController)
		{
			if(MatchState==MatchState::WaitingToStart)
			{
				PlayerController->OnMatchStateSet(MatchState, bTeamsMatch, WaitingStartTime);
			}
			else if(MatchState==MatchState::InProgress)
			{
				PlayerController->OnMatchStateSet(MatchState, bTeamsMatch, MatchStartTime);			
			}
			else if (MatchState==MatchState::Cooldown)
			{
				PlayerController->OnMatchStateSet(MatchState, bTeamsMatch, CooldownStartTime);
			}
		}
	}
}

void ARoundGameMode::PlayerEliminated(class ACDPlayerController* VictimController,
                                   ACDPlayerController* AttackerController)
{
	if (AttackerController==nullptr||AttackerController->PlayerState==nullptr) return;
	if (VictimController==nullptr||VictimController->PlayerState==nullptr) return;
	ACDPlayerState* AttackerPlayerState=AttackerController?Cast<ACDPlayerState>(AttackerController->PlayerState):nullptr;
	ACDPlayerState* VictimPlayerState=VictimController?Cast<ACDPlayerState>(VictimController->PlayerState):nullptr;
	
	if (AttackerPlayerState)
	{
		AttackerPlayerState->AddKill();
		AttackerPlayerState->AddGold(200);
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddDeath();
	}
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACDPlayerController* CDPC = Cast<ACDPlayerController>(*It);
		if (CDPC)
		{//Need to Set Client RPC
			CDPC->UpdateKDOverlayData();
		}
	}
}

void ARoundGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{//Unused
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 SelectionPlayerStartingPoint=FMath::RandRange(0,PlayerStarts.Num()-1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[SelectionPlayerStartingPoint]);
	}
}

void ARoundGameMode::RestartGame()
{
	for (auto actor : _createdActors)
	{
		if (IsValid(actor))
			actor->Destroy();
	}
	
	for (FConstPlayerControllerIterator PCIter = GetWorld()->GetPlayerControllerIterator();PCIter;++PCIter)
	{
		AController* controller = Cast<AController>(*PCIter);
		if (controller)
		{
			//AActor* startSpot = GetSpawnPoint(controller);
			ACDCharacter* Character = Cast<ACDCharacter>((*PCIter)->GetCharacter());
			if (Character)
			{
				Character->Reset();
				AActor* playerStart = FindPlayerStart(controller);
				if (playerStart)
				{
					Character->SetActorLocation(playerStart->GetActorLocation());
					Character->SetActorRotation(playerStart->GetActorRotation());
				}
			}
		}
	}
	WaitingStartTime = GetWorld()->GetTimeSeconds();
	MatchTime = defaultMatchTime;
	SetMatchState(MatchState::PreInProgress);
}

AActor* ARoundGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	FString TeamTag = TEXT("None");
	ACDPlayerState* playerState = Cast<ACDPlayerState>(Player->PlayerState);
	if (playerState)
	{
		if (playerState->GetTeam() == ETeam::ET_RedTeam)
		{
			TeamTag = TEXT("RED");
		}
		else if (playerState->GetTeam() == ETeam::ET_BlueTeam)
		{
			TeamTag = TEXT("BLUE");
		}
	}

	UE_LOG(LogGameMode, Log, TEXT("%s"), *TeamTag);
	// StartPoint init
	if (!AvailStartPoints.Contains(TeamTag))
	{
		TArray<AActor*> AllStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), AllStarts);

		TArray<APlayerStart*> TeamStarts;
		for (AActor* Actor : AllStarts)
		{
			APlayerStart* teamStart = Cast<APlayerStart>(Actor);
			if (teamStart && teamStart->PlayerStartTag == FName(*TeamTag))
			{
				TeamStarts.Add(Cast<APlayerStart>(teamStart));
			}
		}
		AvailStartPoints.Add(TeamTag, TeamStarts);
	}

	TArray<APlayerStart*>& Avail = AvailStartPoints[TeamTag];
	if (Avail.Num() > 0)
	{
		const int32 Index = FMath::RandRange(0, Avail.Num() - 1);
		APlayerStart* ChosenStart = Avail[Index];
		return ChosenStart;
	}

	return Super::FindPlayerStart_Implementation(Player, IncomingName);
}

// bool ARoundGameMode::ShouldSpawnAtStartSpot(AController* Player)
// {
// 	return false;
// 	//return Super::ShouldSpawnAtStartSpot(Player);
// }

void ARoundGameMode::SetMatchState(FName NewState)
{
	if (NewState == MatchState::WaitingToStart)
	{
		WaitingStartTime = GetWorld()->GetTimeSeconds();
	}
	else if (NewState == MatchState::InProgress)
	{
		MatchStartTime = GetWorld()->GetTimeSeconds();
	}
	else if (NewState == MatchState::Cooldown)
	{
		CooldownStartTime = GetWorld()->GetTimeSeconds();
	}
	Super::SetMatchState(NewState);
}