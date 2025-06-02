// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CDServer/Data/Player/FPlayerSessionInfo.h"
#include "GameFramework/PlayerState.h"
#include "CDSessionPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API ACDSessionPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	UFUNCTION(Server,Reliable)
	void Server_PlayerReady(const FString& playerSessionId, bool ShouldReset);

	UFUNCTION(Server,Reliable)
	void Server_UpdateSession(const FString& playerSessionId, const FString& RoomMode, const FString& RoomMap);
	
	UFUNCTION(Server,Reliable)
	void Server_LeaveSession(const FString& playerSessionId);

	UFUNCTION(Client,Reliable)
	void Client_ReceivePlayerInfos(const FPlayerSessionInfoArray& InInfos);
};
