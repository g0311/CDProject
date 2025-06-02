// Fill out your copyright notice in the Description page of Project Settings.


#include "CDSessionPlayerState.h"

#include "CDServer/Game/CDGameInstanceSubsystem.h"

void ACDSessionPlayerState::Server_PlayerReady_Implementation(const FString& playerSessionId, bool ShouldReset)
{
	if (IsValid(GetGameInstance()))
	{
		UCDGameInstanceSubsystem* GameInstanceSubsystem = GetGameInstance()->GetSubsystem<UCDGameInstanceSubsystem>();
		if (IsValid(GameInstanceSubsystem))
		{
			GameInstanceSubsystem->Server_PlayerReady(playerSessionId, ShouldReset);
		}
	}
}

void ACDSessionPlayerState::Server_UpdateSession_Implementation(const FString& playerSessionId, const FString& RoomMode,
	const FString& RoomMap)
{
	if (IsValid(GetGameInstance()))
	{
		UCDGameInstanceSubsystem* GameInstanceSubsystem = GetGameInstance()->GetSubsystem<UCDGameInstanceSubsystem>();
		if (IsValid(GameInstanceSubsystem))
		{
			GameInstanceSubsystem->SetRoomMode(playerSessionId, RoomMode);
			GameInstanceSubsystem->SetRoomMap(playerSessionId, RoomMap);
		}
	}
}

void ACDSessionPlayerState::Server_LeaveSession_Implementation(const FString& playerSessionId)
{
	if (IsValid(GetGameInstance()))
	{
		UCDGameInstanceSubsystem* GameInstanceSubsystem = GetGameInstance()->GetSubsystem<UCDGameInstanceSubsystem>();
		if (IsValid(GameInstanceSubsystem))
		{
			GameInstanceSubsystem->Server_LeaveSession(playerSessionId);
		}
	}
}

void ACDSessionPlayerState::Client_ReceivePlayerInfos_Implementation(const FPlayerSessionInfoArray& InInfos)
{
	if (IsValid(GetGameInstance()))
	{
		UCDGameInstanceSubsystem* GameInstanceSubsystem = GetGameInstance()->GetSubsystem<UCDGameInstanceSubsystem>();
		if (IsValid(GameInstanceSubsystem))
		{
			GameInstanceSubsystem->GetPlayerInfos() = InInfos;
		}
	}
}