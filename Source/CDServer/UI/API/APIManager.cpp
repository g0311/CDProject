// Fill out your copyright notice in the Description page of Project Settings.


#include "APIManager.h"
#include "HttpModule.h"
#include "CDServer/Data/API/APIData.h"

void UAPIManager::ListFleetsButtonOnClicked()
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UAPIManager::ListFleets_Response);

	check(APIData);
	const FString APIUrl = APIData->GetAPIEndpoint(ServerTags::GameSessionAPI::ListFleets);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	//Request->SetContentAsString(); FOR INPUT
	
	Request->ProcessRequest();
}

void UAPIManager::ListFleets_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded)
{
	
}