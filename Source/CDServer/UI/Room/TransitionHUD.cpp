// Fill out your copyright notice in the Description page of Project Settings.


#include "TransitionHUD.h"
#include "RoomPage.h"
#include "CDServer/Game/CDLobbyServerGameState.h"
#include "CDServer/Game/FPlayerLobbyInfo.h"

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
	ACDLobbyServerGameState* GameState = GetWorld()->GetGameState<ACDLobbyServerGameState>();
	if (!GameState) return;

	const TArray<FPlayerLobbyInfo>& Infos = GameState->GetPlayerInfos();
	RoomPage->UpdatePlayerList(Infos, GameState->GetRoomMode(), GameState->GetRoomMap());
}
