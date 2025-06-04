// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TransitionHUD.generated.h"

/**
 * 
 */
UCLASS()
class CDSERVER_API ATransitionHUD : public AHUD
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class URoomPage> RoomPageClass;
	
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY() 
	TObjectPtr<class URoomPage> RoomPage;

	FTimerHandle LobbyCheckTimerHandle;

	void UpdateRoomPage();
};
