// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CDServer/Data/Player/FPlayerSessionInfo.h"
#include "GameLiftServerSDK.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CDGameInstanceSubsystem.generated.h"

/**
 * 
 */
DECLARE_LOG_CATEGORY_EXTERN(CD_GAMEINSTANCE, Log, All);

UCLASS()
class CDSERVER_API UCDGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UCDGameInstanceSubsystem();
	void InitGameLift(const FServerParameters& ServerParameters);
	UPROPERTY(BlueprintReadOnly)
	bool bGameLiftInitialized = false;
	
private:
	void ParseCommandLienPort(int32& outPort);
	FProcessParameters m_params;

public:
	void AddPlayerInfo(FPlayerSessionInfo playerInfo);
	void RemovePlayerInfo(const FString& PlayerSessionId);
	
	FPlayerSessionInfoArray& GetPlayerInfos();
	
	const FString& GetRoomMode();
	const FString& GetRoomMap();
	void SetRoomMode(const FString& PlayerSessionId, const FString& NextRoomMode);
	void SetRoomMap(const FString& PlayerSessionId, const FString& NextRoomMap);

	const FString& GetGameSessionId();
	
	UFUNCTION()
	void Server_LeaveSession(const FString& PlayerSessionId);

	UFUNCTION()
	void Server_PlayerReady(const FString& PlayerSessionId, bool ShouldReset = false);
	
	FTimerHandle ExitHandle;
private:
	FPlayerSessionInfoArray PlayerInfos;
	//name, readystate, ping
	
	FString RoomMode;
	FString RoomMap;
	FString bIsPrivate;
	FString GameSessionId;
};
