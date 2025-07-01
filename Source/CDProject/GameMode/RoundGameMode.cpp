// Fill out your copyright notice in the Description page of Project Settings.


#include "RoundGameMode.h"

#include <filesystem>

#include "AIController.h"
#include "EngineUtils.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "CDProject/PlayerState/CDPlayerState.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "CDProject/AI/CDAIController.h"
#include "CDProject/GameState/CDGameState.h"
#include "CDProject/Weapon/C4Weapon.h"
#include "CDProject/Weapon/Weapon.h"
#include "CDServer/Game/CDGameInstanceSubsystem.h"
#include "CDServer/Game/CDSessionGameState.h"

namespace MatchState
{
	const FName PreInProgress=FName("PreProgress");
	const FName Cooldown=FName("Cooldown");
	const FName ModeSelect=FName("ModeSelect");
}

ARoundGameMode::ARoundGameMode()
{
	DefaultPawnClass = ACDCharacter::StaticClass();
}

void ARoundGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if(ACDGameState* CDGameState = GetGameState<ACDGameState>(); IsValid(CDGameState))
	{
		if (IsRunningDedicatedServer())
			_maxClientCount = CDGameState->GetPlayerInfos().Items.Num();
		else
			_maxClientCount = 2;
	}
	
	if (ACDPlayerController* PC = Cast<ACDPlayerController>(NewPlayer))
	{
		PC->InitializeController();
	}
	
	_joinedClinetCount++;
	if (GetCurMatchState() == ECurMatchState::EMS_None && _joinedClinetCount >= _maxClientCount)
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle ,FTimerDelegate::CreateLambda([this]()
		{
			if (IsValid(this))
				SetCurMatchState(ECurMatchState::EMS_Waiting, true);
		}), 3.f, false);
		if (ACDPlayerController* PC = Cast<ACDPlayerController>(NewPlayer))
		{
			PC->ShowAnnounceText(false);
		}
	}
}

void ARoundGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);
	
	if(ACDGameState* CDGameState = GetGameState<ACDGameState>(); IsValid(CDGameState))
	{
		_maxClientCount = CDGameState->GetPlayerInfos().Items.Num();
	}
	
	if (ACDPlayerController* PC = Cast<ACDPlayerController>(C))
	{
		PC->InitializeController();
	}
	
	_joinedClinetCount++;
	if (GetCurMatchState() == ECurMatchState::EMS_None && _joinedClinetCount >= _maxClientCount)
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle ,FTimerDelegate::CreateLambda([this]()
		{
			if (IsValid(this))
				SetCurMatchState(ECurMatchState::EMS_Waiting, true);
		}), 3.f, false);
		if (ACDPlayerController* PC = Cast<ACDPlayerController>(C))
		{
			PC->ShowAnnounceText(false);
		}
	}
}

void ARoundGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (_curMatchState==ECurMatchState::EMS_Waiting)
	{
		Countdown=FMath::CeilToInt(WaitingStartTime + WarmUpTime-GetWorld()->GetTimeSeconds());
		//UE_LOG(LogGameMode, Log, TEXT("Countdown %f"), Countdown);
		if (Countdown<=0.1f)
		{
			SetCurMatchState(ECurMatchState::EMS_InGame);
		}
	}
	else if (_curMatchState==ECurMatchState::EMS_InGame)
	{
		Countdown=MatchStartTime + MatchTime-GetWorld()->GetTimeSeconds();
		if (Countdown<=0.1f)
		{
			SetCurMatchState(ECurMatchState::EMS_CoolDown);
		}
	}
	else if (_curMatchState==ECurMatchState::EMS_CoolDown)
	{
		Countdown=CooldownStartTime + CooldownTime-GetWorld()->GetTimeSeconds();
		//UE_LOG(LogGameMode, Log, TEXT("%f %f %f"), CooldownTime, CooldownStartTime, GetWorld()->GetTimeSeconds());
		if (Countdown<=0.1f)
		{
			//UE_LOG(LogGameMode, Log, TEXT("Restart Called"));
			SetCurMatchState(ECurMatchState::EMS_Waiting);
		}
	}
}

void ARoundGameMode::BeginPlay()
{
	Super::BeginPlay();
	//LevelStartingTime=GetWorld()->GetTimeSeconds();
	//StartMatch();
	MatchTime = defaultMatchTime;
	
	UCDGameInstanceSubsystem* GameInstanceSubsystem = GetGameInstance()->GetSubsystem<UCDGameInstanceSubsystem>();
	if (IsValid(GameInstanceSubsystem))
	{
		_maxClientCount = GameInstanceSubsystem->PlayerInfos.Items.Num();
	}
}

void ARoundGameMode::OnCurMatchStateSet()
{
	for (TActorIterator<AController> It(GetWorld()); It; ++It)
	{
		ACDPlayerController* PlayerController=Cast<ACDPlayerController> (*It);
		if (PlayerController)
		{
			if(_curMatchState==ECurMatchState::EMS_Waiting)
			{
				PlayerController->OnMatchStateSet(_curMatchState, bTeamsMatch, WaitingStartTime);
			}
			else if(_curMatchState==ECurMatchState::EMS_InGame)
			{
				PlayerController->OnMatchStateSet(_curMatchState, bTeamsMatch, MatchStartTime);			
			}
			else if (_curMatchState==ECurMatchState::EMS_CoolDown)
			{
				PlayerController->OnMatchStateSet(_curMatchState, bTeamsMatch, CooldownStartTime);
			}
			else if (_curMatchState==ECurMatchState::EMS_GameEnd)
			{
				PlayerController->OnMatchStateSet(_curMatchState);
			}
		}
		
		if (ACDAIController* AIController = Cast<ACDAIController> (*It))
		{
			if(_curMatchState==ECurMatchState::EMS_Waiting)
			{
				AIController->StopBehavior();
			}
			else if(_curMatchState==ECurMatchState::EMS_InGame)
			{
				AIController->RestartBehavior();
			}
		}
	}
}

