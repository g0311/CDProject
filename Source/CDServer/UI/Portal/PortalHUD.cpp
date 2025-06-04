// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalHUD.h"
#include "SignIn/USignInOverlay.h"
#include "Blueprint/UserWidget.h"
#include "CDServer/Player/CDLocalPlayerSubsystem.h"
#include "Dashboard/DashboardOverlay.h"

void APortalHUD::OnSignIn()
{
	if (IsValid(SignInOverlay))
	{
		SignInOverlay->RemoveFromParent();
	}

	APlayerController* OwningPlayerController = GetOwningPlayerController();
	DashboardOverlay = CreateWidget<UDashboardOverlay>(OwningPlayerController, DashboardOverlayClass, TEXT("DashboardOverlay"));
	if (IsValid(DashboardOverlay))
	{
		DashboardOverlay->AddToViewport();
	}
}

void APortalHUD::OnSignOut()
{
	if (IsValid(DashboardOverlay))
	{
		DashboardOverlay->RemoveFromParent();
	}
	APlayerController* OwningPlayerController = GetOwningPlayerController();
	SignInOverlay = CreateWidget<USignInOverlay>(OwningPlayerController, SignInOverlayClass, TEXT("SignInOverlay"));
	if (IsValid(SignInOverlay))
	{
		SignInOverlay->AddToViewport();
	}
}

void APortalHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* OwningPlayerController = GetOwningPlayerController();
	if (OwningPlayerController)
	{
		if (ULocalPlayer* LocalPlayer = OwningPlayerController->GetLocalPlayer())
		{
			if (UCDLocalPlayerSubsystem* LocalPlayerSubsystem = LocalPlayer->GetSubsystem<UCDLocalPlayerSubsystem>())
			{
				if (LocalPlayerSubsystem->GetAuthResult().AccessToken.IsEmpty())
				{
					OnSignOut();
				}
				else
				{
					OnSignIn();
				}
			}
		}
	}
	FInputModeGameAndUI InputModeData;
	OwningPlayerController->SetInputMode(InputModeData);
	OwningPlayerController->SetShowMouseCursor(true);
}
