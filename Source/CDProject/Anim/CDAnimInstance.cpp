// Fill out your copyright notice in the Description page of Project Settings.


#include "CDAnimInstance.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/PawnMovementComponent.h"

void UCDAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	if (_playerPawn == nullptr)
	{
		_playerPawn = TryGetPawnOwner();	// 소유자의 Pawn를 가져온다.
	}
}

void UCDAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (_playerPawn == nullptr)
		_playerPawn = TryGetPawnOwner();

	if (_playerPawn)
	{
		FVector velocity = _playerPawn->GetVelocity();
		velocity = FVector(velocity.X, velocity.Y, 0.f);
		
		_movementSpeed = FVector(velocity.X, velocity.Y, 0.f).Size();
		_direction = UKismetAnimationLibrary::CalculateDirection(velocity, _playerPawn->GetActorRotation());

		FRotator controlRot = _playerPawn->GetControlRotation();
		FRotator actorRot = _playerPawn->GetActorRotation();
		FRotator deltaRot = controlRot - actorRot;
		_aimPitch = FMath::UnwindDegrees(deltaRot.Pitch);
		_aimPitch = FMath::Clamp(_aimPitch, -75.f, 75.f);
		_aimYaw = FMath::UnwindDegrees(deltaRot.Yaw);
		
		_isJumping = _playerPawn->GetMovementComponent()->IsFalling();
		_isCrouching = _playerPawn->GetMovementComponent()->IsCrouching();
	}
}