void ARoundGameMode::PlayerEliminated(class AController* VictimController,
                                      AController* AttackerController)
{
	if (AttackerController==nullptr||AttackerController->PlayerState==nullptr) return;
	if (VictimController==nullptr||VictimController->PlayerState==nullptr) return;
	ACDPlayerState* AttackerPlayerState=AttackerController?Cast<ACDPlayerState>(AttackerController->PlayerState):nullptr;
	ACDPlayerState* VictimPlayerState=VictimController?Cast<ACDPlayerState>(VictimController->PlayerState):nullptr;

	//Deactive Inputs
	if (ACDAIController* CDAIController = Cast<ACDAIController>(VictimController); IsValid(CDAIController))
	{
		CDAIController->StopBehavior();
	}
	if (ACDPlayerController* PlayerController = Cast<ACDPlayerController>(VictimController))
	{
		PlayerController->ClientSetPlayerAlive(false);
		PlayerController->ClientSetEnableInput(false);
	}
	
	if (GetCurMatchState() == ECurMatchState::EMS_None)
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, VictimController]()
		{
			if (!IsValid(this))
				return;
			if (VictimController)
			{
				if (ACDCharacter* Character = Cast<ACDCharacter>(VictimController->GetCharacter()))
				{
					Character->Reset();
				
					AActor* playerStart = FindPlayerStart(VictimController);
					if (playerStart)
					{
						Character->SetActorLocation(playerStart->GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);
						Character->SetActorRotation(playerStart->GetActorRotation());
						VictimController->SetControlRotation(playerStart->GetActorRotation());
					}
				}
				if (ACDPlayerController* PlayerController = Cast<ACDPlayerController>(VictimController))
				{
					PlayerController->ClientSetPlayerAlive(true);
					PlayerController->ClientSetEnableInput(true);
				}
				if (ACDAIController* AIController = Cast<ACDAIController>(VictimController))
				{
					AIController->RestartBehavior();
				}
			}
		}), 1.5f, false);
	}
	
	if (GetCurMatchState() ==  ECurMatchState::EMS_InGame)
	{
		if (AttackerPlayerState)
		{
			AttackerPlayerState->AddKill();
		}
		if (VictimPlayerState)
		{
			VictimPlayerState->AddDeath();
		}
	}
}

void ARoundGameMode::RestartMatch(bool isInit)
{
	for (TActorIterator<AController> It(GetWorld()); It; ++It)
	{
		AController* Controller = *It;
		if (Controller)
		{
			if (ACDCharacter* Character = Cast<ACDCharacter>(Controller->GetCharacter()))
			{
				if (isInit)
				{
					Character->DestroyAllWeapon();
					Character->Kill();
				}
				Character->Reset();
				
				AActor* playerStart = FindPlayerStart(Controller);
				if (playerStart)
				{
					Character->SetActorLocation(playerStart->GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);
					Character->SetActorRotation(playerStart->GetActorRotation());
					Controller->SetControlRotation(playerStart->GetActorRotation());
				}
			}
		}
	}
	for (auto actor : _createdActors)
	{
		if (IsValid(actor))
		{
			if (Cast<AWeapon>(actor) && Cast<AWeapon>(actor)->GetWeaponState() != EWeaponState::EWS_Dropped && !Cast<AC4Weapon>(actor))
				continue;
			
			actor->Destroy();
		}
	}
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

void ARoundGameMode::SetCurMatchState(ECurMatchState NewState, bool IsInit)
{
	if (CurRound == MaxRound)
	{
		NewState = ECurMatchState::EMS_GameEnd;
	}
	
	_curMatchState = NewState;
	if (_curMatchState == ECurMatchState::EMS_Waiting)
	{
		MatchTime = defaultMatchTime;
		WaitingStartTime = GetWorld()->GetTimeSeconds();
		RestartMatch(IsInit);
	}
	if (_curMatchState == ECurMatchState::EMS_InGame)
	{
		MatchStartTime = GetWorld()->GetTimeSeconds();
		UE_LOG(LogGameMode, Log, TEXT("EMS_InGame"));
	}
	if (_curMatchState == ECurMatchState::EMS_CoolDown)
	{
		CooldownStartTime = GetWorld()->GetTimeSeconds();
		UE_LOG(LogGameMode, Log, TEXT("EMS_CoolDown"));
	}
	if (_curMatchState == ECurMatchState::EMS_GameEnd)
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			if (IsValid(this))
			{
				if (ACDGameState* CDGameState = GetGameState<ACDGameState>(); IsValid(CDGameState))
				{
					if (CDGameState->TeamAScore > CDGameState->TeamBScore)
						EndGame(WinState::ATEAMWIN);
					else if (CDGameState->TeamAScore == CDGameState->TeamBScore)
						EndGame(WinState::DRAW);
					else
						EndGame(WinState::ATEAMLOSE);		
				}
			}
		}), 10.f, false);
	}
	OnCurMatchStateSet();
}
