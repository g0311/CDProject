// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CDServer/Data/Player/FPlayerSessionInfo.h"
#include "CDServer/Player/CDSessionPlayerController.h"
#include "GameFramework/GameState.h"
#include "CDSessionGameState.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API ACDSessionGameState : public AGameState
{
	GENERATED_BODY()
public:
	ACDSessionGameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void AddPlayerInfo(FPlayerSessionInfo playerInfo);
	void RemovePlayerInfo(const FString& PlayerSessionId);
	
	FPlayerSessionInfoArray& GetPlayerInfos();
	
	const FString& GetRoomMode();
	const FString& GetRoomMap();
	const FString& GetRoomName();
	void SetRoomMode(const FString& PlayerSessionId, const FString& NextRoomMode);
	void SetRoomMap(const FString& PlayerSessionId, const FString& NextRoomMap);

	const FString& GetGameSessionId();
	bool IsPrivate();
	
	UFUNCTION()
	void LeaveSession(const FString& PlayerSessionId);

	UFUNCTION()
	void PlayerReady(const FString& PlayerSessionId, bool ShouldReset = false);

	UFUNCTION()
	void ChangeTeam(const FString& PlayerSessionId, bool IsATeam);

	void UpdateProperty(FString Mode, FString Map, FString Name, FString Private, FString SessionId);
	void PushProperty();

	APlayerState* GetPlayerState(const FString& PlayerSessionId);
	void SetPlayerStateInfos(const FPlayerSessionInfo& playerInfo);
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(Replicated, VisibleAnywhere)
	FPlayerSessionInfoArray PlayerInfos;
	//name, readystate, ping
	UPROPERTY(Replicated)
	FString RoomMode;
	UPROPERTY(Replicated)
	FString RoomMap;
	UPROPERTY(Replicated)
	FString RoomName;
	
	FString bIsPrivate;
	FString GameSessionId;
	
};
