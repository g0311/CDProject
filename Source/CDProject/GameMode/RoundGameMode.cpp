// Fill out your copyright notice in the Description page of Project Settings.


#include "RoundGameMode.h"

#include <filesystem>

#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "CDProject/PlayerState/CDPlayerState.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "CDProject/Weapon/Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Runtime/Core/Tests/Containers/TestUtils.h"

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
	
	if (_curMatchState==ECurMatchState::EMS_Waiting)
	{
		Countdown=FMath::CeilToInt(WaitingStartTime + WarmUpTime-GetWorld()->GetTimeSeconds());
		//UE_LOG(LogGameMode, Log, TEXT("Countdown %f"), Countdown);
		if (Countdown==-1)
		{
			SetCurMatchState(ECurMatchState::EMS_InGame);
		}
	}
	else if (_curMatchState==ECurMatchState::EMS_InGame)
	{
		Countdown=MatchStartTime + MatchTime-GetWorld()->GetTimeSeconds();
		if (Countdown<=0.f)
		{
			SetCurMatchState(ECurMatchState::EMS_CoolDown);
		}
	}
	else if (_curMatchState==ECurMatchState::EMS_CoolDown)
	{
		Countdown=CooldownStartTime + CooldownTime-GetWorld()->GetTimeSeconds();
		//UE_LOG(LogGameMode, Log, TEXT("%f %f %f"), CooldownTime, CooldownStartTime, GetWorld()->GetTimeSeconds());
		if (Countdown<=0.f)
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

	StartMatch();
}

void ARoundGameMode::OnCurMatchStateSet()
{
	for (FConstPlayerControllerIterator PCIter = GetWorld()->GetPlayerControllerIterator();PCIter;++PCIter)
	{
		ACDPlayerController* PlayerController=Cast<ACDPlayerController> (*PCIter);
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
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddDeath();
	}
	// for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	// {
	// 	ACDPlayerController* CDPC = Cast<ACDPlayerController>(*It);
	// 	if (CDPC)
	// 	{//Need to Set Client RPC
	// 		CDPC->UpdateKDOverlayData();
	// 	}
	// }
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

void ARoundGameMode::RestartMatch(bool isForce)
{
	for (FConstPlayerControllerIterator PCIter = GetWorld()->GetPlayerControllerIterator(); PCIter; ++PCIter)
	{
		AController* Controller = Cast<AController>(*PCIter);
		if (Controller)
		{
			ACDPlayerController* playerController=Cast<ACDPlayerController>(Controller);
			ACDCharacter* Character = Cast<ACDCharacter>(Controller->GetCharacter());
			if (Character && playerController)
			{
				if (isForce)
					Character->Kill();
				Character->Reset();
				AActor* playerStart = FindPlayerStart(playerController);
				if (playerStart)
				{
					Character->SetActorLocation(playerStart->GetActorLocation());
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
			if (Cast<AWeapon>(actor) && Cast<AWeapon>(actor)->GetWeaponState() != EWeaponState::EWS_Dropped)
				continue;
			actor->Destroy();
		}
	}
	_createdActors.Empty();
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

void ARoundGameMode::SendPlayerJoined()
{
	//need Refactor to check all player joined
	_joinedClinetCount++;
	if (_joinedClinetCount >= _maxClientCount)
	{
		SetCurMatchState(ECurMatchState::EMS_Waiting, true);
	}
	//게임 모드에서 체크 시 컨트롤러 초기화가 덜된 상태기 때문에 스테이트 on rep이 호출이 안됨
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
	else if (_curMatchState == ECurMatchState::EMS_InGame)
	{
		MatchStartTime = GetWorld()->GetTimeSeconds();
		UE_LOG(LogGameMode, Log, TEXT("EMS_InGame"));
	}
	else if (_curMatchState == ECurMatchState::EMS_CoolDown)
	{
		CooldownStartTime = GetWorld()->GetTimeSeconds();
		UE_LOG(LogGameMode, Log, TEXT("EMS_CoolDown"));
	}
	else if (_curMatchState == ECurMatchState::EMS_GameEnd)
	{
		//Shut Down Server After 30 sec
	}
	OnCurMatchStateSet();
}
