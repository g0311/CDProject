// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CDServer/UI/HTTP/HTTPRequestManager.h"
#include "Interfaces/IHttpRequest.h"
#include "APIManager.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API UAPIManager : public UHTTPRequestManager
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void ListFleetsButtonOnClicked();

private:
	void ListFleets_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded);
};
