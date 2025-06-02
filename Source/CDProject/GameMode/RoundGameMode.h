// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CDProject/Types/CurMatchState.h"
#include "CDServer/Game/Server_GameMode.h"
#include "RoundGameMode.generated.h"

namespace MatchState
{
	extern CDPROJECT_API const FName PreInProgress;
	extern CDPROJECT_API const FName Cooldown;
	extern CDPROJECT_API const FName ModeSelect;
}

UCLASS()
class CDPROJECT_API ARoundGameMode : public AServer_GameMode
{
	GENERATED_BODY()
public:
	ARoundGameMode();
	virtual void Tick(float DeltaSeconds) override;
	// virtual void PostLogin(APlayerController* NewPlayer) override;
	// virtual void Logout(AController* Exiting) override;
	
	virtual void RestartMatch(bool isForce = false); //Custom
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName = L"") override;
	//virtual bool ShouldSpawnAtStartSpot(AController* Player) override;
	void SendPlayerJoined();
	virtual void SetCurMatchState(ECurMatchState NewState, bool IsInit = false);
	virtual void PlayerEliminated(
		class ACDPlayerController* VictimController,
		ACDPlayerController* AttackerController
		);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);

	ECurMatchState GetCurMatchState(){ return _curMatchState; }
	void AddDestroyableActor(AActor* DestroyableActor) {_createdActors.Add(DestroyableActor);}

	//InGame Variable
	UPROPERTY(EditDefaultsOnly)
	float defaultMatchTime = 90.f;//	//1 Round in Matching Time
	float MatchTime = 90.f;
	
	UPROPERTY(EditDefaultsOnly)
	float CooldownTime=10.f;
	UPROPERTY(EditDefaultsOnly)
	float WarmUpTime=10.f;
	float Countdown=10.f;
	bool bNotifiedCooldown=false;
	
	int32 MaxRound=4;
	int32 CurRound=0;
	bool bTeamsMatch=false;

	float WaitingStartTime = 0.f;
	float MatchStartTime = 0.f;
	float CooldownStartTime = 0.f;

	int _joinedClinetCount = 0;
	int _maxClientCount = 2; //6
protected:
	virtual void BeginPlay() override;
	virtual void OnCurMatchStateSet();
	
	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> _createdActors;
private:
	TMap<FString, TArray<class APlayerStart*>> AvailStartPoints;

	UPROPERTY(visibleAnywhere)
	ECurMatchState _curMatchState = ECurMatchState::EMS_None;

	FTimerHandle _startGameTimerHandle;
};