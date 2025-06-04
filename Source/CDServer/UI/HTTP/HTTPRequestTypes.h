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
struct FCDGameProperty
{
    GENERATED_BODY()

    UPROPERTY()
    FString Key{};
    UPROPERTY()
    FString Value{};
};

USTRUCT()
struct FCDGameSession
{
    GENERATED_BODY()
    
    UPROPERTY()
    FString CreationTime{};
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
    TArray<FCDGameProperty> GameProperties;
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
    FString MaximumPlayerSessionCount{};
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
    FString TerminationTime{};
    
    void Dump()const;  
};


USTRUCT()
struct FCDPlayerSession
{
    GENERATED_BODY()
    
    UPROPERTY()
    FString CreationTime{};
    UPROPERTY()
    FString DnsName{};
    UPROPERTY()
    FString FleetArn{};
    UPROPERTY()
    FString FleetId{};
    UPROPERTY()
    FString GameSessionId{};
    UPROPERTY()
    FString IpAddress{};
    UPROPERTY()
    FString PlayerData{};
    UPROPERTY()
    FString PlayerId{};
    UPROPERTY()
    FString PlayerSessionId{};
    UPROPERTY()
    int32 Port{};
    UPROPERTY()
    FString Status{};
    UPROPERTY()
    FString TerminationTime{};
    
    void Dump()const;  
};

USTRUCT()
struct FCDCodeDeliveryDetails
{
    GENERATED_BODY()

    UPROPERTY()
    FString AttributeName{};
    UPROPERTY()
    FString DeliveryMedium{};
    UPROPERTY()
    FString Destination{};

    void Dump()const;  
};

USTRUCT()
struct FCDSignUpResponse
{
    GENERATED_BODY()

    UPROPERTY()
    FCDCodeDeliveryDetails CodeDeliveryDetails{};
    UPROPERTY()
    FString Session{};
    UPROPERTY()
    bool UserConfirmed{};
    UPROPERTY()
    FString UserSub{};
    
    void Dump()const;  
};

USTRUCT()
struct FCDNewDeviceMetadata
{
    GENERATED_BODY()

    UPROPERTY()
    FString DeviceGroupKey{};
    UPROPERTY()
    FString DeviceKey{};

    void Dump() const;
};

USTRUCT()
struct FCDAuthenticationResult
{
    GENERATED_BODY()

    UPROPERTY()
    FString AccessToken{};
    UPROPERTY()
    int32 ExpiresIn{};
    UPROPERTY()
    FString IdToken{};
    UPROPERTY()
    FCDNewDeviceMetadata NewDeviceMetadata{};
    UPROPERTY()
    FString RefreshToken{};
    UPROPERTY()
    FString TokenType{};

    void Dump() const;
};

USTRUCT()
struct FCDInitiateAuthResponse
{
    GENERATED_BODY()

    UPROPERTY()
    FCDAuthenticationResult AuthenticationResult{};
    UPROPERTY()
    TArray<FString> AvailableChallenges{};
    UPROPERTY()
    FString ChallengeName{};
    UPROPERTY()
    TMap<FString, FString> ChallengeParameters{};
    UPROPERTY()
    FString Session{};
    UPROPERTY()
    FString Email{};

    void Dump() const;
};

USTRUCT()
struct FCDDescribeGameSessionResult
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FCDGameSession> GameSessions{};

    UPROPERTY()
    FString NextToken{};

    void Dump() const;
};


USTRUCT()
struct FCDMatchData
{
    GENERATED_BODY()

    UPROPERTY()
    int32 Kill{};
    UPROPERTY()
    int32 Death{};
    UPROPERTY()
    int32 Iswin{};
    UPROPERTY()
    FString Mode{};
    UPROPERTY()
    FString Map{};
};


USTRUCT()
struct FCDMatchStats
{
    GENERATED_BODY()
    
    UPROPERTY()
    int32 Kill{};
    UPROPERTY()
    int32 Death{};
    UPROPERTY()
    int32 shot{};
    UPROPERTY()
    int32 Headshot{};
    UPROPERTY()
    int32 Totalwin{};
    UPROPERTY()
    int32 Totaldraw{};
    UPROPERTY()
    int32 Totallose{};
};

USTRUCT()
struct FCDRecordMatchStatsInput
{
    GENERATED_BODY()

    UPROPERTY()
    FString Username{};
    UPROPERTY()
    FCDMatchStats MatchStats{};
    UPROPERTY()
    FCDMatchData MatchData{};
};

USTRUCT()
struct FCDRetrieveMatchStatsResult
{
    GENERATED_BODY()

    UPROPERTY()
    FString Username{};
    UPROPERTY()
    int32 Kill{};
    UPROPERTY()
    int32 Death{};
    UPROPERTY()
    int32 shot{};
    UPROPERTY()
    int32 Headshot{};
    UPROPERTY()
    int32 Totalwin{};
    UPROPERTY()
    int32 Totaldraw{};
    UPROPERTY()
    int32 Totallose{};
    UPROPERTY()
    TArray<FCDMatchData> Matches{};
};
