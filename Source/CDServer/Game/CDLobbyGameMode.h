// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Server_GameMode.h"
#include "CDLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API ACDLobbyGameMode : public AServer_GameMode
{
	GENERATED_BODY()
public:

protected:
	virtual void BeginPlay() override;
	virtual void InitSeamlessTravelPlayer(AController* NewController) override;
	virtual void Logout(AController* Exiting) override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	
private:
	bool bIsPrivateGame = false;

	void TryAcceptPlayerSession(const FString& PlayerSessionId, const FString& Username, FString& ErrorMessage);
};
