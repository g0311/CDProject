#pragma once

#include "CoreMinimal.h"
#include "Team.h"
#include "CDServer/UI/HTTP/HTTPRequestTypes.h"

#include "CDPlayerStateStatsProvider.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UCDPlayerStateStatsProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CDSERVER_API ICDPlayerStateStatsProvider
{
	GENERATED_BODY()
	
public:
	virtual FCDMatchStats GetPRecordInput() const = 0;
	virtual ETeam GetPTeam() const = 0;
	virtual FString GetPUsername() const = 0;
	virtual void SetPTeam(ETeam team) = 0;
	virtual void SetPName(const FString& name) = 0;
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};
