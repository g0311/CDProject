// Fill out your copyright notice in the Description page of Project Settings.


#include "CDLobbyGameMode.h"

#include "Kismet/GameplayStatics.h"

void ACDLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ACDLobbyGameMode::InitSeamlessTravelPlayer(AController* NewController)
{
	Super::InitSeamlessTravelPlayer(NewController);
}

void ACDLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

void ACDLobbyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	const FString PlayerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));
	const FString Username = UGameplayStatics::ParseOption(Options, TEXT("Username"));

	TryAcceptPlayerSession(PlayerSessionId, Username, ErrorMessage);
}

void ACDLobbyGameMode::TryAcceptPlayerSession(const FString& PlayerSessionId, const FString& Username, FString& ErrorMessage)
{
	if (PlayerSessionId.IsEmpty() || Username.IsEmpty())
	{
		ErrorMessage = TEXT("PlayerSessionId and/or Username inmvalid");
		return;
	}

#if WITH_GAMELIFT
	
#endif
}
