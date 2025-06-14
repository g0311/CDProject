// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSessionsManager.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "CDServer/Data/API/APIData.h"
#include "CDServer/Player/CDLocalPlayerSubsystem.h"
#include "CDServer/UI/HTTP/HTTPRequestTypes.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/IHttpResponse.h"

void UGameSessionsManager::QuickJoinGameSession(const FString& GameMode, const FString& RoomMap)
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
		{TEXT("roomName"), TEXT("")},
		{TEXT("isPrivate"), TEXT("true")},
		{TEXT("isStarted"), TEXT("false")},
		{TEXT("roomMap"), RoomMap},
		{TEXT("roomMode"), GameMode},
		};
	const FString& Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	
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
	PrivateSessionCreateDelegate.Broadcast(TEXT("Creating Game Session..."), false);

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UGameSessionsManager::CreatePrivateGameSession_Response);

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
		{TEXT("roomName"), RoomName},
		{TEXT("isPrivate"), TEXT("true")},
		{TEXT("isStarted"), TEXT("false")},
		{TEXT("roomMap"), RoomMap},
		{TEXT("roomMode"), RoomMode},
		};
	const FString& Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	
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

void UGameSessionsManager::UpdateGameSession(const FString& GameSessionId, const FString& GameMap, const FString& GameMode, const FString& IsStarted)
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UGameSessionsManager::UpdateGameSession_Response);

	check(APIData);
	const FString APIUrl = APIData->GetAPIEndpoint(ServerTags::GameSessionAPI::UpdateGameSession);
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
		{TEXT("isStarted"), IsStarted},
		{TEXT("roomMap"), GameMap},
		{TEXT("roomMode"), GameMode},
		};
	const FString& Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	
	Request->ProcessRequest();
}

void UGameSessionsManager::StartMatchMaking()
{
	// Aws::GameLift::Server::Model::Player Player;
	// Player.SetPlayerId("Player123");
	//
	// // 속성 추가
	// Aws::GameLift::Server::Model::AttributeValue SkillAttr;
	// SkillAttr.SetN(1600.0);
	// Player.AddPlayerAttributes("skill", SkillAttr);
	//
	// // 위치별 latency (옵션)
	// Aws::Map<Aws::String, int> LatencyMap;
	// LatencyMap.emplace("ap-northeast-2", 30); // Seoul
	// Player.SetLatencyInMs(LatencyMap);
}


void UGameSessionsManager::FindOrCreateGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response,
                                                            bool bSucceeded)
{
	if (!bSucceeded)
	{
		JoinGameSessionMessageDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
		PrivateSessionDelegate.Broadcast(TEXT(""), true);
		PrivateSessionCreateDelegate.Broadcast(TEXT(""), true);
		return;
	}
	if (Response->GetResponseCode() == 401)
	{
		OnSignOut();
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		FString ErrorType = ContainsError(JsonObject);
		if (!ErrorType.IsEmpty())
		{
			JoinGameSessionMessageDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
			PrivateSessionDelegate.Broadcast(TEXT(""), true);
			PrivateSessionCreateDelegate.Broadcast(TEXT(""), true);
			return;
		}
		//DumpMetaData(JsonObject);

		FCDGameSession GameSession;
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &GameSession);
		
		const FString GameSessionId = GameSession.GameSessionId;
		const FString GameSessionStatus = GameSession.Status;
		FString GameMode;
		FString GameMap;
		for (auto& property : GameSession.GameProperties)
		{
			if (property.Key == TEXT("Mode"))
			{
				GameMode = property.Value;
			}
			if (property.Key == TEXT("Map"))
			{
				GameMap = property.Value;
			}
		}
		HandleGameSessionStatus(GameSessionStatus, GameSessionId, GameMode, GameMap);
	}
}

