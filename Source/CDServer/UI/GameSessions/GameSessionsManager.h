// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CDServer/UI/HTTP/HTTPRequestManager.h"
#include "Interfaces/IHttpRequest.h"
#include "GameSessionsManager.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGetSessionsRequestSucceeded, const FCDDescribeGameSessionResult&, DescribeGameSessionResult);

UCLASS()
class CDSERVER_API UGameSessionsManager : public UHTTPRequestManager
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FAPIStatusMessage JoinGameSessionMessageDelegate;
	UPROPERTY(BlueprintAssignable)
	FOnGetSessionsRequestSucceeded OnGetSessionsRequestSucceeded;
	UPROPERTY(BlueprintAssignable)
	FAPIStatusMessage PrivateSessionDelegate;
	UPROPERTY(BlueprintAssignable)
	FAPIStatusMessage PrivateSessionCreateDelegate;
	
	UFUNCTION()
	void JoinGameSession(const FString& GameMode);
	void FindGameSessions();
	void CreatePrivateGameSession(const FString& RoomName, const FString& RoomMode, const FString& RoomMap);
	void JoinPrivateGameSession(const FString& GameSessionId);

private:
	void FindOrCreateGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded);
	void CreatePlayerSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded);
	void GetGameSessions_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded);
	
	FString GetUniquePlayerId();
	void HandleGameSessionStatus(const FString& Status, const FString& SessionId, const FString& GameMode);
	void TryCreatePlayerSession(const FString& PlayerId, const FString& GameSessionId);
	
	FTimerHandle CreatePlayerSessionTimer;
};
