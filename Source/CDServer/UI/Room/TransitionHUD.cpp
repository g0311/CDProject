// Fill out your copyright notice in the Description page of Project Settings.


#include "TransitionHUD.h"
#include "RoomPage.h"
#include "CDServer/Game/CDGameInstanceSubsystem.h"
#include "CDServer/Player/CDSessionPlayerState.h"

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
	if (IsValid(GetGameInstance()))
	{
		UCDGameInstanceSubsystem* GameInstanceSubsystem = GetGameInstance()->GetSubsystem<UCDGameInstanceSubsystem>();
		if (IsValid(GameInstanceSubsystem))
		{
			FPlayerSessionInfoArray& Infos = GameInstanceSubsystem->GetPlayerInfos();
			RoomPage->UpdatePlayerList(Infos.Items, GameInstanceSubsystem->GetRoomMode(), GameInstanceSubsystem->GetRoomMap());
		}
	}
}
