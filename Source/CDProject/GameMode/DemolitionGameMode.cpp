// Fill out your copyright notice in the Description page of Project Settings.


#include "DemolitionGameMode.h"

#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "CDProject/GameState/CDGameState.h"
#include "CDProject/Weapon/Weapon.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "Runtime/Core/Tests/Containers/TestUtils.h"
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
			if (BGameState->BTeam.Num()>=BGameState->ATeam.Num())
			{
				BGameState->ATeam.AddUnique(BPState);
				BPState->SetMatchTeam(ETeam::ET_ATeam);
				BPState->SetTeam(ETeam::ET_RedTeam);
			}
			else
			{
				BGameState->BTeam.AddUnique(BPState);
				BPState->SetMatchTeam(ETeam::ET_BTeam);
				BPState->SetTeam(ETeam::ET_BlueTeam);
			}
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
		if (BGameState->ATeam.Contains(BPState))
		{
			BGameState->ATeam.Remove(BPState);
		}
		if (BGameState->BTeam.Contains(BPState))
		{
			BGameState->BTeam.Remove(BPState);
		}
		InitializeTeamCount();
	}
}

void ADemolitionGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);
	
	ACDGameState* BGameState=Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		ACDPlayerState* BPState = C->GetPlayerState<ACDPlayerState>();
		if (BPState&&BPState->GetTeam()==ETeam::ET_NoTeam)
		{
			if (BGameState->BTeam.Num()>=BGameState->ATeam.Num())
			{
				BGameState->ATeam.AddUnique(BPState);
				BPState->SetMatchTeam(ETeam::ET_ATeam);
				BPState->SetTeam(ETeam::ET_RedTeam);
			}
			else
			{
				BGameState->BTeam.AddUnique(BPState);
				BPState->SetMatchTeam(ETeam::ET_BTeam);
				BPState->SetTeam(ETeam::ET_BlueTeam);
			}
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

void ADemolitionGameMode::RoundWin(bool isRed)
{
	if (CurRound < MaxRound / 2)
	{
		if (Cast<ACDGameState>(GameState))
			Cast<ACDGameState>(GameState)->UpdateTeamScore(isRed);
	}
	else
	{
		if (Cast<ACDGameState>(GameState))
			Cast<ACDGameState>(GameState)->UpdateTeamScore(!isRed);
	}
	
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
	CurRound++;
}

void ADemolitionGameMode::SetSecondHalf()
{
	if (ACDGameState* ACDGS = GetGameState<ACDGameState>())
	{
		ACDGS->UpdateIsSecondHalf(true);
	}
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (ACDPlayerController* CDPC = Cast<ACDPlayerController>(*It))
		{
			if (ACDPlayerState* CDPS = Cast<ACDPlayerState>(CDPC->PlayerState))
			{
				CDPS->SwitchTeam();
			}
		}
	}
}

void ADemolitionGameMode::SetCurMatchState(ECurMatchState NewState, bool IsInit)
{
	CooldownStartTime = GetWorld()->GetTimeSeconds();
	if (NewState == ECurMatchState::EMS_Waiting && CurRound == MaxRound / 2)
	{
		SetSecondHalf();
	}
	Super::SetCurMatchState(NewState, IsInit);
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
				if (BGameState->BTeam.Num()>=BGameState->ATeam.Num())
				{
					BGameState->ATeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_RedTeam);
				}
				else
				{
					BGameState->BTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_BlueTeam);
				}
			}
		}
		InitializeTeamCount();
	}
}

void ADemolitionGameMode::RestartMatch(bool isInit)
{
	TArray<AController*> PlayerControllers;
	for (FConstPlayerControllerIterator PCIter = GetWorld()->GetPlayerControllerIterator(); PCIter; ++PCIter)
	{
		if (AController* Controller = Cast<AController>(*PCIter))
		{
			PlayerControllers.Add(Controller);
		}
	}
	Test::Shuffle(PlayerControllers);

	for (AController* controller : PlayerControllers)
	{
		if (controller)
		{
			ACDPlayerController* playerController=Cast<ACDPlayerController>(controller);
			ACDCharacter* Character = Cast<ACDCharacter>(controller->GetCharacter());
			if (Character && playerController)
			{
				if (Character->GetTeam() == ETeam::ET_RedTeam)
				{
					Character->GiveC4();
					break;
				}
			}
			if (isInit && controller->GetPlayerState<ACDPlayerState>())
			{
				controller->GetPlayerState<ACDPlayerState>()->SetGold(1000);
			}
		}
	}
	InitializeTeamCount();

	Super::RestartMatch(isInit);
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
			AttackerPlayerState->AddGold(300);
		}
		if (VictimPlayerState->GetTeam() == ETeam::ET_RedTeam)
		{
			if (CurRound < MaxRound / 2)	
				BGameState->AliveATeam.Remove(VictimPlayerState);
			else
				BGameState->AliveBTeam.Remove(VictimPlayerState);
		}
		else if (VictimPlayerState->GetTeam() == ETeam::ET_BlueTeam)
		{
			if (CurRound < MaxRound / 2)
				BGameState->AliveBTeam.Remove(VictimPlayerState);
			else
				BGameState->AliveATeam.Remove(VictimPlayerState);
		}
		
		if (GetCurMatchState() != ECurMatchState::EMS_CoolDown)
		{
			if (BGameState->AliveBTeam.Num()==0)
			{
				if (CurRound < MaxRound / 2)
					RoundWin(true);
				else
					RoundWin(false);
				
				CooldownStartTime = GetWorld()->GetTimeSeconds();
				SetCurMatchState(ECurMatchState::EMS_CoolDown);
			}
			else if (BGameState->AliveATeam.Num()==0)
			{
				if (CurRound < MaxRound / 2)
					RoundWin(false);
				else
					RoundWin(true);
				
				CooldownStartTime = GetWorld()->GetTimeSeconds();
				SetCurMatchState(ECurMatchState::EMS_CoolDown);
			}
		}
	}
}

void ADemolitionGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{ //Unused
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
		BGameState->AliveATeam=BGameState->ATeam;
		BGameState->AliveBTeam=BGameState->BTeam;
	}
}
