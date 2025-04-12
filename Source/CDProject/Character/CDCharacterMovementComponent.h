// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CDCharacterMovementComponent.generated.h"

/**
 * 
 */
class FSavedMove_CDCharacter : public FSavedMove_Character
{
public:
	typedef FSavedMove_Character Super;

	virtual void Clear() override;

	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;

	uint8 GetCompressedFlags() const override;

	uint32 bWantsToWalk : 1;
};

class FNetworkPredictionData_Client_CDCharacter : public FNetworkPredictionData_Client_Character
{
public:
	typedef FNetworkPredictionData_Client_Character Super;

	FNetworkPredictionData_Client_CDCharacter(const UCharacterMovementComponent& ClientMovement);

	virtual FSavedMovePtr AllocateNewMove() override;
};

UCLASS()
class CDPROJECT_API UCDCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	uint8 bWantsToWalk : 1;
    virtual  FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual float GetMaxSpeed() const override;

	UPROPERTY(Category="Character Movement: Walking", EditAnywhere, BlueprintReadWrite, meta=(ClampMin="0", UIMin="0", ForceUnits="cm/s"))
	float MaxSneakSpeed;
protected:
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
};
