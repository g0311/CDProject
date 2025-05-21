// Fill out your copyright notice in the Description page of Project Settings.


#include "CDLocalPlayerSubsystem.h"
#include "CDServer/UI/Portal/Interfaces/PortalManagement.h"

void UCDLocalPlayerSubsystem::InitializeTokens(const FCDAuthenticationResult& AuthResult, TScriptInterface<IPortalManagement> PortalManagement)
{
	AuthenticationResult = AuthResult;
	PortalManagementInterface = PortalManagement;
	SetRefreshTokenTimer();
}

void UCDLocalPlayerSubsystem::SetRefreshTokenTimer()
{
	//Send HTTP Request
	UWorld* World = GetWorld();
	if (IsValid(World) && IsValid(PortalManagementInterface.GetObject()))
	{
		FTimerDelegate RefreshDelegate;
		RefreshDelegate.BindLambda([this]()
		{
			PortalManagementInterface->RefreshToken(AuthenticationResult.RefreshToken);
		});
		World->GetTimerManager().SetTimer(RefreshTimer, RefreshDelegate, TokenRefreshInterval, false);
	}
}

void UCDLocalPlayerSubsystem::UpdateTokens(const FString& AccessToken, const FString& IdToken)
{
	AuthenticationResult.AccessToken = AccessToken;
	AuthenticationResult.IdToken = IdToken;
	SetRefreshTokenTimer();
}

FCDAuthenticationResult UCDLocalPlayerSubsystem::GetAuthResult()
{
	return AuthenticationResult;
}
