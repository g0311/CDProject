// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CDServer/UI/HTTP/HTTPRequestTypes.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "CDLocalPlayerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API UCDLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
public:
	void InitializeTokens(const FCDAuthenticationResult& AuthResult, TScriptInterface<class IPortalManagement> PortalManagement);
	void SetRefreshTokenTimer();
	void UpdateTokens(const FString& AccessToken, const FString& IdToken);
	FCDAuthenticationResult GetAuthResult();

	FString Username;
	FString Email;
	FString PlayerSessionId;
private:
	UPROPERTY();
    FCDAuthenticationResult AuthenticationResult;
	UPROPERTY()
	TScriptInterface<IPortalManagement> PortalManagementInterface;

	float TokenRefreshInterval = 2700.f; //timeout time: 3600
	FTimerHandle RefreshTimer;
};