void UGameSessionsManager::CreatePrivateGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bSucceeded)
{
	if (!bSucceeded)
	{
		JoinGameSessionMessageDelegate.Broadcast(TEXT(""), true);
		PrivateSessionDelegate.Broadcast(TEXT(""), true);
		PrivateSessionCreateDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
		return;
	}
	if (Response->GetResponseCode() == 401)
	{
		OnSignOut();
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		FString ErrorType = ContainsError(JsonObject);
		if (!ErrorType.IsEmpty())
		{
			JoinGameSessionMessageDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
			PrivateSessionDelegate.Broadcast(TEXT(""), true);
			PrivateSessionCreateDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
			return;
		}
		
		FCDGameSession GameSession;
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &GameSession);
		if (!GameSession.Status.Equals(TEXT("ACTIVE")) && !GameSession.Status.Equals(TEXT("ACTIVATING")))
		{
			JoinGameSessionMessageDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
			PrivateSessionDelegate.Broadcast(TEXT(""), true);
			PrivateSessionCreateDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
			return;
		}
		
		const FString GameSessionId = GameSession.GameSessionId;

		FTimerDelegate CreatePlayerSessionDelegate;
		CreatePlayerSessionDelegate.BindLambda([this, GameSessionId]()
		{
			if (IsValid(this))
			{
				UCDLocalPlayerSubsystem* LocalPlayerSubsystem = GetCDLocalPlayerSubsystem();
				if (IsValid(LocalPlayerSubsystem))
				{
					TryCreatePlayerSession(LocalPlayerSubsystem->Username, GameSessionId);
				}
			}
		});
		APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
		if (IsValid(LocalPlayerController))
		{
			LocalPlayerController->GetWorldTimerManager().SetTimer(CreatePlayerSessionTimer, CreatePlayerSessionDelegate,0.5f, false);
		}
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

void UGameSessionsManager::HandleGameSessionStatus(const FString& Status, const FString& SessionId, const FString& GameMode, const FString& GameMap)
{
	if (Status.Equals(TEXT("ACTIVE")))
	{
		UCDLocalPlayerSubsystem* LocalPlayerSubsystem = GetCDLocalPlayerSubsystem();
		if (IsValid(LocalPlayerSubsystem))
		{
			TryCreatePlayerSession(LocalPlayerSubsystem->Username, SessionId);
		}
	}
	else if (Status.Equals(TEXT("ACTIVATING")))
	{
		FTimerDelegate CreatePlayerSessionDelegate;
		CreatePlayerSessionDelegate.BindLambda([this, GameMode, GameMap]()
		{
			if (IsValid(this))
				QuickJoinGameSession(GameMode, GameMap);
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
		PrivateSessionDelegate.Broadcast(TEXT(""), true);
		PrivateSessionCreateDelegate.Broadcast(TEXT(""), true);
	}
}

void UGameSessionsManager::TryCreatePlayerSession(const FString& PlayerId, const FString& GameSessionId)
{
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
		JoinGameSessionMessageDelegate.Broadcast(TEXT(""), true);
		PrivateSessionDelegate.Broadcast(TEXT(""), true);
		PrivateSessionCreateDelegate.Broadcast(TEXT(""), true);
		return;
	}
	if (Response->GetResponseCode() == 401)
	{
		OnSignOut();
	}
	
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		FString ErrorType = ContainsError(JsonObject);
		if (!ErrorType.IsEmpty())
		{
			PrivateSessionDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
			return;
		}
		FCDPlayerSession PlayerSession;
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &PlayerSession);
		PlayerSession.Dump();

		// APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
		// if (IsValid(LocalPlayerController))
		// {
		// 	FInputModeGameOnly InputModeData;
		// 	LocalPlayerController->SetInputMode(InputModeData);
		// 	LocalPlayerController->SetShowMouseCursor(false);
		// }
		
		FString Options = "?PlayerSessionId=" + PlayerSession.PlayerSessionId + "?Username=" + PlayerSession.PlayerId;

		//PlayerSession.IpAddress = TEXT("127.0.0.1");
		const FString IpAndPort = PlayerSession.IpAddress + TEXT(":") + FString::FromInt(PlayerSession.Port) + Options;
		UE_LOG(LogTemp, Warning, TEXT("%s"), *IpAndPort);

		APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
		if (IsValid(LocalPlayerController))
		{
			LocalPlayerController->ClientTravel(IpAndPort, TRAVEL_Absolute, false);
		}
	}
}

void UGameSessionsManager::GetGameSessions_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded)
{
	if (!bSucceeded)
	{
		OnGetSessionsRequestSucceeded.Broadcast(FCDDescribeGameSessionResult());
		return;
	}
	if (Response->GetResponseCode() == 401)
	{
		OnSignOut();
	}

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		FString ErrorType = ContainsError(JsonObject);
		if (!ErrorType.IsEmpty())
		{
			OnGetSessionsRequestSucceeded.Broadcast(FCDDescribeGameSessionResult());
			return;
		}
		
		FCDDescribeGameSessionResult DescribeGameSessionResult;
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &DescribeGameSessionResult);
		//GameSession.Dump();

		OnGetSessionsRequestSucceeded.Broadcast(DescribeGameSessionResult);
	}
	else
	{
		//Empty String (no active game sessions)
		OnGetSessionsRequestSucceeded.Broadcast(FCDDescribeGameSessionResult());
	}
}

void UGameSessionsManager::UpdateGameSession_Response(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bSucceeded)
{
	
}
