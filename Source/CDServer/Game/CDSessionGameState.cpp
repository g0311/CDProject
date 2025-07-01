// Fill out your copyright notice in the Description page of Project Settings.


#include "CDSessionGameState.h"
#include "CDGameInstanceSubsystem.h"
#include "Server_GameMode.h"
#include "CDServer/Player/CDPlayerStateStatsProvider.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

ACDSessionGameState::ACDSessionGameState()
{
}

void ACDSessionGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ACDSessionGameState, PlayerInfos);
    DOREPLIFETIME(ACDSessionGameState, RoomMode);
    DOREPLIFETIME(ACDSessionGameState, RoomMap);
    DOREPLIFETIME(ACDSessionGameState, RoomName);
}

void ACDSessionGameState::AddPlayerInfo(FPlayerSessionInfo playerInfo)
{
    if (PlayerInfos.Items.IsEmpty())
    {
        playerInfo.bIsHost = true;
    }
    PlayerInfos.AddPlayer(playerInfo);
    SetPlayerStateInfos(playerInfo);
}

void ACDSessionGameState::RemovePlayerInfo(const FString& PlayerSessionId)
{
    for (auto& playerInfo : PlayerInfos.Items)
    {
        if (PlayerSessionId == playerInfo.PlayerSessionId)
        {
            PlayerInfos.RemovePlayer(playerInfo);
            break;
        }
    }
}

FPlayerSessionInfoArray& ACDSessionGameState::GetPlayerInfos()
{
    return PlayerInfos;
}

const FString& ACDSessionGameState::GetRoomMode()
{
    return RoomMode;
}

const FString& ACDSessionGameState::GetRoomMap()
{
    return RoomMap;
}

const FString& ACDSessionGameState::GetRoomName()
{
    return RoomName;
}

void ACDSessionGameState::SetRoomMode(const FString& PlayerSessionId, const FString& NextRoomMode)
{
    if (!PlayerInfos.Items.IsEmpty() && PlayerSessionId == PlayerInfos.Items[0].PlayerSessionId)
    {
        this->RoomMode = NextRoomMode;
    }
}

void ACDSessionGameState::SetRoomMap(const FString& PlayerSessionId, const FString& NextRoomMap)
{
    if (!PlayerInfos.Items.IsEmpty() && PlayerSessionId == PlayerInfos.Items[0].PlayerSessionId)
    {
        this->RoomMap = NextRoomMap;
    }
}

const FString& ACDSessionGameState::GetGameSessionId()
{
    return GameSessionId;
}

bool ACDSessionGameState::IsPrivate()
{
    if (bIsPrivate == TEXT("true"))
        return true;

    return false;
}

void ACDSessionGameState::LeaveSession(const FString& PlayerSessionId)
{
#if WITH_GAMELIFT
    Aws::GameLift::Server::RemovePlayerSession(TCHAR_TO_UTF8(*PlayerSessionId));
#endif
    RemovePlayerInfo(PlayerSessionId);
}

void ACDSessionGameState::PlayerReady(const FString& PlayerSessionId, bool ShouldReset)
{
    if (PlayerInfos.IsPlayerHost(PlayerSessionId))
    {
        UE_LOG(LogCD_ServerLog, Warning, TEXT("Server_PlayerReady: Player Is Host"));
        if (PlayerInfos.IsAllPlayerReady())
        {
            AServer_GameMode* Server_GameMode = Cast<AServer_GameMode>(GetWorld()->GetAuthGameMode());
            if (IsValid(Server_GameMode))
            {
                Server_GameMode->StartGame();	
            }
        }
    }
    else
    {
        UE_LOG(LogCD_ServerLog, Warning, TEXT("Server_PlayerReady: Player Is Not Host"));
        PlayerInfos.UpdatePlayerReadyState(PlayerSessionId, ShouldReset);
        PlayerInfos.Log();
    }
}

