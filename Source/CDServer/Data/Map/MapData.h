// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MapData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMapList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> MapNames;
};

UCLASS()
class CDSERVER_API UMapData : public UDataAsset
{
	GENERATED_BODY()
public:
	const TArray<FString> GetModes();
	const TArray<FString> GetMapsFromMode(FString Mode);
	const FString GetRandomMode();
	const FString GetRandomMapFromMode(FString Mode);
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, FMapList> Maps;
};
