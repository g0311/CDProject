// Fill out your copyright notice in the Description page of Project Settings.


#include "Server_GameMode.h"

#include "CDGameInstanceSubsystem.h"
#include "CDSessionGameState.h"
#include "CDServer/Player/CDPlayerStateStatsProvider.h"
#include "CDServer/Player/CDSessionPlayerController.h"
#include "CDServer/UI/GameSessions/GameSessionsManager.h"
#include "CDServer/UI/GameStats/GameStatsManager.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
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

    if (IsRunningDedicatedServer())
    {
        TryAcceptPlayerSession(PlayerSessionId, Username, ErrorMessage);
    }
}

APlayerController* AServer_GameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal,
                                           const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
    APlayerController* PlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
    
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
    FString Username = UGameplayStatics::ParseOption(Options, TEXT("Username"));
    FString PlayerSessionId = UGameplayStatics::ParseOption(Options, TEXT("PlayerSessionId"));

    if (PlayerSessionId.IsEmpty())
    {
        PlayerSessionId = FGuid::NewGuid().ToString();
    }
    if (Username.IsEmpty())
    {
        Username = FGuid::NewGuid().ToString();
    }
    
    if (ACDSessionGameState* SessionGameState = GetGameState<ACDSessionGameState>(); IsValid(SessionGameState))
    {
        SessionGameState->AddPlayerInfo(FPlayerSessionInfo(PlayerSessionId, Username, false, 0, NetIdStr, false));
        SessionGameState->GetPlayerInfos().Log();
        //하고 있던거 => 플레이어 스테이트에 이름이랑 팀 저장시키기
    }
    
    if (ACDSessionPlayerController* CDPC = Cast<ACDSessionPlayerController>(PlayerController); IsValid(CDPC))
    {
        CDPC->SetPlayerSessionId(PlayerSessionId);
    }
    
    return PlayerController;
}

void AServer_GameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    ACDSessionPlayerController* PlayerController = Cast<ACDSessionPlayerController>(Exiting);
    if (IsValid(PlayerController))
    {
        const FString PlayerSessionId = PlayerController->GetPlayerSessionId();
        if (ACDSessionGameState* SessionGameState = GetGameState<ACDSessionGameState>(); IsValid(SessionGameState))
        {
            SessionGameState->LeaveSession(PlayerSessionId);
        }
    }
    
    if (IsRunningDedicatedServer() && GetNumPlayers() == 0)
    {
        UE_LOG(LogCD_ServerLog, Warning, TEXT("Session Empty"));
        FGameLiftServerSDKModule* gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
        TerminateProcess(gameLiftSdkModule, 200);
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
        if (ACDSessionGameState* SessionGameState = GetGameState<ACDSessionGameState>(); IsValid(SessionGameState))
        {
            GameSessionManager->UpdateGameSession(SessionGameState->GetGameSessionId(), SessionGameState->GetRoomMap(), SessionGameState->GetRoomMode(), TEXT("true"));	
            SessionGameState->PushProperty();
            UWorld* World = GEngine->GetWorldContexts()[0].World();
            if (World)
            {
                FString url = TEXT("/Game/Maps/") + SessionGameState->GetRoomMode() + TEXT("/") + SessionGameState->GetRoomMap();
                UE_LOG(LogCD_ServerLog, Warning, TEXT("%s"), *url);
                GetWorld()->ServerTravel(url, false);
                GetWorld()->SeamlessTravel(url);
            }
        }
    }
}

void AServer_GameMode::EndGame(WinState winState)
{
    if (IsValid(GameStatsManager))
    {
        if (ACDSessionGameState* SessionGameState = GetGameState<ACDSessionGameState>(); IsValid(SessionGameState))
        {
            for (auto& player : SessionGameState->PlayerArray)
            {
                if (player->IsABot()) continue;
                if (ICDPlayerStateStatsProvider* CDPlayerState = Cast<ICDPlayerStateStatsProvider>(player))
                {
                    FCDRecordMatchStatsInput RecordMatchStatsInput;
                    FCDMatchStats MatchStats = CDPlayerState->GetPRecordInput();
                    FCDMatchData MatchData;
                    MatchData.Kill = MatchStats.Kill;
                    MatchData.Death = MatchStats.Death;
                    MatchData.Mode = SessionGameState->GetRoomMode();
                    MatchData.Map = SessionGameState->GetRoomMap();
                    if (winState == ATEAMWIN)
                    {
                        if (CDPlayerState->GetPTeam() == ETeam::ET_ATeam)
                        {
                            MatchStats.Totalwin = 1;
                            MatchData.Iswin = 1;
                        }
                        else
                        {
                            MatchStats.Totallose = 1;
                            MatchData.Iswin = -1;
                        }
                    }
                    else if (winState == ATEAMLOSE)
                    {
                        if (CDPlayerState->GetPTeam() == ETeam::ET_BTeam)
                        {
                            MatchStats.Totalwin = 1;
                            MatchData.Iswin = 1;
                        }
                        else
                        {
                            MatchStats.Totallose = 1;
                            MatchData.Iswin = -1;
                        }
                    }
                    else if (winState == DRAW)
                    {
                        MatchStats.Totaldraw = 1;
                        MatchData.Iswin = 0;
                    }
                    else
                    {
                        MatchStats.Totaldraw = 2;
                        MatchData.Iswin = 2;
                    }
                    RecordMatchStatsInput.Username = CDPlayerState->GetPUsername();
                    RecordMatchStatsInput.MatchData = MatchData;
                    RecordMatchStatsInput.MatchStats = MatchStats;
                    GameStatsManager->RecordMatchStats(RecordMatchStatsInput);
                }
            }
        }
    }
    if (IsValid(GameSessionManager))
    {
        if (ACDSessionGameState* SessionGameState = GetGameState<ACDSessionGameState>(); IsValid(SessionGameState))
        {
            if (SessionGameState->IsPrivate())
            {
                GameSessionManager->UpdateGameSession(SessionGameState->GetGameSessionId(), SessionGameState->GetRoomMap(), SessionGameState->GetRoomMode(), TEXT("false"));	
                SessionGameState->PushProperty();
                UWorld* World = GEngine->GetWorldContexts()[0].World();
                if (World)
                {
                    FString url = TEXT("/Game/Maps/ServerDefaultMap");
                    UE_LOG(LogCD_ServerLog, Warning, TEXT("%s"), *url);
                }
            }
            else
            {
                FGameLiftServerSDKModule* gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
                TerminateProcess(gameLiftSdkModule, 200);
            }
        }
    }
    else
    {
        FGameLiftServerSDKModule* gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
        TerminateProcess(gameLiftSdkModule, 200);
    }
}

void AServer_GameMode::KickPlayer(const FString& PlayerSessionId)
{
    if (!GetWorld()) return;
    for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        ACDSessionPlayerController* PC = Cast<ACDSessionPlayerController>(*It);
        if (PC && PC->GetPlayerSessionId() == PlayerSessionId)
        {
            if (UNetConnection* NetConnection = Cast<UNetConnection>(PC->GetNetConnection()))
            {
                NetConnection->Close();
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
    if (GameStatsManagerClass)
    {
        GameStatsManager = NewObject<UGameStatsManager>(this, GameStatsManagerClass);
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
