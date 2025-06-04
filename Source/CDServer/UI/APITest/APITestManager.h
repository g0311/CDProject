// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CDServer/UI/HTTP/HTTPRequestManager.h"
#include "Interfaces/IHttpRequest.h"
#include "APITestManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnListFleetsResponseReceived, const FCDListFleetsResponse&, ListFleetsResponse, bool, bWasSuccessed);

UCLASS()
class CDSERVER_API UAPITestManager : public UHTTPRequestManager
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void ListFleetsButtonOnClicked();

	UPROPERTY()
	FOnListFleetsResponseReceived OnListFleetsResponseReceived;

private:
	void ListFleets_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded);
};
