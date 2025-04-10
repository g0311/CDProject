// Fill out your copyright notice in the Description page of Project Settings.


#include "CDCharacterMovementComponent.h"

#include "GameFramework/Character.h"

void FSavedMove_CDCharacter::Clear()
{
    Super::Clear();
    bWantsToWalk = false;
}

void FSavedMove_CDCharacter::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
	class FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
	
	if (UCDCharacterMovementComponent* CDCMC = Cast<UCDCharacterMovementComponent>(C->GetCharacterMovement()))
	{
		bWantsToWalk = CDCMC->bWantsToWalk;
	}
}

uint8 FSavedMove_CDCharacter::GetCompressedFlags() const
{
    uint8 Result = Super::GetCompressedFlags();
	if (bWantsToWalk)
	{
		Result |= FLAG_Custom_0;
	}

	return Result;
}

FNetworkPredictionData_Client_CDCharacter::FNetworkPredictionData_Client_CDCharacter(
	const UCharacterMovementComponent& ClientMovement)
		: Super(ClientMovement)
{}

FSavedMovePtr FNetworkPredictionData_Client_CDCharacter::AllocateNewMove()
{
    return FSavedMovePtr(new FSavedMove_CDCharacter());
}

void UCDCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bWantsToWalk = ((Flags & FSavedMove_Character::FLAG_Custom_0) != 0);
}

FNetworkPredictionData_Client* UCDCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UCDCharacterMovementComponent* MutableThis = const_cast<UCDCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_CDCharacter(*this);
	}

	return ClientPredictionData;
}

float UCDCharacterMovementComponent::GetMaxSpeed() const {

	if (!bWantsToCrouch && bWantsToWalk)
        return MaxSneakSpeed;
    return Super::GetMaxSpeed(); // 걷기 속도
}