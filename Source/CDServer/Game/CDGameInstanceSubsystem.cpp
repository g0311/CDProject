// Fill out your copyright notice in the Description page of Project Settings.

#include "CDGameInstanceSubsystem.h"

#include "CDSessionGameState.h"
#include "Server_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY(CD_GAMEINSTANCE);

UCDGameInstanceSubsystem::UCDGameInstanceSubsystem()
{
    bGameLiftInitialized = false;
}

void UCDGameInstanceSubsystem::InitGameLift(const FServerParameters& ServerParameters)
{
    if (bGameLiftInitialized)
        return;

#if WITH_GAMELIFT
    UE_LOG(CD_GAMEINSTANCE, Log, TEXT("Initializing the GameLift Server"));

    //Getting the module first.
    FGameLiftServerSDKModule* gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));

    //InitSDK establishes a local connection with the Amazon GameLift Servers Agent to enable further communication.
    //Use InitSDK(serverParameters) for an Amazon GameLift Servers Anywhere fleet. 
    //Use InitSDK() for Amazon GameLift Servers managed EC2 fleet.
    gameLiftSdkModule->InitSDK(ServerParameters);

    //Implement callback function onStartGameSession
    //Amazon GameLift Servers sends a game session activation request to the game server
    //and passes a game session object with game properties and other settings.
    //Here is where a game server takes action based on the game session object.
    //When the game server is ready to receive incoming player connections, 
    //it invokes the server SDK call ActivateGameSession().
    auto onGameSession = [=, this](Aws::GameLift::Server::Model::GameSession gameSession)
    {
        GameSessionId = FString(gameSession.GetGameSessionId());
        UE_LOG(CD_GAMEINSTANCE, Log, TEXT("GameSession Initializing: %s"), *GameSessionId);

        int PropertyCount;
        const Aws::GameLift::Server::Model::GameProperty* gameProperties = gameSession.GetGameProperties(PropertyCount);
        
        for (int i = 0; i < PropertyCount; ++i)
        {
            const Aws::GameLift::Server::Model::GameProperty& property = gameProperties[i];
            FString key = FString(property.GetKey());
            FString value = FString(property.GetValue());

            if (key.Equals(TEXT("IsPrivate"), ESearchCase::IgnoreCase))
            {
                bIsPrivate = value;
            }
            if (key.Equals(TEXT("Mode"), ESearchCase::IgnoreCase))
            {
                RoomMode = value;
            }
            if (key.Equals(TEXT("Map"), ESearchCase::IgnoreCase))
            {
                RoomMap = value;
            }
        }
        
        if (GetWorld())
        {
            if (ACDSessionGameState* SessionGameState = GetWorld()->GetGameState<ACDSessionGameState>(); IsValid(SessionGameState))
            {
                SessionGameState->UpdateProperty(RoomMode,RoomMap,RoomName,bIsPrivate, GameSessionId);
            }
        }
        
        UWorld* World = GEngine->GetWorldContexts()[0].World();
        if (World)
        {
            // if (bIsPrivate.Equals(TEXT("false"), ESearchCase::IgnoreCase))
            // {
            //     AsyncTask(ENamedThreads::GameThread, [World = World, Url = FString(TEXT("/Game/Maps/") + RoomMode + TEXT("/") + RoomMap)]()
            //     {
            //         if (World)
            //         {
            //             UE_LOG(LogTemp, Log, TEXT("Opening Level: %s"), *Url);
            //             UGameplayStatics::OpenLevel(World, FName(*Url), true);
            //         }
            //     });
            //     //On Game Map Loaded, Call Activate
            //     //gameLiftSdkModule->ActivateGameSession();
            // }
            // else
            {
                AsyncTask(ENamedThreads::GameThread, [World, this]()
                {
                    if (World && IsValid(this))
                    {
                        World->GetTimerManager().SetTimer(this->ExitHandle, FTimerDelegate::CreateLambda([]()
                        {
                            UE_LOG(LogTemp, Warning, TEXT("Timer expired, shutting down server."));
                            FGameLiftServerSDKModule* gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));
                            gameLiftSdkModule->ProcessEnding();
                        }), 30.f, false);
                    }
                });
                gameLiftSdkModule->ActivateGameSession();
            }
        }
        else
        {
            UE_LOG(CD_GAMEINSTANCE, Log, TEXT("World Is Null!!!"));
        }
    };
    m_params.OnStartGameSession.BindLambda(onGameSession);

    //Implement callback function OnProcessTerminate
    //Amazon GameLift Servers invokes this callback before shutting down the instance hosting this game server.
    //It gives the game server a chance to save its state, communicate with services, etc., 
    //and initiate shut down. When the game server is ready to shut down, it invokes the 
    //server SDK call ProcessEnding() to tell Amazon GameLift Servers it is shutting down.
    auto onProcessTerminate = [=]()
    {
        UE_LOG(CD_GAMEINSTANCE, Log, TEXT("Game Server Process is terminating"));
        gameLiftSdkModule->ProcessEnding();
        FGenericPlatformMisc::RequestExit(false);
    };
    m_params.OnTerminate.BindLambda(onProcessTerminate);

    //Implement callback function OnHealthCheck
    //Amazon GameLift Servers invokes this callback approximately every 60 seconds.
    //A game server might want to check the health of dependencies, etc.
    //Then it returns health status true if healthy, false otherwise.
    //The game server must respond within 60 seconds, or Amazon GameLift Servers records 'false'.
    //In this example, the game server always reports healthy.
    auto onHealthCheck = []()
    {
        UE_LOG(CD_GAMEINSTANCE, Log, TEXT("Performing Health Check"));
        return true;
    };
    m_params.OnHealthCheck.BindLambda(onHealthCheck);

    //The game server gets ready to report that it is ready to host game sessions
    //and that it will listen on port 7777 for incoming player connections.
    int32 port = FURL::UrlConfig.DefaultPort;
    ParseCommandLienPort(port);
    m_params.port = port;

    //Here, the game server tells Amazon GameLift Servers where to find game session log files.
    //At the end of a game session, Amazon GameLift Servers uploads everything in the specified 
    //location and stores it in the cloud for access later.
    TArray<FString> logfiles;
    logfiles.Add(TEXT("CDProject/Saved/Logs/CDProject.log"));
    m_params.logParameters = logfiles;

    //The game server calls ProcessReady() to tell Amazon GameLift Servers it's ready to host game sessions.
    UE_LOG(CD_GAMEINSTANCE, Log, TEXT("Calling Process Ready"));
    gameLiftSdkModule->ProcessReady(m_params);
#endif
    bGameLiftInitialized = true;
}

void UCDGameInstanceSubsystem::ParseCommandLienPort(int32& outPort)
{
    FParse::Value(FCommandLine::Get(), TEXT("-port="), outPort);
}
