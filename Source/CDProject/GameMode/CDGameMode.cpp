// Fill out your copyright notice in the Description page of Project Settings.


#include "CDGameMode.h"

#include "CDProject/Controller/CDPlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"


ACDGameMode::ACDGameMode()
{
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
	//elim()
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


