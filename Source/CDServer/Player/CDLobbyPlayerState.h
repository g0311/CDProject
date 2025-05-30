// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CDLobbyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API ACDLobbyPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	UFUNCTION(Server,Reliable)
	void Server_PlayerReady(const FString& playerSessionId, bool ShouldReset);

	UFUNCTION(Server,Reliable)
	void Server_UpdateSession(const FString& playerSessionId, const FString& RoomMode, const FString& RoomMap);
	
	FString PlayerSessionId;
	//Only Using In Server
};
