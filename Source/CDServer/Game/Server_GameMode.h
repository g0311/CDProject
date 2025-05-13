// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameLiftServerSDK.h"
#include "GameFramework/GameMode.h"
#include "Server_GameMode.generated.h"

/**
 * 
 */
DECLARE_LOG_CATEGORY_EXTERN(LogCD_ServerLog, Log, All);

UCLASS()
class CDSERVER_API AServer_GameMode : public AGameMode
{
	GENERATED_BODY()
public:
	AServer_GameMode();
    
protected:
	virtual void BeginPlay() override;
	void SetServerParameters(FServerParameters& serverParameters);
	void ParseCommandLienPort(int32& outPort);

private:
	// Process Parameters needs to remain in scope for the lifetime of the app
	FProcessParameters m_params;
    
	void InitGameLift();
};
