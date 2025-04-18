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
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	

	UFUNCTION()
	void OnRep_Team();

protected:
	UPROPERTY(ReplicatedUsing=OnRep_Team)
	ETeam Team=ETeam::ET_NoTeam;

	
public:
	FORCEINLINE ETeam GetTeam() const {return Team;}
	
	void SetTeam(ETeam TeamToSet);
};
