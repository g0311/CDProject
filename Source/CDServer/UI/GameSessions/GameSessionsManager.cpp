// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSessionsManager.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "CDServer/Data/API/APIData.h"
#include "CDServer/Player/CDLocalPlayerSubsystem.h"
#include "CDServer/UI/HTTP/HTTPRequestTypes.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"

void UGameSessionsManager::JoinGameSession(const FString& GameMode)
{
	JoinGameSessionMessageDelegate.Broadcast(TEXT("Searching For Game Session..."), false);

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UGameSessionsManager::FindOrCreateGameSession_Response);

	check(APIData);
	const FString APIUrl = APIData->GetAPIEndpoint(ServerTags::GameSessionAPI::FindOrCreateGameSession);
	Request->SetURL(APIUrl);
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");

	UCDLocalPlayerSubsystem* LocalPlayerSubsystem = GetCDLocalPlayerSubsystem();
	if (IsValid(LocalPlayerSubsystem))
	{
		Request->SetHeader("Authorization", LocalPlayerSubsystem->GetAuthResult().AccessToken);	
	}
	TMap<FString, FString> Params =
		{
		{TEXT("isPrivate"), TEXT("false")},
		{TEXT("map"), TEXT("Default")},
		{TEXT("gameMode"), GameMode},
		};
	const FString& Content = SerializeJsonContent(Params);
	
	Request->ProcessRequest();
}

void UGameSessionsManager::FindGameSessions()
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UGameSessionsManager::GetGameSessions_Response);

	check(APIData);
	const FString APIUrl = APIData->GetAPIEndpoint(ServerTags::GameSessionAPI::FindGameSessions);
	Request->SetURL(APIUrl);
	Request->SetVerb("GET");
	Request->SetHeader("Content-Type", "application/json");

	UCDLocalPlayerSubsystem* LocalPlayerSubsystem = GetCDLocalPlayerSubsystem();
	if (IsValid(LocalPlayerSubsystem))
	{
		Request->SetHeader("Authorization", LocalPlayerSubsystem->GetAuthResult().AccessToken);	
	}

	Request->ProcessRequest();
}

void UGameSessionsManager::CreatePrivateGameSession(const FString& RoomName, const FString& RoomMode,
	const FString& RoomMap)
{
	PrivateSessionDelegate.Broadcast(TEXT("Creating Game Session..."), false);

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UGameSessionsManager::FindOrCreateGameSession_Response);

	check(APIData);
	const FString APIUrl = APIData->GetAPIEndpoint(ServerTags::GameSessionAPI::FindOrCreateGameSession);
	Request->SetURL(APIUrl);
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");

	UCDLocalPlayerSubsystem* LocalPlayerSubsystem = GetCDLocalPlayerSubsystem();
	if (IsValid(LocalPlayerSubsystem))
	{
		Request->SetHeader("Authorization", LocalPlayerSubsystem->GetAuthResult().AccessToken);	
	}
	TMap<FString, FString> Params =
		{
		{TEXT("name"), RoomName},
		{TEXT("isPrivate"), TEXT("true")},
		{TEXT("isStarted"), TEXT("false")},
		{TEXT("map"), TEXT("Default")},
		{TEXT("gameMode"), RoomMode},
		};
	const FString& Content = SerializeJsonContent(Params);
	
	Request->ProcessRequest();
}

void UGameSessionsManager::JoinPrivateGameSession(const FString& GameSessionId)
{
	UCDLocalPlayerSubsystem* LocalPlayerSubsystem = GetCDLocalPlayerSubsystem();
	if (IsValid(LocalPlayerSubsystem))
	{
		TryCreatePlayerSession(LocalPlayerSubsystem->Username, GameSessionId);
	}
	else
	{
		PrivateSessionDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
	}
}


void UGameSessionsManager::FindOrCreateGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response,
                                                            bool bSucceeded)
{
	if (!bSucceeded)
	{
		JoinGameSessionMessageDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
		PrivateSessionCreateDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
		return;
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsError(JsonObject))
		{
			JoinGameSessionMessageDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
			PrivateSessionCreateDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
			return;
		}
		//DumpMetaData(JsonObject);

		FCDGameSession GameSession;
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &GameSession);
		//GameSession.Dump();

		const FString GameSessionId = GameSession.GameSessionId;
		const FString GameSessionStatus = GameSession.Status;
		const FString GameMode = GameSession.GameProperties["GameMode"];
		HandleGameSessionStatus(GameSessionStatus, GameSessionId, GameMode);
	}
}

