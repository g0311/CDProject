// Fill out your copyright notice in the Description page of Project Settings.


#include "DemolitionGameMode.h"

#include "CDProject/Controller/CDPlayerController.h"
#include "CDProject/GameState/CDGameState.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

ADemolitionGameMode::ADemolitionGameMode()
{
	bTeamsMatch=true;
}

void ADemolitionGameMode::PostLogin(APlayerController* NewPlayer)
{
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
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACDPlayerController* CDPC = Cast<ACDPlayerController>(*It);
		if (CDPC)
		{
			FTimerDelegate TimerDel;
			TimerDel.BindUObject(CDPC, &ACDPlayerController::SetKDOverlayUI); 

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle,
				TimerDel,
				0.1f,
				false
			);
		}
	}
	Super::PostLogin(NewPlayer);
}

void ADemolitionGameMode::Logout(AController* Exiting)
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
		InitializeTeamCount();
	}
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACDPlayerController* CDPC = Cast<ACDPlayerController>(*It);
		if (CDPC)
		{
			CDPC->SetKDOverlayUI(); 
		}
	}
}

void ADemolitionGameMode::SetMatchTime(float c4ExplodeTime)
{
	MatchTime = c4ExplodeTime - WarmUpTime - WaitingStartTime + GetWorld()->GetTimeSeconds();
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACDPlayerController* PC = Cast<ACDPlayerController>(It->Get());
		if (PC)
		{
			PC->ClientSetMatchTime(MatchTime);
		}
	}
}

void ADemolitionGameMode::TeamWin(bool isRed)
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACDPlayerController* CDPC = Cast<ACDPlayerController>(*It);
		if (CDPC)
		{
			ACDPlayerState* playerState = Cast<ACDPlayerState>(CDPC->PlayerState);
			if (playerState)
			{
				if (playerState->GetTeam() == ETeam::ET_RedTeam && isRed)
				{
					playerState->AddGold(300);
					UE_LOG(LogTemp, Display, TEXT("Red Team Win"));
				}
				else if (playerState->GetTeam() == ETeam::ET_BlueTeam && !isRed)
				{
					playerState->AddGold(300);
					UE_LOG(LogTemp, Display, TEXT("Blue Team Win"));
				}
			}
		}
	}
}

void ADemolitionGameMode::SetCurMatchState(ECurMatchState NewState)
{
	if (!_isPlanted && NewState == ECurMatchState::EMS_CoolDown)
	{
		CooldownStartTime = GetWorld()->GetTimeSeconds();
		TeamWin(false);
	}
	Super::SetCurMatchState(NewState);
}

void ADemolitionGameMode::HandleMatchHasStarted()
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
		InitializeTeamCount();
	}
}

void ADemolitionGameMode::PlayerEliminated(class ACDPlayerController* VictimController,
	ACDPlayerController* AttackerController)
{
	Super::PlayerEliminated(VictimController, AttackerController);
	ACDGameState* BGameState=Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	ACDPlayerState* AttackerPlayerState=AttackerController?Cast<ACDPlayerState>(AttackerController->PlayerState):nullptr;
	ACDPlayerState* VictimPlayerState=VictimController?Cast<ACDPlayerState>(VictimController->PlayerState):nullptr;
	
	if (BGameState && VictimPlayerState)
	{
		if (AttackerPlayerState)
		{
			AttackerPlayerState->AddGold(200);
		}
		if (VictimPlayerState->GetTeam() == ETeam::ET_RedTeam)
		{
			BGameState->AliveRedTeam.Remove(VictimPlayerState);
		}
		else if (VictimPlayerState->GetTeam() == ETeam::ET_BlueTeam)
		{
			BGameState->AliveBlueTeam.Remove(VictimPlayerState);
		}
		BGameState->CheckTeamElimination();
		if (BGameState->AliveBlueTeam.Num()==0||BGameState->AliveRedTeam.Num()==0)
		{
			CooldownStartTime = GetWorld()->GetTimeSeconds();
			SetCurMatchState(ECurMatchState::EMS_CoolDown);
		}
	}

}

void ADemolitionGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy(); 
	}
	
	ACDPlayerState* PS = ElimmedController->GetPlayerState<ACDPlayerState>();
	FName TeamTag= PS->GetTeam() == ETeam::ET_RedTeam ? FName("Red") : FName("Blue");

	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
	for (AActor* Start : PlayerStarts)
	{
		APlayerStart* StartPoint = Cast<APlayerStart>(Start);
		if (StartPoint && StartPoint->PlayerStartTag == TeamTag)
		{
			RestartPlayerAtPlayerStart(ElimmedController, StartPoint);
			return;
		}
	}
}

void ADemolitionGameMode::InitializeTeamCount()
{
	ACDGameState* BGameState=Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		BGameState->AliveRedTeam=BGameState->RedTeam;
		BGameState->AliveBlueTeam=BGameState->BlueTeam;
	}
	
}
