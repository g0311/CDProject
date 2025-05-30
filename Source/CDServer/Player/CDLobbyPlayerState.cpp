// Fill out your copyright notice in the Description page of Project Settings.


#include "CDLobbyPlayerState.h"
#include "CDServer/Game/CDLobbyServerGameState.h"

void ACDLobbyPlayerState::Server_PlayerReady_Implementation(const FString& playerSessionId, bool ShouldReset)
{
	ACDLobbyServerGameState* GameState = GetWorld()->GetGameState<ACDLobbyServerGameState>();
	if (GameState)
	{
		GameState->Server_PlayerReady(playerSessionId, ShouldReset);
	}
}

void ACDLobbyPlayerState::Server_UpdateSession_Implementation(const FString& playerSessionId, const FString& RoomMode,
	const FString& RoomMap)
{
	ACDLobbyServerGameState* GameState = GetWorld()->GetGameState<ACDLobbyServerGameState>();
	if (GameState)
	{
		GameState->SetRoomMode(playerSessionId, RoomMode);
		GameState->SetRoomMap(playerSessionId, RoomMap);
	}
}