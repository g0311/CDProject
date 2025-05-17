#pragma once

#include "CoreMinimal.h"
#include "HTTPRequestTypes.generated.h"

namespace HTTPStatusMessages
{
    extern CDSERVER_API const FString SomethingWentWrong;
}

USTRUCT()
struct FCDMetaData
{
    GENERATED_BODY()
    
    UPROPERTY()
    int32 httpStatusCode{};
    UPROPERTY()
    FString requestId{};
    UPROPERTY()
    int32 attempts{};
    UPROPERTY()
    double totalRetryDelay{};
    
    void Dump()const;  
};

USTRUCT()
struct FCDListFleetsResponse
{
    GENERATED_BODY()
    
    UPROPERTY()
    TArray<FString> FleetIds{};
    UPROPERTY()
    FString NextToken{};
    
    void Dump()const;  
};

USTRUCT()
struct FCDGameSession
{
    GENERATED_BODY()
    
    UPROPERTY()
    double CreationTime{};
    UPROPERTY()
    FString CreatorId{};
    UPROPERTY()
    FString CurrentPlayerSessionCount{};
    UPROPERTY()
    FString DnsName{};
    UPROPERTY()
    FString FleetArn{};
    UPROPERTY()
    FString FleetId{};
    UPROPERTY()
    TMap<FString, FString> GameProperties{};
    UPROPERTY()
    FString GameSessionData{};
    UPROPERTY()
    FString GameSessionId{};
    UPROPERTY()
    FString IpAddress{};
    UPROPERTY()
    FString Location{};
    UPROPERTY()
    FString MatchmakerData{};
    UPROPERTY()
    int32 MaximumPlayerSessionCount{};
    UPROPERTY()
    FString Name{};
    UPROPERTY()
    FString PlayerSessionCreationPolicy{};
    UPROPERTY()
    int32 Port{};
    UPROPERTY()
    FString Status{};
    UPROPERTY()
    FString StatusReason{};
    UPROPERTY()
    double TerminationTime{};
    
    void Dump()const;  
};
