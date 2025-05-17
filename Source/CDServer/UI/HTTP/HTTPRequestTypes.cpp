#include "CDServer/UI/HTTP/HTTPRequestTypes.h"
#include "CDServer/CDServer.h"

namespace HTTPStatusMessages
{
	const FString SomethingWentWrong{TEXT("Something Went Wrong!!")};
}

void FCDMetaData::Dump()const
{
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("==== MetaData Dump ===="));
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("httpStatusCode: %d"), httpStatusCode);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("requestId: %s"), *requestId);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("attempts: %d"), attempts);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("totalRetryDelay: %f"), totalRetryDelay);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("==========================="));
}

void FCDListFleetsResponse::Dump() const
{
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("==== ListFleetResponse Dump ===="));
	for (const FString& fleetId : FleetIds)
	{
		UE_LOG(LogCD_DedicatedServer, Log, TEXT("fleetId: %s"), *fleetId);
	}
	if (!NextToken.IsEmpty())
	{
		UE_LOG(LogCD_DedicatedServer, Log, TEXT("NextToken: %s"), *NextToken);
	}
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("==========================="));
}

void FCDGameSession::Dump() const
{
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("==== GameSession Dump ===="));
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("GameSessionId: %s"), *GameSessionId);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("Name: %s"), *Name);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("CreatorId: %s"), *CreatorId);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("Status: %s"), *Status);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("StatusReason: %s"), *StatusReason);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("PlayerSessionCreationPolicy: %s"), *PlayerSessionCreationPolicy);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("CurrentPlayerSessionCount: %s"), *CurrentPlayerSessionCount);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("MaximumPlayerSessionCount: %d"), MaximumPlayerSessionCount);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("IpAddress: %s"), *IpAddress);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("DnsName: %s"), *DnsName);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("Port: %d"), Port);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("FleetId: %s"), *FleetId);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("FleetArn: %s"), *FleetArn);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("Location: %s"), *Location);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("GameSessionData: %s"), *GameSessionData);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("MatchmakerData: %s"), *MatchmakerData);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("CreationTime: %s"), *CreationTime);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("TerminationTime: %s"), *TerminationTime);

	UE_LOG(LogCD_DedicatedServer, Log, TEXT("GameProperties:"));
	for (const auto& Pair : GameProperties)
	{
		UE_LOG(LogCD_DedicatedServer, Log, TEXT("  %s = %s"), *Pair.Key, *Pair.Value);
	}
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("=========================="));
}

void FCDPlayerSession::Dump() const
{
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("==== PlayerSession Dump ===="));
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("PlayerSessionId: %s"), *PlayerSessionId);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("PlayerId: %s"), *PlayerId);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("GameSessionId: %s"), *GameSessionId);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("Status: %s"), *Status);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("PlayerData: %s"), *PlayerData);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("IpAddress: %s"), *IpAddress);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("DnsName: %s"), *DnsName);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("Port: %d"), Port);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("FleetId: %s"), *FleetId);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("FleetArn: %s"), *FleetArn);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("CreationTime: %s"), *CreationTime);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("TerminationTime: %s"), *TerminationTime);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("==========================="));
}
