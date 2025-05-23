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
		ErrorMessage = TEXT("PlayerSessionId and/or Username invalid");
		return;
	}

//Server Check
#if WITH_GAMELIFT
	Aws::GameLift::Server::Model::DescribePlayerSessionsRequest DescribePlayerSessionsRequest;
	DescribePlayerSessionsRequest.SetPlayerSessionId(TCHAR_TO_ANSI(*PlayerSessionId));
	const auto& DescribePlayerSessionsOutcome = Aws::GameLift::Server::DescribePlayerSessions(DescribePlayerSessionsRequest);
	if(!DescribePlayerSessionsOutcome.IsSuccess())
	{
		ErrorMessage = TEXT("DescribePlayerSession failed");
		return;
	}

	const auto& DescribePlayerSessionsResult = DescribePlayerSessionsOutcome.GetResult();
	int32 Count = 0;
	const Aws::GameLift::Server::Model::PlayerSession* PlayerSessions = DescribePlayerSessionsResult.GetPlayerSessions(Count);
	if (PlayerSessions == nullptr || Count == 0)
	{
		ErrorMessage = TEXT("GetPlayerSessions failed");
		return;
	}
	for (int32 i = 0; i < Count; i++)
	{
		if(!Username.Equals(PlayerSessions[i].GetPlayerId())) continue;
		if (PlayerSessions[i].GetStatus() != Aws::GameLift::Server::Model::PlayerSessionStatus::RESERVED)
		{
			ErrorMessage = TEXT("PlayerSessions have already been reserved");
			return;
		}
		const auto& AcceptPlayerSessionOutcome = Aws::GameLift::Server::AcceptPlayerSession(TCHAR_TO_ANSI(*PlayerSessionId));
		ErrorMessage = AcceptPlayerSessionOutcome.IsSuccess() ? "" : FString::Printf(TEXT("Failed to accept player session"));
	}
#endif
}
