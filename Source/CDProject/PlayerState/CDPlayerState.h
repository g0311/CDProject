// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CDProject/Types/Team.h"
#include "CDProject/Weapon/Weapon.h"
#include "GameFramework/PlayerState.h"
#include "CDPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class CDPROJECT_API ACDPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ACDPlayerState();

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Player Stats")
	int32 Kills;
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Player Stats")
	int32 Deaths;
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Player Stats")
	FName Name;
	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_Gold,  Category="Player Stats")
	int32 Gold;

	
	void AddKill(){Kills++;}
	void AddDeath(){Deaths++;}
	void AddGold(int32 Amount);
	bool SpendGold(int32 Amount);
	
//OnRep_Function
	UFUNCTION()
	void OnRep_Team();

	UFUNCTION()
	void OnRep_Gold();

protected:
	UPROPERTY(ReplicatedUsing=OnRep_Team)
	ETeam Team=ETeam::ET_NoTeam;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
public:
	FORCEINLINE ETeam GetTeam() const {return Team;}
	
	void SetTeam(ETeam TeamToSet);
};
