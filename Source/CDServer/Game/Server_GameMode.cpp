// Fill out your copyright notice in the Description page of Project Settings.


#include "Server_GameMode.h"

#include "CDGameInstanceSubsystem.h"
#include "CDServer/Player/CDSessionPlayerState.h"
#include "CDServer/UI/GameSessions/GameSessionsManager.h"
#include "GameFramework/GameState.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogCD_ServerLog);

AServer_GameMode::AServer_GameMode()
{
    bUseSeamlessTravel = true;
}

void AServer_GameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
                                FString& ErrorMessage)
{
    Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
    
    const FString PlayerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));
    const FString Username = UGameplayStatics::ParseOption(Options, TEXT("Username"));
    
    TryAcceptPlayerSession(PlayerSessionId, Username, ErrorMessage);
}

APlayerController* AServer_GameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal,
                                           const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
    if (UGameInstance* GameInstance = GetGameInstance(); IsValid(GameInstance))
    {
        if (CDGameInstanceSubsystem = GameInstance->GetSubsystem<UCDGameInstanceSubsystem>(); IsValid(CDGameInstanceSubsystem))
        {
            UE_LOG(LogCD_ServerLog, Warning, TEXT("LogIn"));
            if (GetWorld()->GetTimerManager().IsTimerActive(CDGameInstanceSubsystem->ExitHandle))
            {
                UE_LOG(LogCD_ServerLog, Warning, TEXT("Clear Timeout Timer"));
                GetWorld()->GetTimerManager().ClearTimer(CDGameInstanceSubsystem->ExitHandle);
            }
        }
    }

    const FString NetIdStr = UniqueId.IsValid() ? UniqueId->ToString() : TEXT("Unknown");
    const FString Username = UGameplayStatics::ParseOption(Options, TEXT("Username"));
    const FString PlayerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));

    //save player info
    UCDGameInstanceSubsystem* GameInstanceSubsystem = GetGameInstanceSubsystem();
    if (IsValid(GameInstanceSubsystem))
    {
        GameInstanceSubsystem->AddPlayerInfo(FPlayerSessionInfo(PlayerSessionId, Username, false, 0, NetIdStr));
    }
    
    return Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
}

void AServer_GameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    if (GetNumPlayers() == 0)
    {
        UE_LOG(LogCD_ServerLog, Warning, TEXT("Session Empty"));
        FGameLiftServerSDKModule* gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
        gameLiftSdkModule->ProcessEnding();
    }
}

void AServer_GameMode::HandleSeamlessTravelPlayer(AController*& C)
{
    Super::HandleSeamlessTravelPlayer(C);
    
    APlayerController* PC = Cast<APlayerController>(C);
    if (PC)
    {
        PC->ClientSetHUD(HUDClass);
        
        if (PC->GetPawn())
        {
            PC->GetPawn()->Destroy();
        }
        RestartPlayer(PC);
    }
}
void AServer_GameMode::StartGame()
{
    if (IsValid(GameSessionManager))
    {
        UCDGameInstanceSubsystem* GameInstanceSubsystem = GetGameInstanceSubsystem();
        if (IsValid(GameInstanceSubsystem))
        {
            GameSessionManager->UpdateGameSession(GameInstanceSubsystem->GetGameSessionId(), GameInstanceSubsystem->GetRoomMap(), GameInstanceSubsystem->GetRoomMode(), TEXT("true"));	
            UWorld* World = GEngine->GetWorldContexts()[0].World();
            if (World)
            {
                FString url = TEXT("/Game/Maps/") + GameInstanceSubsystem->GetRoomMode() + TEXT("/") + GameInstanceSubsystem->GetRoomMap();
                UE_LOG(LogCD_ServerLog, Warning, TEXT("%s"), *url);
                GetWorld()->ServerTravel(url, false);
                GetWorld()->SeamlessTravel(url);
                
            }
        }
    }
}

UCDGameInstanceSubsystem* AServer_GameMode::GetGameInstanceSubsystem()
{
    if (UGameInstance* GameInstance = GetGameInstance(); IsValid(GameInstance))
    {
        if (CDGameInstanceSubsystem = GameInstance->GetSubsystem<UCDGameInstanceSubsystem>(); IsValid(CDGameInstanceSubsystem))
        {
            return CDGameInstanceSubsystem;
        }
    }
    return nullptr;
}

void AServer_GameMode::BeginPlay()
{
    InitGameLift();
    UE_LOG(LogCD_ServerLog, Warning, TEXT("Server Lobby GameMode On"));

    if (GameSessionManagerClass)
    {
        GameSessionManager = NewObject<UGameSessionsManager>(this, GameSessionManagerClass);
    }
    else
    {
        UE_LOG(LogCD_ServerLog, Warning, TEXT("Session Manager is null"));
    }
    
    if (HasAuthority())
    {
        GetWorldTimerManager().SetTimer(
       LobbyCheckTimerHandle,
       this,
       &AServer_GameMode::UpdatePlayersStatus,
       0.5f,
       true
        );
    }
	
    Super::BeginPlay();
}

