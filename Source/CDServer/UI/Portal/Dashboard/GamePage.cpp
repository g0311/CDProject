// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePage.h"

#include "CDServer/UI/API/GameSessions/JoinGame.h"
#include "CDServer/UI/GameSessions/GameSessionsManager.h"
#include "Components/Button.h"

void UGamePage::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(GameSessionManagerClass))
	{
		GameSessionManager = NewObject<UGameSessionsManager>(this, GameSessionManagerClass);
	}
	
	JoinGameWidget->Button_JoinGame->OnClicked.AddDynamic(this, &UGamePage::JoinGameButtonClicked);
	GameSessionManager->JoinGameSessionMessageDelegate.AddDynamic(JoinGameWidget, &UJoinGame::SetStatusMessage);
}

void UGamePage::JoinGameButtonClicked()
{
	JoinGameWidget->Button_JoinGame->SetIsEnabled(false);
	GameSessionManager->JoinGameSession();
}
