// Fill out your copyright notice in the Description page of Project Settings.


#include "CDGameMode.h"

#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

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
