// Fill out your copyright notice in the Description page of Project Settings.


#include "CDLobbyGameMode.h"

#include "CDGameInstanceSubsystem.h"
#include "CDLobbyServerGameState.h"
#include "AI/NavigationSystemBase.h"
#include "CDServer/Player/CDLobbyPlayerState.h"
#include "CDServer/UI/GameSessions/GameSessionsManager.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

void ACDLobbyGameMode::RemovePlayerSession(const FString& PlayerSessionId)
{
#if WITH_GAMELIFT
	Aws::GameLift::Server::RemovePlayerSession(TCHAR_TO_UTF8(*PlayerSessionId));
#endif
	ACDLobbyServerGameState* LobbyServerGameState = GetGameState<ACDLobbyServerGameState>();
	if (IsValid(LobbyServerGameState))
	{
		LobbyServerGameState->RemovePlayerInfo(PlayerSessionId);
	}
}

void ACDLobbyGameMode::BeginPlay()
{
	UE_LOG(LogCD_ServerLog, Warning, TEXT("Server Lobby GameMode On"));

	InitGameLift();
	
	if (GetWorld() && GetWorld()->GetNetDriver())
	{
		UNetDriver* NetDriver = GetWorld()->GetNetDriver();
		FString NetworkAddress = NetDriver->LowLevelGetNetworkNumber();
	
		if (!NetworkAddress.IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("🌐 서버가 수신 중인 주소: %s"), *NetworkAddress);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("⚠ 서버 주소를 가져올 수 없습니다."));
		}
	}
	Super::BeginPlay();
}

void ACDLobbyGameMode::InitSeamlessTravelPlayer(AController* NewController)
{
	Super::InitSeamlessTravelPlayer(NewController);
}

APlayerController* ACDLobbyGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal,
	const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	const FString NetIdStr = UniqueId.IsValid() ? UniqueId->ToString() : TEXT("Unknown");
	const FString Username = UGameplayStatics::ParseOption(Options, TEXT("Username"));
	const FString PlayerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));

	//save player info
	ACDLobbyServerGameState* LobbyGameState = GetGameState<ACDLobbyServerGameState>();
	if (LobbyGameState)
	{
		LobbyGameState->AddPlayerInfo(FPlayerLobbyInfo(PlayerSessionId, Username, false, 0, NetIdStr)); // ✅ OK
	}

	if (NewPlayer->GetPlayerController(GetWorld()))
	{
		if (NewPlayer->GetPlayerController(GetWorld())->GetPlayerState<ACDLobbyPlayerState>())
		{
			NewPlayer->GetPlayerController(GetWorld())->GetPlayerState<ACDLobbyPlayerState>()->PlayerSessionId = PlayerSessionId;
		}
	}
	
	return Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
}

void ACDLobbyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	const FString PlayerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));
	const FString Username = UGameplayStatics::ParseOption(Options, TEXT("Username"));

	TryAcceptPlayerSession(PlayerSessionId, Username, ErrorMessage);
}

void ACDLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if(Exiting->GetPlayerState<ACDLobbyPlayerState>())
	{
		RemovePlayerSession(Exiting->GetPlayerState<ACDLobbyPlayerState>()->PlayerSessionId);
	}
	if (GetNumPlayers() == 0)
	{
		FGameLiftServerSDKModule* gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
		gameLiftSdkModule->ProcessEnding();
		FGenericPlatformMisc::RequestExit(false);
	}
}

void ACDLobbyGameMode::StartGame()
{
	if (IsValid(GameSessionManager))
	{
		ACDLobbyServerGameState* LobbyServerGameState = GetGameState<ACDLobbyServerGameState>();
		if (IsValid(LobbyServerGameState))
		{
			GameSessionManager->UpdateGameSession(GameSessionId, LobbyServerGameState->GetRoomMap(), LobbyServerGameState->GetRoomMode(), TEXT("true"));	
			UWorld* World = GEngine->GetWorldContexts()[0].World();
			if (World)
			{
				FString url = TEXT("/Game/Maps/") + LobbyServerGameState->GetRoomMode() + TEXT("/") + LobbyServerGameState->GetRoomMap();
				UE_LOG(LogCD_ServerLog, Warning, TEXT("%s"), *url);
				GetWorld()->ServerTravel(url);
			}
		}
	}
}

void ACDLobbyGameMode::InitGameLift()
{
	if (UGameInstance* GameInstance = GetGameInstance<UGameInstance>(); IsValid(GameInstance))
	{
		if(CDGameInstanceSubsystem = GameInstance->GetSubsystem<UCDGameInstanceSubsystem>(); IsValid(CDGameInstanceSubsystem))
		{
			
		}
	}
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
