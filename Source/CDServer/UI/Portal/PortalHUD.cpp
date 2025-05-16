// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalHUD.h"
#include "SignIn/USignInOverlay.h"
#include "Blueprint/UserWidget.h"

void APortalHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* OwningPlayerController = GetOwningPlayerController();
	SignInOverlay = CreateWidget<USignInOverlay>(OwningPlayerController, SignInOverlayClass, TEXT("SignInOverlay"));
	if (IsValid(SignInOverlay))
	{
		SignInOverlay->AddToViewport();
	}

	FInputModeGameAndUI InputModeData;
	OwningPlayerController->SetInputMode(InputModeData);
	OwningPlayerController->SetShowMouseCursor(true);

	
}
