// Fill out your copyright notice in the Description page of Project Settings.


#include "CDSessionPlayerController.h"

#include "CDServer/Game/CDGameInstanceSubsystem.h"
#include "CDServer/Game/CDSessionGameState.h"
#include "Net/UnrealNetwork.h"

void ACDSessionPlayerController::Server_PlayerReady_Implementation(bool ShouldReset)
{
	if (GetWorld())
	{
		if (ACDSessionGameState* SessionGameState = GetWorld()->GetGameState<ACDSessionGameState>(); IsValid(SessionGameState))
		{
			SessionGameState->Server_PlayerReady(PlayerSessionId, ShouldReset);
		}
	}
}

void ACDSessionPlayerController::Server_UpdateSession_Implementation(const FString& RoomMode,
	const FString& RoomMap)
{
	if (GetWorld())
	{
		if (ACDSessionGameState* SessionGameState = GetWorld()->GetGameState<ACDSessionGameState>(); IsValid(SessionGameState))
		{
			SessionGameState->SetRoomMode(PlayerSessionId, RoomMode);
			SessionGameState->SetRoomMap(PlayerSessionId, RoomMap);
		}
	}
}

void ACDSessionPlayerController::Server_LeaveSession_Implementation()
{
	if (GetWorld())
	{
		if (ACDSessionGameState* SessionGameState = GetWorld()->GetGameState<ACDSessionGameState>(); IsValid(SessionGameState))
		{
			SessionGameState->Server_LeaveSession(PlayerSessionId);
		}
	}
}

void ACDSessionPlayerController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACDSessionPlayerController, PlayerSessionId);
}

const FString& ACDSessionPlayerController::GetPlayerSessionId() const
{
	return PlayerSessionId;
}

void ACDSessionPlayerController::SetPlayerSessionId(const FString& playerSessionId)
{
	PlayerSessionId = playerSessionId;
}