FString UGameSessionsManager::GetUniquePlayerId()
{
	APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (IsValid(LocalPlayerController))
	{
		APlayerState* LocalPlayerState = LocalPlayerController->GetPlayerState<APlayerState>();
		if (IsValid(LocalPlayerState) && LocalPlayerState->GetUniqueId().IsValid())
		{
			return FString::FromInt(LocalPlayerState->GetUniqueID());
		}
	}
	return FString();
}

void UGameSessionsManager::HandleGameSessionStatus(const FString& Status, const FString& SessionId, const FString& GameMode)
{
	if (Status.Equals(TEXT("ACTIVE")))
	{
		JoinGameSessionMessageDelegate.Broadcast(TEXT("Found Active Game Session"), false);

		UCDLocalPlayerSubsystem* LocalPlayerSubsystem = GetCDLocalPlayerSubsystem();
		if (IsValid(LocalPlayerSubsystem))
		{
			TryCreatePlayerSession(LocalPlayerSubsystem->Username, SessionId);
		}
	}
	else if (Status.Equals(TEXT("ACTIVATING")))
	{
		FTimerDelegate CreatePlayerSessionDelegate;
		CreatePlayerSessionDelegate.BindLambda([this, GameMode]()
		{
			if (IsValid(this))
				JoinGameSession(GameMode);
		});
		APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
		if (IsValid(LocalPlayerController))
		{
			LocalPlayerController->GetWorldTimerManager().SetTimer(CreatePlayerSessionTimer, CreatePlayerSessionDelegate,0.5f, false);
		}
	}
	else
	{
		JoinGameSessionMessageDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
	}
}

void UGameSessionsManager::TryCreatePlayerSession(const FString& PlayerId, const FString& GameSessionId)
{
	PrivateSessionDelegate.Broadcast(TEXT("Joining Game Session..."), false);
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UGameSessionsManager::CreatePlayerSession_Response);

	check(APIData);
	const FString APIUrl = APIData->GetAPIEndpoint(ServerTags::GameSessionAPI::CreatePlayerSession);
	Request->SetURL(APIUrl);
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");
	UCDLocalPlayerSubsystem* LocalPlayerSubsystem = GetCDLocalPlayerSubsystem();
	if (IsValid(LocalPlayerSubsystem))
	{
		Request->SetHeader("Authorization", LocalPlayerSubsystem->GetAuthResult().AccessToken);	
	}
	
	TMap<FString, FString> Params =
		{
		{TEXT("playerId"), PlayerId},
		{TEXT("gameSessionId"), GameSessionId},
		};
	const FString& Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	
	Request->ProcessRequest();
}

void UGameSessionsManager::CreatePlayerSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded)
{
	if (!bSucceeded)
	{
		PrivateSessionDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
		return;
	}
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsError(JsonObject))
		{
			PrivateSessionDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
			return;
		}
		FCDPlayerSession PlayerSession;
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &PlayerSession);
		PlayerSession.Dump();

		APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
		if (IsValid(LocalPlayerController))
		{
			FInputModeGameOnly InputModeData;
			LocalPlayerController->SetInputMode(InputModeData);
			LocalPlayerController->SetShowMouseCursor(false);
		}
		
		FString Options = "?PlayerSessionId=" + PlayerSession.PlayerSessionId + "?Username=" + PlayerSession.PlayerId;
		
		const FString IpAndPort = PlayerSession.IpAddress + TEXT(":") + FString::FromInt(PlayerSession.Port);
		const FName Address{*IpAndPort};
		UGameplayStatics::OpenLevel(this, Address, true, Options);
	}
}

void UGameSessionsManager::GetGameSessions_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded)
{
	if (!bSucceeded)	
		return;

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		if (ContainsError(JsonObject))
			return;
		
		FCDDescribeGameSessionResult DescribeGameSessionResult;
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &DescribeGameSessionResult);
		//GameSession.Dump();

		OnGetSessionsRequestSucceeded.Broadcast(DescribeGameSessionResult);
	}
}
