// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CDSessionPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API ACDSessionPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(Server,Reliable)
	void Server_PlayerReady(bool ShouldReset);

	UFUNCTION(Server,Reliable)
	void Server_UpdateSession(const FString& RoomMode, const FString& RoomMap);
	
	UFUNCTION(Server,Reliable)
	void Server_KickSession(const FString& playerSessionId);
	
	UFUNCTION(Server,Reliable)
	void Server_SetTeam(bool isRed);

	const FString& GetPlayerSessionId() const;
	void SetPlayerSessionId(const FString& playerSessionId);
private:
	UPROPERTY(Replicated, VisibleAnywhere)
	FString PlayerSessionId;
};