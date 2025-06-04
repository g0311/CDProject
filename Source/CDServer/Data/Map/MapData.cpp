// Fill out your copyright notice in the Description page of Project Settings.


#include "MapData.h"

const TArray<FString> UMapData::GetModes()
{
	TArray<FString> Modes;
	for (auto mode : Maps)
	{
		Modes.Add(mode.Key);
	}
	return Modes;
}

const TArray<FString> UMapData::GetMapsFromMode(FString Mode)
{
	if (Maps.Contains(Mode))
		return Maps[Mode].MapNames;
	else
		return TArray<FString>();
}

const FString UMapData::GetRandomMode()
{
	TArray<FString> Keys;
	Maps.GetKeys(Keys);

	if (Keys.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("GetRandomMode: No modes available in Maps."));
		return FString();
	}

	const int32 RandomIndex = FMath::RandRange(0, Keys.Num() - 1);
	return Keys[RandomIndex];
}

const FString UMapData::GetRandomMapFromMode(FString Mode)
{
	if (Maps.Contains(Mode))
	{
		int32 RandIndex = FMath::RandRange(0, Maps[Mode].MapNames.Num() - 1);
		return Maps[Mode].MapNames[RandIndex];
	}
	else
	{
		return FString();
	}
}
