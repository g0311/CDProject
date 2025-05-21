// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HTTPRequestManager.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAPIStatusMessage, const FString&, StatusMessage, bool, bShouldReset);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAPIRequestSucceeded);

UCLASS(Blueprintable)
class CDSERVER_API UHTTPRequestManager : public UObject
{
	GENERATED_BODY()
public:
	class UCDLocalPlayerSubsystem* GetCDLocalPlayerSubsystem();
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAPIData> APIData;

	bool ContainsError(TSharedPtr<FJsonObject> JsonObject);
	void DumpMetaData(TSharedPtr<FJsonObject> JsonObject);

	FString SerializeJsonContent(const TMap<FString, FString>& Params); 
};
