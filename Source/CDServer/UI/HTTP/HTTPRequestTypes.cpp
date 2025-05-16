#include "CDServer/UI/HTTP/HTTPRequestTypes.h"
#include "CDServer/CDServer.h"

void FCDMetaData::Dump()const
{
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("MetaData: "));
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("httpStatusCode: %d"), httpStatusCode);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("requestId: %s"), *requestId);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("attempts: %d"), attempts);
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("totalRetryDelay: %f"), totalRetryDelay);
}

void FCDListFleetsResponse::Dump() const
{
	UE_LOG(LogCD_DedicatedServer, Log, TEXT("ListFleetResponse: "));
	for (const FString& fleetId : FleetIds)
	{
		UE_LOG(LogCD_DedicatedServer, Log, TEXT("fleetId: %s"), *fleetId);
	}
	if (!NextToken.IsEmpty())
	{
		UE_LOG(LogCD_DedicatedServer, Log, TEXT("NextToken: %s"), *NextToken);
	}
}
