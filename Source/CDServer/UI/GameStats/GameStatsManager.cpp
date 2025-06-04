// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStatsManager.h"

#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "CDProject/PlayerState/CDPlayerState.h"
#include "CDServer/Data/API/APIData.h"
#include "CDServer/Game/Server_GameMode.h"
#include "CDServer/UI/HTTP/HTTPRequestManager.h"
#include "CDServer/UI/HTTP/HTTPRequestTypes.h"
#include "Interfaces/IHttpRequest.h"
#include "CDServer/GameplayTags/ServerTags.h"
#include "CDServer/Player/CDLocalPlayerSubsystem.h"
#include "CDServer/Player/CDSessionPlayerController.h"
#include "Interfaces/IHttpResponse.h"

void UGameStatsManager::RecordMatchStats(const FCDRecordMatchStatsInput& RecordMatchStatsInput)
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

	check(APIData);
	const FString APIUrl = APIData->GetAPIEndpoint(ServerTags::GameStatsAPI::RecordMatchStats);
	Request->SetURL(APIUrl);
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");

	// TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	// FJsonObjectConverter::UStructToJsonObject(FCDRecordMatchStatsInput::StaticStruct(), &RecordMatchStatsInput, JsonObject.ToSharedRef());

	FString JsonString;
	FJsonObjectConverter::UStructToJsonObjectString(FCDRecordMatchStatsInput::StaticStruct(), &RecordMatchStatsInput, JsonString);
	Request->SetContentAsString(JsonString);
	
	Request->ProcessRequest();
}

void UGameStatsManager::RetrieveMatchStats()
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UGameStatsManager::RetrieveMatchStats_Response);

	check(APIData);
	const FString APIUrl = APIData->GetAPIEndpoint(ServerTags::GameStatsAPI::RetrieveMatchStats);
	Request->SetURL(APIUrl);
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");

	UCDLocalPlayerSubsystem* LocalPlayerSubsystem = GetCDLocalPlayerSubsystem();
	if (!IsValid(LocalPlayerSubsystem))
		return;
	
	TMap<FString, FString> Params =
		{
		{TEXT("accessToken"), LocalPlayerSubsystem->GetAuthResult().AccessToken }
		};
	const FString& Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	
	Request->ProcessRequest();
}

void UGameStatsManager::RecordMatchStat_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded)
{
	if (!bSucceeded)
	{
		UE_LOG(LogCD_ServerLog, Error, TEXT("Record Match Stats Failed"));
	}
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		ContainsError(JsonObject);
	}
}

void UGameStatsManager::RetrieveMatchStats_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded)
{
	if (!bSucceeded)
	{
		UE_LOG(LogCD_ServerLog, Error, TEXT("Retrieve Match Stat Failed"));
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		ContainsError(JsonObject);
		
		FCDRetrieveMatchStatsResult PlayerMatchStats;
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &PlayerMatchStats);

		OnRetrieveMatchStats.Broadcast(PlayerMatchStats);
	}
}