void AServer_GameMode::InitGameLift()
{
    if (UGameInstance* GameInstance = GetGameInstance(); IsValid(GameInstance))
    {
        if (CDGameInstanceSubsystem = GameInstance->GetSubsystem<UCDGameInstanceSubsystem>(); IsValid(CDGameInstanceSubsystem))
        {
            FServerParameters ServerParameters;
            SetServerParameters(ServerParameters);
            CDGameInstanceSubsystem->InitGameLift(ServerParameters);
        }
    }
}

void AServer_GameMode::SetServerParameters(FServerParameters& serverParameters)
{
    //AuthToken returned from the "aws gamelift get-compute-auth-token" API. Note this will expire and require a new call to the API after 15 minutes.
    if (FParse::Value(FCommandLine::Get(), TEXT("-authtoken="), serverParameters.m_authToken))
    {
        UE_LOG(LogCD_ServerLog, Log, TEXT("AUTH_TOKEN: %s"), *serverParameters.m_authToken)
    }

    if (FParse::Value(FCommandLine::Get(), TEXT("-awsregion="), serverParameters.m_awsRegion))
    {
        UE_LOG(LogCD_ServerLog, Log, TEXT("AWS_REGION: %s"), *serverParameters.m_awsRegion)
    }


    if (FParse::Value(FCommandLine::Get(), TEXT("-accesskey="), serverParameters.m_accessKey))
    {
        UE_LOG(LogCD_ServerLog, Log, TEXT("ACCESS_KEY: %s"), *serverParameters.m_accessKey)
    }
    if (FParse::Value(FCommandLine::Get(), TEXT("-secretkey="), serverParameters.m_secretKey))
    {
        UE_LOG(LogCD_ServerLog, Log, TEXT("SECRET_KEY: % s"), *serverParameters.m_secretKey)
    }
    if (FParse::Value(FCommandLine::Get(), TEXT("-sessiontoken="), serverParameters.m_sessionToken))
    {
        UE_LOG(LogCD_ServerLog, Log, TEXT("SESSION_TOKEN: %s"), *serverParameters.m_sessionToken)
    }

    //The Host/compute-name of the Amazon GameLift Servers Anywhere instance.
    if (FParse::Value(FCommandLine::Get(), TEXT("-hostid="), serverParameters.m_hostId))
    {
        UE_LOG(LogCD_ServerLog, Log, TEXT("HOST_ID: %s"), *serverParameters.m_hostId)
    }

    //The Anywhere Fleet ID.
    if (FParse::Value(FCommandLine::Get(), TEXT("-fleetid="), serverParameters.m_fleetId))
    {
        UE_LOG(LogCD_ServerLog, Log, TEXT("FLEET_ID: %s"), *serverParameters.m_fleetId)
    }

    //The WebSocket URL (GameLiftServiceSdkEndpoint).
    if (FParse::Value(FCommandLine::Get(), TEXT("-websocketurl="), serverParameters.m_webSocketUrl))
    {
        UE_LOG(LogCD_ServerLog, Log, TEXT("WEBSOCKET_URL: %s"), *serverParameters.m_webSocketUrl)
    }

    FString glProcessId = "";
    if (FParse::Value(FCommandLine::Get(), TEXT("-processId="), glProcessId))
    {
        serverParameters.m_processId = TCHAR_TO_UTF8(*glProcessId);
    }
    else
    {
        // If no ProcessId is passed as a command line argument, generate a randomized unique string.
        FString TimeString = FString::FromInt(std::time(nullptr));
        FString ProcessId = "ProcessId_" + TimeString;
        serverParameters.m_processId = TCHAR_TO_UTF8(*ProcessId);
    }
    //The PID of the running process
    UE_LOG(LogCD_ServerLog, Log, TEXT("PID: %s"), *serverParameters.m_processId);
}

void AServer_GameMode::TryAcceptPlayerSession(const FString& PlayerSessionId, const FString& Username,
    FString& ErrorMessage)
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

void AServer_GameMode::UpdatePlayersStatus()
{
    if (IsValid(GetGameState<AGameState>()) && IsValid(GetGameInstanceSubsystem()))
    {
        for (auto PS :  GetGameState<AGameState>()->PlayerArray)
        {
            if (!PS) continue;
            FString NetIdStr = PS->GetUniqueId().IsValid() ? PS->GetUniqueId()->ToString() : TEXT("Unknown");
            int32 Ping = FMath::RoundToInt(PS->ExactPing);
            FPlayerSessionInfoArray& InfoArray = GetGameInstanceSubsystem()->GetPlayerInfos();
            for (auto& Info : InfoArray.Items)
            {
                InfoArray.UpdatePing(NetIdStr, Ping);
            }
            
            ACDSessionPlayerState* SessionPlayerState = Cast<ACDSessionPlayerState>(PS);
            if (IsValid(SessionPlayerState))
            {
                SessionPlayerState->Client_ReceivePlayerInfos(InfoArray);
            }
        }
    }
}
