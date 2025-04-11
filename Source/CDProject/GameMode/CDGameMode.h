// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameModeBase.h"
#include "CDGameMode.generated.h"

namespace MatchState
{
	extern CDPROJECT_API const FName Cooldown;
}
UCLASS()
class CDPROJECT_API ACDGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ACDGameMode();
	virtual void Tick(float DeltaSeconds) override;
	virtual void PlayerEliminated(class ACDCharacter* ElimmedCharacter,
		class ACDPlayerController* VictimController,
		ACDPlayerController* AttackerController
		);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);
	

	//InGame Variable
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;
	//1 Round in Matching Time
	UPROPERTY(EditDefaultsOnly)
	float CooldownTime=10.f;
	//when 1 Round End -> Waiting TIme(this) -> 2Round
	UPROPERTY(EditDefaultsOnly)
	float WarmUpTime=10.f;
	float Countdown=0.f;
	float LevelStartingTime=0.f;

	
	
protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;
};
