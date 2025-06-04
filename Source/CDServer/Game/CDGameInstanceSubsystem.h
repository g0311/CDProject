// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CDServer/Data/Player/FPlayerSessionInfo.h"
#include "GameLiftServerSDK.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CDGameInstanceSubsystem.generated.h"

/**
 * 
 */
DECLARE_LOG_CATEGORY_EXTERN(CD_GAMEINSTANCE, Log, All);

UCLASS()
class CDSERVER_API UCDGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UCDGameInstanceSubsystem();
	void InitGameLift(const FServerParameters& ServerParameters);
	UPROPERTY(BlueprintReadOnly)
	bool bGameLiftInitialized = false;

	FTimerHandle ExitHandle;
private:
	void ParseCommandLienPort(int32& outPort);
	FProcessParameters m_params;

public:
	FPlayerSessionInfoArray PlayerInfos;
	//name, readystate, ping
	
	FString RoomMode;
	FString RoomMap;
	FString RoomName;
	FString bIsPrivate;
	FString GameSessionId;
};
