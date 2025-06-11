// Fill out your copyright notice in the Description page of Project Settings.


#include "DemolitionGameMode.h"

#include "AIController.h"
#include "EngineUtils.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Component/CombatComponent.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "CDProject/GameState/CDGameState.h"
#include "CDProject/Weapon/Weapon.h"
#include "CDProject/Widget/ShopOverlay.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "Engine/DataTable.h"
#include "Runtime/Core/Tests/Containers/TestUtils.h"
#include "Kismet/GameplayStatics.h"

ADemolitionGameMode::ADemolitionGameMode()
{
	bTeamsMatch=true;
}

void ADemolitionGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	ACDGameState* BGameState=Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	if ( BotCount > MAX_PLAYER - _joinedClinetCount ){KickBot();}
	if (BGameState)
	{
		ACDPlayerState* BPState=NewPlayer->GetPlayerState<ACDPlayerState>();
		if (BPState&&BPState->GetTeam()==ETeam::ET_NoTeam)
		{
			if (BGameState->BTeam.Num()>=BGameState->ATeam.Num())
			{
				BGameState->ATeam.AddUnique(BPState);
				BPState->SetMatchTeam(ETeam::ET_ATeam);
				if (!BGameState->IsSecondHalf)
					BPState->SetTeam(ETeam::ET_RedTeam);
				else
					BPState->SetTeam(ETeam::ET_BlueTeam);
			}
			else
			{
				BGameState->BTeam.AddUnique(BPState);
				BPState->SetMatchTeam(ETeam::ET_BTeam);
				if (!BGameState->IsSecondHalf)
					BPState->SetTeam(ETeam::ET_BlueTeam);
				else
					BPState->SetTeam(ETeam::ET_RedTeam);
			}
		}
	}
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
	if ( BotCount > MAX_PLAYER - _joinedClinetCount ){KickBot();}
	if (BGameState)
	{
		ACDPlayerState* BPState=C->GetPlayerState<ACDPlayerState>();
		if (BPState&&BPState->GetTeam()==ETeam::ET_NoTeam)
		{
			if (BGameState->BTeam.Num()>=BGameState->ATeam.Num())
			{
				BGameState->ATeam.AddUnique(BPState);
				BPState->SetMatchTeam(ETeam::ET_ATeam);
				if (!BGameState->IsSecondHalf)
					BPState->SetTeam(ETeam::ET_RedTeam);
				else
					BPState->SetTeam(ETeam::ET_BlueTeam);
			}
			else
			{
				BGameState->BTeam.AddUnique(BPState);
				BPState->SetMatchTeam(ETeam::ET_BTeam);
				if (!BGameState->IsSecondHalf)
					BPState->SetTeam(ETeam::ET_BlueTeam);
				else
					BPState->SetTeam(ETeam::ET_RedTeam);
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

void ADemolitionGameMode::RoundWin(bool isRedTeam)
{
	if (CurRound < MaxRound / 2)
	{
		if (Cast<ACDGameState>(GameState))
			Cast<ACDGameState>(GameState)->UpdateTeamScore(isRedTeam);
	}
	else
	{
		if (Cast<ACDGameState>(GameState))
			Cast<ACDGameState>(GameState)->UpdateTeamScore(!isRedTeam);
	}

	for (TActorIterator<AController> It(GetWorld()); It; ++It)
	{
		AController* PC = *It;
		if (PC)
		{
			ACDPlayerState* playerState = Cast<ACDPlayerState>(PC->PlayerState);
			if (playerState)
			{
				if (isRedTeam)
				{
					if (playerState->GetTeam() == ETeam::ET_RedTeam && isRedTeam)
						playerState->AddGold(800);
					else if (playerState->GetTeam() == ETeam::ET_BlueTeam && !isRedTeam)
						playerState->AddGold(400);
					UE_LOG(LogTemp, Display, TEXT("Red Team Win"));
				}
				else
				{
					if (playerState->GetTeam() == ETeam::ET_RedTeam && isRedTeam)
						playerState->AddGold(800);
					else if (playerState->GetTeam() == ETeam::ET_BlueTeam && !isRedTeam)
						playerState->AddGold(400);
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
	
	for (TActorIterator<AController> It(GetWorld()); It; ++It)
	{
		if (ACDPlayerState* CDPS = Cast<ACDPlayerState>((*It)->PlayerState))
		{
			CDPS->SwitchTeam();
		}
	}
}

void ADemolitionGameMode::InitiateBot()
{
	ACDGameState* BGameState=Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	
	int FullCount=MAX_PLAYER;
	int CurrentPlayers=BGameState->GetPlayerInfos().Items.Num();
	int BotsSpawnCount=FullCount-CurrentPlayers;
	UE_LOG(LogTemp, Warning, TEXT("Current Players -> %d, BotsSpawnCount -> %d"), CurrentPlayers, BotsSpawnCount);
	if (BGameState)
	{
		for (int i=0;i<BotsSpawnCount; i++)
			SpawnBot();
	}
}


void ADemolitionGameMode::KickBot()
{
	BotCount--;
	ACDGameState* BGameState=Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	
	TArray<AActor*> BotEnemies;
	for (TObjectIterator<ACDCharacter> It; It; ++It)
	{
		if (ACDCharacter* Character = *It; IsValid(Character))
		{
			AAIController* BotController = Cast<AAIController>(Character->GetInstigatorController());
			if (!IsValid(BotController))
				continue;
			
			ACDPlayerState* BotState = BotController->GetPlayerState<ACDPlayerState>();
			if (Character->ActorHasTag("Bot"))
			{
				if (BGameState->ATeam.Contains(BotState))
				{
					BGameState->ATeam.Remove(BotState);
				}
				else if (BGameState->BTeam.Contains(BotState))
				{
					BGameState->BTeam.Remove(BotState);
				}
				InitializeTeamCount();
				Character->DestroyAllWeapon();
				Character->Destroy();
				BotState->Destroy();
				return;
			}
		}
	}
	
	// UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACDCharacter::StaticClass(), BotEnemies);
	//
	// for (AActor* Bot:BotEnemies)
	// {
	// 	AController* BotController = Bot->GetInstigatorController();
	// 	ACDPlayerState* BotState = BotController->GetPlayerState<ACDPlayerState>();
	// 	if (Bot->ActorHasTag("Bot"))
	// 	{
	// 		if (BGameState->ATeam.Contains(BotState))
	// 		{
	// 			BGameState->ATeam.Remove(BotState);
	// 		}
	// 		else if (BGameState->BTeam.Contains(BotState))
	// 		{
	// 			BGameState->BTeam.Remove(BotState);
	// 		}
	// 	}
	// 	InitializeTeamCount();
	// 	Bot->Destroy();
	// 	BotState->Destroy();
	// 	return;
	// }
}

void ADemolitionGameMode::SpawnBot()
{
	BotCount++;
	if (!AIBot) return; 

	ETeam BotAssignedTeam;
	FString BotTeamTag;

	ACDGameState* BGameState = Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	//ACDPlayerState* BotPlayerState = BotCharacter->GetPlayerState<ACDPlayerState>();
    //FindPlayerStart	
	if (BGameState->BTeam.Num() >= BGameState->ATeam.Num())
	{
		BotAssignedTeam = ETeam::ET_RedTeam;
		BotTeamTag = TEXT("RED");
	}
	else
	{
		BotAssignedTeam = ETeam::ET_BlueTeam;
		BotTeamTag = TEXT("BLUE");
	}

	if (!AvailStartPoints.Contains(BotTeamTag))
	{
		TArray<AActor*> AllStarts;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), AllStarts);

		TArray<APlayerStart*> TeamStarts;
		for (AActor* Actor : AllStarts)
		{
			APlayerStart* TeamStart = Cast<APlayerStart>(Actor);
			if (TeamStart && TeamStart->PlayerStartTag == FName(*BotTeamTag))
			{
				TeamStarts.Add(TeamStart);
			}
		}
		AvailStartPoints.Add(BotTeamTag, TeamStarts);
	}
	FVector SpawnLocation = FVector::ZeroVector;
    FRotator SpawnRotation = FRotator::ZeroRotator;
    if (AvailStartPoints.Contains(BotTeamTag))
    {
        TArray<APlayerStart*>& TeamAvailStarts = AvailStartPoints[BotTeamTag];
        if (TeamAvailStarts.Num() > 0)
        {
            const int32 Index = FMath::RandRange(0, TeamAvailStarts.Num() - 1);
            APlayerStart* ChosenStart = TeamAvailStarts[Index];
            SpawnLocation = ChosenStart->GetActorLocation();
            SpawnRotation = ChosenStart->GetActorRotation();
        }
    }
	
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; 
	//If Collision -> possible none Collision place spawn
    ACDAIController* AIController = GetWorld()->SpawnActor<ACDAIController>(CDAIController, SpawnLocation, SpawnRotation, SpawnParams);
    if (AIController)
    {
	    ACDPlayerState* BotPlayerState = Cast<ACDPlayerState>(AIController->PlayerState);
    	if (BotPlayerState)
    	{
    		ACDCharacter* EnemyBot = GetWorld()->SpawnActor<ACDCharacter>(AIBot, SpawnLocation, SpawnRotation, SpawnParams);
    		if (EnemyBot)//PC->PS->Pawn->(Possess PC->Character)
    		{
    			AIController->SetPawn(EnemyBot);
    			AIController->PlayerState = BotPlayerState; 
    			BotPlayerState->SetOwner(AIController);
    			BotPlayerState->SetUsername(TEXT("Bot") + FString::FromInt(rand()));
    			EnemyBot->SetUserName(BotPlayerState->GetUsername());
    			EnemyBot->SetPlayerState(BotPlayerState);
    			EnemyBot->Tags.Add(FName("Bot"));
            	
    			if (BotAssignedTeam == ETeam::ET_RedTeam)
    			{
    				BotPlayerState->SetMatchTeam(ETeam::ET_ATeam);
    				if (!BGameState->IsSecondHalf)
    					BotPlayerState->SetTeam(ETeam::ET_RedTeam);
    				else
    					BotPlayerState->SetTeam(ETeam::ET_BlueTeam);
    				BGameState->ATeam.AddUnique(BotPlayerState);
    			}
    			else // ET_BlueTeam
    			{
    				BotPlayerState->SetMatchTeam(ETeam::ET_BTeam); 
    				if (!BGameState->IsSecondHalf)
    					BotPlayerState->SetTeam(ETeam::ET_BlueTeam);
    				else
    					BotPlayerState->SetTeam(ETeam::ET_RedTeam);
    				BGameState->BTeam.AddUnique(BotPlayerState);
    			}
    			UE_LOG(LogTemp, Log, TEXT("BotTeamTag -> %s"), *BotTeamTag);
            	
    			AIController->Possess(EnemyBot);
    		}
    		else
    		{
    			UE_LOG(LogTemp, Error, TEXT("Failed Controller"));
    			EnemyBot->Destroy();
    			AIController->Destroy();
    			BotCount--;
    		}
    	}
    	else
    	{
    		UE_LOG(LogTemp, Error, TEXT("Failed Pawn"));
    		AIController->Destroy();
    		BotCount--;
    	}
    }
    else
    {
    	UE_LOG(LogTemp, Error, TEXT("Failed Anything"));
    	BotCount--;
    }
}

void ADemolitionGameMode::SetCurMatchState(ECurMatchState NewState, bool IsInit)
{
	CooldownStartTime = GetWorld()->GetTimeSeconds();
	if (NewState == ECurMatchState::EMS_CoolDown && bIsPlanted)
	{
		RoundWin(true);
	}
	if (NewState == ECurMatchState::EMS_Waiting && CurRound == MaxRound / 2)
	{
		SetSecondHalf();
	}
	Super::SetCurMatchState(NewState, IsInit);
}

void ADemolitionGameMode::SetC4Planted(bool tf)
{
	if (bIsPlanted && !tf)
	{
		RoundWin(false);
		SetMatchTime(0);
	}
	bIsPlanted = tf;
}

// void ADemolitionGameMode::InitBot(ACDCharacter* BotCharacter)
// {
// 	UE_LOG(LogTemp, Warning, TEXT("InitBot!"));
// 	// ACDGameState* BGameState = Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
// 	// ACDPlayerState* BotPlayerState = BotCharacter->GetPlayerState<ACDPlayerState>();
// 	//
// 	// if (BGameState && BotPlayerState)
// 	// {
// 	// 	if (BGameState->BTeam.Num() >= BGameState->ATeam.Num())
// 	// 	{
// 	// 		BGameState->ATeam.AddUnique(BotPlayerState);
// 	// 		BotPlayerState->SetMatchTeam(ETeam::ET_ATeam);
// 	// 		BotPlayerState->SetTeam(ETeam::ET_RedTeam);
// 	// 	}
// 	// 	else
// 	// 	{
// 	// 		BGameState->BTeam.AddUnique(BotPlayerState);
// 	// 		BotPlayerState->SetMatchTeam(ETeam::ET_BTeam);
// 	// 		BotPlayerState->SetTeam(ETeam::ET_BlueTeam);
// 	// 	}
// 	// }
// }

void ADemolitionGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	ACDGameState* BGameState=Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	InitiateBot();
	if (BGameState)
	{
		for (auto PlayerState: BGameState->PlayerArray)//GameState->PlayerArray 가져올 수 있음.
		{
			ACDPlayerState* BPState=Cast<ACDPlayerState>(PlayerState);
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
	Super::RestartMatch(isInit);
	
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
			if (playerController && Character)
			{
				if (Character->GetTeam() == ETeam::ET_RedTeam)
				{
					Character->GiveC4();
					break;
				}
			}
		}
	}

	for (TActorIterator<AAIController> It(GetWorld()); It; ++It)
	{
		AAIController* BotController = *It;
		if (!BotController) continue;
		ACDPlayerState* BotState = BotController->GetPlayerState<ACDPlayerState>();
		if (!BotState) continue;
		ACDCharacter* BotCharacter = Cast<ACDCharacter>(BotController->GetPawn());
		if (!BotCharacter) continue;
		UCombatComponent* CombatComp = BotCharacter->GetCombatComponent();
		if (!CombatComp) continue;
		
		int32 BotGold=BotState->GetGold();
		if (BotGold>=1000 && ShopOverlay && !CombatComp->GetWeapons()[0])
		{
			FName RowName="Rifle";
			FWeaponStruct* WeaponData=WeaponDataTable->FindRow<FWeaponStruct>(RowName, TEXT("BotBuyWeapon"));
			if (WeaponData)
			{
				UWorld* World = GetWorld();
				if (!World) continue;
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = BotCharacter;
				SpawnParams.Instigator = BotCharacter;

				AWeapon* SpawnedWeapon = World->SpawnActor<AWeapon>(
					WeaponData->WeaponClass,
					BotCharacter->GetActorLocation(),
					FRotator::ZeroRotator,
					SpawnParams
				);

				if (SpawnedWeapon)
				{
					CombatComp->GetWeapon(SpawnedWeapon, true);
					BotState->SpendGold(WeaponData->Cost);
				}
			}
		}
	}
	
	InitializeTeamCount();
	//BalancedBot();//2번 반복?
}

void ADemolitionGameMode::PlayerEliminated(class AController* VictimController,
                                           AController* AttackerController)
{
	Super::PlayerEliminated(VictimController, AttackerController);
	ACDGameState* BGameState=Cast<ACDGameState>(UGameplayStatics::GetGameState(this));
	ACDPlayerState* AttackerPlayerState=AttackerController?Cast<ACDPlayerState>(AttackerController->PlayerState):nullptr;
	ACDPlayerState* VictimPlayerState=VictimController?Cast<ACDPlayerState>(VictimController->PlayerState):nullptr;
	
	if (BGameState && VictimPlayerState)
	{
		if (GetCurMatchState() != ECurMatchState::EMS_None)
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
		}
		if (GetCurMatchState() == ECurMatchState::EMS_InGame)
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
