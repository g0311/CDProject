// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HTTPRequestManager.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class CDSERVER_API UHTTPRequestManager : public UObject
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UAPIData> APIData;

	bool ContainsError(TSharedPtr<FJsonObject> JsonObject);
	void DumpMetaData(TSharedPtr<FJsonObject> JsonObject);
};
