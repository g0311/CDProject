// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "CDProject/Character/CDCharacter.h"
#include "GameFramework/GameStateBase.h"

ALobbyGameMode::ALobbyGameMode()
{
	DefaultPawnClass = ACDCharacter::StaticClass();
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	UE_LOG(LogTemp, Warning, TEXT("PostLogin: %s"), *NewPlayer->GetName());
	if (NumberOfPlayers == 2)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			bUseSeamlessTravel = true;
			World->ServerTravel(FString("/Game/Maps/CDGameMap?listen"));
		}
	}
}