// Fill out your copyright notice in the Description page of Project Settings.


#include "TransitionHUD.h"
#include "RoomPage.h"
#include "CDServer/Game/CDGameInstanceSubsystem.h"
#include "CDServer/Game/CDSessionGameState.h"

class ACDSessionPlayerState;

void ATransitionHUD::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* OwningPlayerController = GetOwningPlayerController();
	check(IsValid(RoomPageClass));
	RoomPage = CreateWidget<URoomPage>(OwningPlayerController, RoomPageClass, TEXT("DashboardOverlay"));
	if (IsValid(RoomPage))
	{
		RoomPage->AddToViewport();
	}
	
	FInputModeGameAndUI InputModeData;
	OwningPlayerController->SetInputMode(InputModeData);
	OwningPlayerController->SetShowMouseCursor(true);
	
	GetWorldTimerManager().SetTimer(
		LobbyCheckTimerHandle,
		this,
		&ATransitionHUD::UpdateRoomPage,
		0.5f,
	   true
		);
}

void ATransitionHUD::UpdateRoomPage()
{
	if (GetWorld())
	{
		if (ACDSessionGameState* SessionGameState = GetWorld()->GetGameState<ACDSessionGameState>(); IsValid(SessionGameState))
		{
			FPlayerSessionInfoArray& Infos = SessionGameState->GetPlayerInfos();
			RoomPage->UpdatePlayerList(Infos, SessionGameState->GetRoomName(),SessionGameState->GetRoomMode(), SessionGameState->GetRoomMap());
		}
	}
}
