// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalManager.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "Interfaces/HUDManagement.h"
#include "CDServer/Data/API/APIData.h"
#include "CDServer/Player/CDLocalPlayerSubsystem.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/HUD.h"

void UPortalManager::SignIn(const FString& Username, const FString& Password)
{
	SignInMessageDelegate.Broadcast(TEXT("Signing in..."), false);
	
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UPortalManager::SignIn_Response);
	check(APIData);
	const FString APIUrl = APIData->GetAPIEndpoint(ServerTags::PortalAPI::SignIn);
	Request->SetURL(APIUrl);
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");

	LastUserName = Username;
	TMap<FString, FString> Params =
		{
		{TEXT("username"), Username},
		{TEXT("password"), Password},
		};
	const FString& Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	Request->ProcessRequest();
}

void UPortalManager::SignUp(const FString& Username, const FString& Password, const FString& Email)
{
	SignUpMessageDelegate.Broadcast(TEXT("Creating a new account..."), false);
	
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UPortalManager::SignUp_Response);
	check(APIData);
	const FString APIUrl = APIData->GetAPIEndpoint(ServerTags::PortalAPI::SignUp);
	Request->SetURL(APIUrl);
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");
	TMap<FString, FString> Params =
		{
		{TEXT("username"), Username},
		{TEXT("password"), Password},
		{TEXT("email"), Email},
		};
	const FString& Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	Request->ProcessRequest();
	LastUserName = Username;
}

void UPortalManager::SignOut(const FString& AccessToken)
{
	SignInMessageDelegate.Broadcast(TEXT("Signing Out..."), false);
    	
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UPortalManager::SignOut_Response);
	check(APIData);
	const FString APIUrl = APIData->GetAPIEndpoint(ServerTags::PortalAPI::SignOut);
	Request->SetURL(APIUrl);
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");
	TMap<FString, FString> Params =
		{
		{TEXT("accessToken"), AccessToken},
		};
	const FString& Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	Request->ProcessRequest();
}

void UPortalManager::Confirm(const FString& ConfirmationCode)
{
	ConfirmSignUpMessageDelegate.Broadcast(TEXT("Verifying..."), false);
	
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UPortalManager::ConfirmSignUp_Response);
	check(APIData);
	const FString APIUrl = APIData->GetAPIEndpoint(ServerTags::PortalAPI::ConfirmSignUp);
	Request->SetURL(APIUrl);
	Request->SetVerb("PUT");
	Request->SetHeader("Content-Type", "application/json");
	TMap<FString, FString> Params =
		{
		{TEXT("username"), LastUserName},
		{TEXT("confirmationCode"), ConfirmationCode},
		};
	const FString& Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	Request->ProcessRequest();
}

void UPortalManager::RefreshToken(const FString& RefreshToken)
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UPortalManager::RefreshToken_Response);
	check(APIData);
	const FString APIUrl = APIData->GetAPIEndpoint(ServerTags::PortalAPI::SignIn);
	Request->SetURL(APIUrl);
	Request->SetVerb("POST");
	Request->SetHeader("Content-Type", "application/json");
	TMap<FString, FString> Params =
		{
		{TEXT("refreshToken"), RefreshToken}
		};
	const FString& Content = SerializeJsonContent(Params);
	Request->SetContentAsString(Content);
	Request->ProcessRequest();
}

void UPortalManager::QuitGame()
{
	APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (IsValid(LocalPlayerController))
	{
		UKismetSystemLibrary::QuitGame(this, LocalPlayerController, EQuitPreference::Quit, false);
	}
}

void UPortalManager::SignIn_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded)
{
	if (!bSucceeded)
	{
		SignInMessageDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
		return;
	}
	
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		FString ErrorType = ContainsError(JsonObject);
		if (!ErrorType.IsEmpty())
		{
			SignInMessageDelegate.Broadcast(ErrorType, true);
			return;
		}
		//DumpMetaData(JsonObject);

		FCDInitiateAuthResponse initiateAuthResponse;
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &initiateAuthResponse);
		
		UCDLocalPlayerSubsystem* LocalPlayerSubsystem = GetCDLocalPlayerSubsystem();
		if (IsValid(LocalPlayerSubsystem))
		{
			LocalPlayerSubsystem->InitializeTokens(initiateAuthResponse.AuthenticationResult, this);
			LocalPlayerSubsystem->Username = LastUserName;
			LocalPlayerSubsystem->Email = initiateAuthResponse.Email;
		}
		
		SignInSucceededDelegate.Broadcast();
		APlayerController* LocalPlayerController = GEngine->GetFirstLocalPlayerController(GetWorld());
		if (IsValid(LocalPlayerController))
		{
			IHUDManagement* HUDManagementInterface = Cast<IHUDManagement>(LocalPlayerController->GetHUD());
			if (HUDManagementInterface)
			{
				HUDManagementInterface->OnSignIn();
			}
		}
	}
}

void UPortalManager::SignUp_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded)
{
	if (!bSucceeded)
	{
		SignUpMessageDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
		return;
	}
	
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		FString ErrorType = ContainsError(JsonObject);
		if (!ErrorType.IsEmpty())
		{
			SignUpMessageDelegate.Broadcast(ErrorType, true);
			return;
		}
		//DumpMetaData(JsonObject);

		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &LastSignUpResponse);
		
		SignUpSucceededDelegate.Broadcast();
	}
}

void UPortalManager::SignOut_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded)
{
	if (!bSucceeded)
		return;
	
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		FString ErrorType = ContainsError(JsonObject);
		if (!ErrorType.IsEmpty())
			return;

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
}

void UPortalManager::ConfirmSignUp_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded)
{
	if (!bSucceeded)
	{
		ConfirmSignUpMessageDelegate.Broadcast(HTTPStatusMessages::SomethingWentWrong, true);
		return;
	}
	
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		FString ErrorType = ContainsError(JsonObject);
		if (!ErrorType.IsEmpty())
		{
			if(JsonObject->HasField("name"))
			{
				FString ExeptionName = JsonObject->GetStringField("name");
				if (ExeptionName.Equals(TEXT("CodeMismatchException")))
				{
					ConfirmSignUpMessageDelegate.Broadcast(TEXT("Incorrect verification code"), true);
				}
			}
			else
			{
				ConfirmSignUpMessageDelegate.Broadcast(ErrorType, true);
			}
			return;
		}
		ConfirmSignUpSucceededDelegate.Broadcast();
	}
}

void UPortalManager::RefreshToken_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSucceeded)
{
	if (!bSucceeded) return;
	
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		FString ErrorType = ContainsError(JsonObject);
		if (!ErrorType.IsEmpty()) return;

		FCDInitiateAuthResponse initiateAuthResponse;
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &initiateAuthResponse);
		
		UCDLocalPlayerSubsystem* LocalPlayerSubsystem = GetCDLocalPlayerSubsystem();
		if (IsValid(LocalPlayerSubsystem))
		{
			LocalPlayerSubsystem->UpdateTokens(initiateAuthResponse.AuthenticationResult.AccessToken, initiateAuthResponse.AuthenticationResult.IdToken);
		}
	}
}














