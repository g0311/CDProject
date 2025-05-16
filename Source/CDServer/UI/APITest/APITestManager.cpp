// Fill out your copyright notice in the Description page of Project Settings.


#include "APITestManager.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "CDServer/Data/API/APIData.h"
#include "CDServer/Game/Server_GameMode.h"
#include "CDServer/UI/HTTP/HTTPRequestTypes.h"
#include "Interfaces/IHttpResponse.h"

void UAPITestManager::ListFleetsButtonOnClicked()
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UAPITestManager::ListFleets_Response);

	check(APIData);
	const FString APIUrl = APIData->GetAPIEndpoint(ServerTags::GameSessionAPI::ListFleets);
	Request->SetURL(APIUrl);
	Request->SetVerb(TEXT("GET"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	//Request->SetContentAsString(); FOR INPUT
	
	Request->ProcessRequest();
}

void UAPITestManager::ListFleets_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded)
{
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsError(JsonObject))
		{
			OnListFleetsResponseReceived.Broadcast(FCDListFleetsResponse(), false);
			return;
		}
		DumpMetaData(JsonObject);

		FCDListFleetsResponse CDListFleetsResponse;
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &CDListFleetsResponse);
		CDListFleetsResponse.Dump();
		OnListFleetsResponseReceived.Broadcast(CDListFleetsResponse, true);
	}
}
