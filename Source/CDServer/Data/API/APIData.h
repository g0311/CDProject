// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CDServer/GameplayTags/ServerTags.h"
#include "APIData.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API UAPIData : public UDataAsset
{
	GENERATED_BODY()
public:
	FString GetAPIEndpoint(const FGameplayTag& APIEndpoint);
	//return FULL invoke url

protected:
	UPROPERTY(EditDefaultsOnly)
	FString APIName;

	UPROPERTY(EditDefaultsOnly)
	FString InvokeURL;

	UPROPERTY(EditDefaultsOnly)
	FString Stage;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FString> Resources;
};
