// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalManager.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "CDServer/Data/API/APIData.h"
#include "CDServer/UI/APITest/APITestManager.h"
#include "CDServer/UI/HTTP/HTTPRequestTypes.h"
#include "Interfaces/IHttpResponse.h"

void UPortalManager::JoinGameSession()
{
	BroadcastJoinGameSessionMessage.Broadcast(TEXT("Searching For Game Session..."), false);

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UPortalManager::FindOrCreateGameSession_Response);

	check(APIData);
	const FString APIUrl = APIData->GetAPIEndpoint(ServerTags::GameSessionAPI::FindOrCreateGameSession);
	Request->SetURL(APIUrl);
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");

	Request->ProcessRequest();
}

void UPortalManager::FindOrCreateGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bSucceeded)
{
	if (!bSucceeded)
	{
		BroadcastJoinGameSessionMessage.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsError(JsonObject))
		{
			BroadcastJoinGameSessionMessage.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
			return;
		}
		DumpMetaData(JsonObject);

		FCDGameSession GameSession;
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &GameSession);
		GameSession.Dump();
		BroadcastJoinGameSessionMessage.Broadcast(TEXT("Found Game Session"), false);
	}
}