void ACDSessionGameState::ChangeTeam(const FString& PlayerSessionId, bool IsATeam)
{
    TSet<int32> UsedIndices;
    for (const FPlayerSessionInfo& Existing : PlayerInfos.Items)
    {
        UsedIndices.Add(Existing.Index);
    }
    int32 NewIndex = -1;

    int32 StartIndex = IsATeam ? 0 : 3;
    int32 EndIndex   = IsATeam ? 2 : 5;

    FPlayerSessionInfo* TargetInfo = PlayerInfos.Items.FindByPredicate(
        [&](const FPlayerSessionInfo& Info) { return Info.PlayerSessionId == PlayerSessionId; });

    if (!TargetInfo)
    {
        UE_LOG(LogTemp, Warning, TEXT("ChangeTeam: Player not found: %s"), *PlayerSessionId);
        return;
    }

    // 이미 같은 팀이면 무시
    if ((IsATeam && TargetInfo->Index <= 2) || (!IsATeam && TargetInfo->Index >= 3))
    {
        UE_LOG(LogTemp, Log, TEXT("ChangeTeam: Already in desired team."));
        return;
    }

    // 해당 팀 내 빈 인덱스 찾기
    for (int32 i = StartIndex; i <= EndIndex; ++i)
    {
        if (!UsedIndices.Contains(i))
        {
            NewIndex = i;
            break;
        }
    }

    if (NewIndex == -1)
    {
        UE_LOG(LogTemp, Warning, TEXT("ChangeTeam: No available slot in desired team."));
        return;
    }

    TargetInfo->Index = NewIndex;
    
    SetPlayerStateInfos(*TargetInfo);
    
    UE_LOG(LogTemp, Log, TEXT("ChangeTeam: Player %s moved to %s team at index %d"),
        *PlayerSessionId, IsATeam ? TEXT("Red") : TEXT("Blue"), NewIndex);
}

void ACDSessionGameState::UpdateProperty(FString Mode, FString Map, FString Name, FString Private, FString SessionId)
{
    RoomMode = Mode;
    RoomMap = Map;
    RoomName = Name;
    bIsPrivate = Private;
    GameSessionId = SessionId;
}

void ACDSessionGameState::BeginPlay()
{
    Super::BeginPlay();
    
    if (IsRunningDedicatedServer() && HasAuthority() && IsValid(GetGameInstance()))
    {
        UCDGameInstanceSubsystem* GameInstanceSubsystem = GetGameInstance()->GetSubsystem<UCDGameInstanceSubsystem>();
        if (IsValid(GameInstanceSubsystem))
        {
            PlayerInfos = GameInstanceSubsystem->PlayerInfos;
            RoomMode = GameInstanceSubsystem->RoomMode;
            RoomMap = GameInstanceSubsystem->RoomMap;
            RoomName = GameInstanceSubsystem->RoomName;
            bIsPrivate = GameInstanceSubsystem->bIsPrivate;
            GameSessionId = GameInstanceSubsystem->GameSessionId;
        }
    }
}

void ACDSessionGameState::PushProperty()
{
    if (HasAuthority() && IsValid(GetGameInstance()))
    {
        UCDGameInstanceSubsystem* GameInstanceSubsystem = GetGameInstance()->GetSubsystem<UCDGameInstanceSubsystem>();
        if (IsValid(GameInstanceSubsystem))
        {
            GameInstanceSubsystem->PlayerInfos = PlayerInfos;
            GameInstanceSubsystem->RoomMode = RoomMode;
            GameInstanceSubsystem->RoomMap = RoomMap;
            GameInstanceSubsystem->RoomName = RoomName;
            GameInstanceSubsystem->bIsPrivate = bIsPrivate;
            GameInstanceSubsystem->GameSessionId = GameSessionId;
        }
    }
}

APlayerState* ACDSessionGameState::GetPlayerState(const FString& PlayerSessionId)
{
    for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
    {
        ACDSessionPlayerController* PC = Cast<ACDSessionPlayerController>(*It);
        if (PC && PC->GetPlayerSessionId() == PlayerSessionId)
        {
            return PC->PlayerState;
        }
    }
    return nullptr;
}

void ACDSessionGameState::SetPlayerStateInfos(const FPlayerSessionInfo& playerInfo)
{
    APlayerState* PS = GetPlayerState(playerInfo.PlayerSessionId);
    ICDPlayerStateStatsProvider* CDPlayerStateStatsProvider = Cast<ICDPlayerStateStatsProvider>(PS);
    if (CDPlayerStateStatsProvider)
    {
        if(PlayerInfos.IsPlayerATeam(playerInfo.PlayerSessionId))
        {
            CDPlayerStateStatsProvider->SetPTeam(ETeam::ET_ATeam);
        }
        else
        {
            CDPlayerStateStatsProvider->SetPTeam(ETeam::ET_BTeam);
        }
        CDPlayerStateStatsProvider->SetPName(playerInfo.Username);
    }
}