// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameLiftServerSDK.h"
#include "CDServer/UI/HTTP/HTTPRequestTypes.h"
#include "GameFramework/GameMode.h"
#include "Server_GameMode.generated.h"

/**
 * 
 */
enum WinState
{
	ATEAMLOSE = -1,
	DRAW = 0,
	ATEAMWIN = 1,
	NONE = 2,
};

DECLARE_LOG_CATEGORY_EXTERN(LogCD_ServerLog, Log, All);
#define MAX_PLAYER 6

UCLASS()
class CDSERVER_API AServer_GameMode : public AGameMode
{
	GENERATED_BODY()
public:
	AServer_GameMode();
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	
	virtual void Logout(AController* Exiting) override;
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;
	
	void StartGame();
	void EndGame(WinState winState);
	void KickPlayer(const FString& PlayerSessionId);
	class UCDGameInstanceSubsystem* GetGameInstanceSubsystem();
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameSessionsManager> GameSessionManagerClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameStatsManager> GameStatsManagerClass;
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TObjectPtr<class UGameSessionsManager> GameSessionManager;
	UPROPERTY()
	TObjectPtr<class UGameStatsManager> GameStatsManager;
private:
	void InitGameLift();
	void SetServerParameters(FServerParameters& serverParameters);

	void TryAcceptPlayerSession(const FString& PlayerSessionId, const FString& Username, FString& ErrorMessage);
	
	UPROPERTY()
	TObjectPtr<class UCDGameInstanceSubsystem> CDGameInstanceSubsystem;
};

