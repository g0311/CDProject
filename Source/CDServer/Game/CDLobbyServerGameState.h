// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <ocidl.h>

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "FPlayerLobbyInfo.h"
#include "CDLobbyServerGameState.generated.h"

/**
 * 
 */

UCLASS()
class CDSERVER_API ACDLobbyServerGameState : public AGameState
{
	GENERATED_BODY()
public:
	void AddPlayerInfo(FPlayerLobbyInfo playerInfo);
	void RemovePlayerInfo(const FString& PlayerSessionId);
	
	const TArray<FPlayerLobbyInfo>& GetPlayerInfos();
	
	const FString& GetRoomMode();
	const FString& GetRoomMap();
	void SetRoomMode(const FString& PlayerSessionId, const FString& NextRoomMode);
	void SetRoomMap(const FString& PlayerSessionId, const FString& NextRoomMap);
	
	UFUNCTION()
	void Server_LeaveLobby(const FString& PlayerSessionId);

	UFUNCTION()
	void Server_PlayerReady(const FString& PlayerSessionId, bool ShouldReset = false);
	

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UPROPERTY(Replicated)
	FPlayerLobbyInfoArray PlayerInfos;
				//name, readystate, ping
	FTimerHandle LobbyCheckTimerHandle;

	FString RoomMode;
	FString RoomMap;
	
	void TickLobbyStatus();
};
