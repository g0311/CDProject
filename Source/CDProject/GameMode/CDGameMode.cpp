// Fill out your copyright notice in the Description page of Project Settings.


#include "CDGameMode.h"

#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "CDProject/PlayerState/CDPlayerState.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"


ACDGameMode::ACDGameMode()
{
	DefaultPawnClass = ACDCharacter::StaticClass();
	bDelayedStart=true;
}

void ACDGameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelStartingTime=GetWorld()->GetTimeSeconds();
}

void ACDGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator PCIter = GetWorld()->GetPlayerControllerIterator();PCIter;++PCIter)
	{
		ACDPlayerController* PlayerController=Cast<ACDPlayerController> (*PCIter);
		if (PlayerController)
		{
			PlayerController->OnMatchStateSet(MatchState);
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




