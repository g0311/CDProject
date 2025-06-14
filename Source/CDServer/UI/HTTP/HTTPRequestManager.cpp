// Fill out your copyright notice in the Description page of Project Settings.


#include "HTTPRequestManager.h"

#include "HTTPRequestTypes.h"
#include "CDServer/Player/CDLocalPlayerSubsystem.h"
#include "JsonObjectConverter.h"
#include "CDServer/Game/Server_GameMode.h"
#include "CDServer/UI/Portal/Interfaces/HUDManagement.h"

UCDLocalPlayerSubsystem* UHTTPRequestManager::GetCDLocalPlayerSubsystem()
{
	APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (IsValid(LocalPlayerController))
	{
		ULocalPlayer* LocalPlayer = LocalPlayerController->GetLocalPlayer();
		if (IsValid(LocalPlayer))
		{
			return LocalPlayer->GetSubsystem<UCDLocalPlayerSubsystem>();
		}
	}
	return nullptr;
}

FString UHTTPRequestManager::ContainsError(TSharedPtr<FJsonObject> JsonObject)
{
	if (JsonObject->HasField(TEXT("errorType"))|| JsonObject->HasField(TEXT("errorMessage")))
	{
		FString ErrorType = JsonObject->HasField(TEXT("errorType")) ? JsonObject->GetStringField(TEXT("errorType")) : TEXT("Unknown Error"); 
		FString ErrorMessage = JsonObject->HasField(TEXT("errorMessage")) ? JsonObject->GetStringField(TEXT("errorMessage")) : TEXT("Unknown Error Message");

		UE_LOG(LogCD_ServerLog, Error, TEXT("Error Type: %s"), *ErrorType);
		UE_LOG(LogCD_ServerLog, Error, TEXT("Error Message: %s"), *ErrorMessage);

		return ErrorType;
	}
	else if (JsonObject->HasField(TEXT("$fault")))
	{
		FString ErrorType = JsonObject->HasField(TEXT("name")) ? JsonObject->GetStringField(TEXT("name")) : TEXT("Unknown Error"); 
		UE_LOG(LogCD_ServerLog, Error, TEXT("Error Type: %s"), *ErrorType);

		return ErrorType;
	}
	
	return FString();
}

void UHTTPRequestManager::DumpMetaData(TSharedPtr<FJsonObject> JsonObject)
{
	if(JsonObject->HasField(TEXT("$metadata")))
	{
		TSharedPtr<FJsonObject> MetaDataJsonObject = JsonObject->GetObjectField(TEXT("$metadata"));
		FCDMetaData CDMetaData;
		FJsonObjectConverter::JsonObjectToUStruct(MetaDataJsonObject.ToSharedRef(), &CDMetaData);
		CDMetaData.Dump();
	}
}

FString UHTTPRequestManager::SerializeJsonContent(const TMap<FString, FString>& Params)
{
	TSharedPtr<FJsonObject> ContentJsonObject = MakeShareable(new FJsonObject());

	for (const auto& Param : Params)
	{
		ContentJsonObject->SetStringField(Param.Key, Param.Value);
	}
	
	FString Content;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Content);
	FJsonSerializer::Serialize(ContentJsonObject.ToSharedRef(), Writer);

	return Content;
}

void UHTTPRequestManager::OnSignOut()
{
	APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (IsValid(LocalPlayerController))
	{
		IHUDManagement* HUDManagementInterface = Cast<IHUDManagement>(LocalPlayerController->GetHUD());
		if (HUDManagementInterface)
		{
			HUDManagementInterface->OnSignOut();
		}
	}
}
