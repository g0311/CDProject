// Fill out your copyright notice in the Description page of Project Settings.


#include "DemolitionGameMode.h"

#include "AIController.h"
#include "aws/gamelift/server/model/Player.h"
#include "CDProject/Character/CDAIEnemy.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "CDProject/GameState/CDGameState.h"
#include "CDProject/HUD/CDHUD.h"
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
	if (BotCount>0){KickBot();}
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
	//Server Call
	// for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	// {
	// 	ACDPlayerController* CDPC = Cast<ACDPlayerController>(*It);
	// 	if (CDPC)
	// 	{
	// 		FTimerDelegate TimerDel;
	// 		TimerDel.BindUObject(CDPC, &ACDPlayerController::SetKDOverlayUI); 
	//
	// 		FTimerHandle TimerHandle;
	// 		GetWorld()->GetTimerManager().SetTimer(
	// 			TimerHandle,
	// 			TimerDel,
	// 			0.1f,
	// 			false
	// 		);
	// 	}
	// }
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
	
	// for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	// {
	// 	ACDPlayerController* CDPC = Cast<ACDPlayerController>(*It);
	// 	if (CDPC)
	// 	{
	// 		CDPC->SetKDOverlayUI(); 
	// 	}
	// }
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

void ADemolitionGameMode::BalancedBot()
{
	int FullCount=10;
	ACDGameState* BGameState=Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	int CurrentPlayers=BGameState->ATeam.Num()+BGameState->BTeam.Num();
	int BotsSpawnCount=FullCount-CurrentPlayers;
	UE_LOG(LogTemp, Warning, TEXT("%d,%d"), CurrentPlayers, BotsSpawnCount);
	if (BGameState)
	{
		for (int i=0;i<BotsSpawnCount; i++) SpawnBot();
	}
}

void ADemolitionGameMode::KickBot()
{
	BotCount--;
	ACDGameState* BGameState=Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	
	TArray<AActor*> BotEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACDCharacter::StaticClass(), BotEnemies);
	
	for (AActor* Bot:BotEnemies)
	{
		AController* BotController = Bot->GetInstigatorController();
		ACDPlayerState* BotState = BotController->GetPlayerState<ACDPlayerState>();
		if (Bot->ActorHasTag("Bot"))
		{
			if (BGameState->ATeam.Contains(BotState))
			{
				BGameState->ATeam.Remove(BotState);
				InitializeTeamCount();
				Bot->Destroy();
				return;
			}
			else if (BGameState->BTeam.Contains(BotState))
			{
				BGameState->BTeam.Remove(BotState);
				InitializeTeamCount();
				Bot->Destroy();
				return;
			}
		}
	} 
}

void ADemolitionGameMode::SpawnBot()
{
	BotCount++;
	if (!AIBot) return; 
	
	FVector SpawnLocation = FVector::ZeroVector; 
	FRotator SpawnRotation = FRotator::ZeroRotator;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AAIController* AIController = GetWorld()->SpawnActor<AAIController>(CDAIController, SpawnLocation, SpawnRotation, SpawnParams);
	if (AIController)
	{
		ACDCharacter* Enemy = GetWorld()->SpawnActor<ACDCharacter>(AIBot, SpawnLocation, SpawnRotation, SpawnParams);
		if (Enemy)
		{
			AIController->Possess(Enemy);
			ACDPlayerState* BotPlayerState = Enemy->GetPlayerState<ACDPlayerState>();
			if (BotPlayerState)
			{
				AIController->PlayerState = BotPlayerState;
				BotPlayerState->SetOwner(AIController);
				
				Enemy->Tags.Add(FName("Bot"));
				InitBot(Enemy);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("ADemolitionGameMode::SpawnBot: Failed to spawn PlayerState for bot."));
				Enemy->Destroy(); 
				AIController->Destroy(); 
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ADemolitionGameMode::SpawnBot: Failed to spawn bot character (Enemy)."));
			AIController->Destroy(); 
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ADemolitionGameMode::SpawnBot: Failed to spawn AIController for bot."));
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

void ADemolitionGameMode::InitBot(ACDCharacter* BotCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("InitBot!"));
	ACDGameState* BGameState = Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	ACDPlayerState* BotPlayerState = BotCharacter->GetPlayerState<ACDPlayerState>();

	if (BGameState && BotPlayerState)
	{
		if (BGameState->BTeam.Num() >= BGameState->ATeam.Num())
		{
			BGameState->ATeam.AddUnique(BotPlayerState);
			BotPlayerState->SetMatchTeam(ETeam::ET_ATeam);
			BotPlayerState->SetTeam(ETeam::ET_RedTeam);
		}
		else
		{
			BGameState->BTeam.AddUnique(BotPlayerState);
			BotPlayerState->SetMatchTeam(ETeam::ET_BTeam);
			BotPlayerState->SetTeam(ETeam::ET_BlueTeam);
		}
	}
}

void ADemolitionGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	ACDGameState* BGameState=Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	BalancedBot();
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
	//BalancedBot();//2번 반복?
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
