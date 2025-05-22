// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "CDProject/Character/CDCharacter.h"
#include "CDProject/HUD/CDHUD.h"
#include "GameFramework/GameStateBase.h"

ALobbyGameMode::ALobbyGameMode()
{
	DefaultPawnClass = ACDCharacter::StaticClass();
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	//UE_LOG(LogTemp, Warning, TEXT("PostLogin: %s"), *NewPlayer->GetName());
	if (NumberOfPlayers == 2)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ALobbyGameMode::StartGame);
	}
}

void ALobbyGameMode::StartGame()
{
	UWorld* World = GetWorld();
	if (World)
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (ACDHUD* CDHUD = Cast<ACDHUD>(PC->GetHUD()))
			{
				CDHUD->AddModeSelect();
			}
		}
		bUseSeamlessTravel = true;
		//World->ServerTravel(FString("/Game/Maps/CDGameMap?listen"));
	}
}


