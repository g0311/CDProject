// Fill out your copyright notice in the Description page of Project Settings.


#include "CDLobbyServerGameState.h"

#include "CDLobbyGameMode.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

void ACDLobbyServerGameState::AddPlayerInfo(FPlayerLobbyInfo playerInfo)
{
	PlayerInfos.AddPlayer(playerInfo);
}

void ACDLobbyServerGameState::RemovePlayerInfo(const FString& PlayerSessionId)
{
	for (auto& playerInfo : PlayerInfos.Items)
	{
		if (PlayerSessionId == playerInfo.PlayerSessionId)
		{
			PlayerInfos.RemovePlayer(playerInfo);
		}
	}
}

const TArray<FPlayerLobbyInfo>& ACDLobbyServerGameState::GetPlayerInfos()
{
	return PlayerInfos.Items;
}

const FString& ACDLobbyServerGameState::GetRoomMode()
{
	return RoomMode;
}

const FString& ACDLobbyServerGameState::GetRoomMap()
{
	return RoomMap;
}

void ACDLobbyServerGameState::SetRoomMode(const FString& PlayerSessionId, const FString& NextRoomMode)
{
	if (!PlayerInfos.Items.IsEmpty() && PlayerSessionId == PlayerInfos.Items[0].PlayerSessionId)
	{
		this->RoomMode = NextRoomMode;
	}
}

void ACDLobbyServerGameState::SetRoomMap(const FString& PlayerSessionId, const FString& NextRoomMap)
{
	if (!PlayerInfos.Items.IsEmpty() && PlayerSessionId == PlayerInfos.Items[0].PlayerSessionId)
	{
		this->RoomMap = NextRoomMap;
	}
}

void ACDLobbyServerGameState::Server_LeaveLobby(const FString& PlayerSessionId)
{
	ACDLobbyGameMode* LobbyGameMode = Cast<ACDLobbyGameMode>(GetWorld()->GetAuthGameMode());
	if (LobbyGameMode)
	{
		LobbyGameMode->RemovePlayerSession(PlayerSessionId);
	}
}

void ACDLobbyServerGameState::Server_PlayerReady(const FString& PlayerSessionId, bool ShouldReset)
{
	if (PlayerInfos.IsPlayerHost(PlayerSessionId))
	{
		UE_LOG(LogCD_ServerLog, Warning, TEXT("Server_PlayerReady: Player Is Host"));
		if (PlayerInfos.IsAllPlayerReady())
		{
			ACDLobbyGameMode* LobbyGameMode = Cast<ACDLobbyGameMode>(GetWorld()->GetAuthGameMode());
			if (IsValid(LobbyGameMode))
			{
				LobbyGameMode->StartGame();	
			}
		}
	}
	else
	{
		UE_LOG(LogCD_ServerLog, Warning, TEXT("Server_PlayerReady: Player Is Not Host"));
		PlayerInfos.UpdatePlayerReadyState(PlayerSessionId, ShouldReset);
		PlayerInfos.Log();
	}
}

void ACDLobbyServerGameState::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(
	   LobbyCheckTimerHandle,
	   this,
	   &ACDLobbyServerGameState::TickLobbyStatus,
	   1.0f,
	   true
		);
	}
}

void ACDLobbyServerGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACDLobbyServerGameState, PlayerInfos);
}

void ACDLobbyServerGameState::TickLobbyStatus()
{
	for (APlayerState* PS : PlayerArray)
	{
		if (!PS) continue;

		FString NetIdStr = PS->GetUniqueId().IsValid() ? PS->GetUniqueId()->ToString() : TEXT("Unknown");
		int32 Ping = FMath::RoundToInt(PS->ExactPing);
		for (auto& Info : PlayerInfos.Items)
		{
			if (Info.NetIDStr == NetIdStr)
			{
				Info.Ping = Ping;
				break;
			}
		}
	}
}
