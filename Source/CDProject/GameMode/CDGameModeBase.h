// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameLiftServerSDK.h"
#include "CDGameModeBase.generated.h"

/**
 * 
 */
DECLARE_LOG_CATEGORY_EXTERN(GameServerLog, Log, All);

UCLASS()
class CDPROJECT_API ACDGameModeBase : public AGameMode
{
	GENERATED_BODY()
	public:
        ACDGameModeBase();
    
    protected:
        virtual void BeginPlay() override;
	void SetServerParameters(FServerParameters& serverParameters);

private:
        // Process Parameters needs to remain in scope for the lifetime of the app
        FProcessParameters m_params;
    
        void InitGameLift();
};
