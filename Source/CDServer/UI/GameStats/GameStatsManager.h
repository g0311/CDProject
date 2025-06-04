// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CDServer/UI/HTTP/HTTPRequestManager.h"
#include "CDServer/UI/HTTP/HTTPRequestTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "GameStatsManager.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRetrieveMatchStats, const FCDRetrieveMatchStatsResult&, result);

UCLASS()
class CDSERVER_API UGameStatsManager : public UHTTPRequestManager
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void RecordMatchStats(const FCDRecordMatchStatsInput& RecordMatchStatsInput);
	UFUNCTION()
	void RetrieveMatchStats();

	FOnRetrieveMatchStats OnRetrieveMatchStats;
private:
	void RecordMatchStat_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded);
	void RetrieveMatchStats_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded);

	
};
