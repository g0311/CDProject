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
	virtual void BeginPlay() override;

	
	void AddKill(){Kills++;}
	void AddDeath(){Deaths++;}
	void AddGold(int32 Amount);
	bool SpendGold(int32 Amount);
	//Getter
	int32 GetKills() const { return Kills; }
	int32 GetDeaths() const { return Deaths; }
	FText GetPlayerName() const { return Name; }
	int32 GetGold() const { return Gold; }
	//Setter

	FORCEINLINE ETeam GetTeam() const {return Team;}
	
//OnRep_Function
	UFUNCTION()
	void OnRep_Team();

	UFUNCTION()
	void OnRep_Gold();

	void SetTeam(ETeam TeamToSet);

protected:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Team)
	ETeam Team=ETeam::ET_NoTeam;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;


private:
	UPROPERTY(Replicated, VisibleAnywhere, Category="Player Stats")
	int32 Kills;
	UPROPERTY(Replicated, VisibleAnywhere, Category="Player Stats")
	int32 Deaths;
	UPROPERTY(Replicated, VisibleAnywhere, Category="Player Stats")
	FText Name;
	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_Gold,  Category="Player Stats")
	int32 Gold=0;

};
