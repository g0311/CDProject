// Fill out your copyright notice in the Description page of Project Settings.


#include "CDAnimInstance.h"

#include "FootIKComponent.h"
#include "KismetAnimationLibrary.h"
#include "CDProject/Component/CombatComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "CDProject/Weapon/Weapon.h"

void UCDAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	if (_playerPawn == nullptr)
	{
		_playerPawn = Cast<ACharacter>(TryGetPawnOwner());
	}
}

void UCDAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (_playerPawn == nullptr)
		_playerPawn = Cast<ACharacter>(TryGetPawnOwner());
	
	if (_playerPawn)
	{
		//Lower Part
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
		
		UFootIKComponent* IKFootComp = _playerPawn->FindComponentByClass<UFootIKComponent>();
		if (!IKFootComp) return;
		_lFootRotator = IKFootComp->_outValue._footRotation_Left;
		_rFootRotator = IKFootComp->_outValue._footRotation_Right;
		_hipOffset = IKFootComp->_outValue._hipOffset;
		_lFootOffset = IKFootComp->_outValue._effectorLocation_Left;
		_rFootOffset = IKFootComp->_outValue._effectorLocation_Right;
		
		//Upper Part
		UCombatComponent* combatComponent = _playerPawn->GetComponentByClass<UCombatComponent>();
		if (combatComponent)
		{
			_weaponType = combatComponent->GetCurWeaponType();
			_isAiming = combatComponent->IsAimng();
			//if (combatComponent->GetCurWeapon()->GetWeaponInfo() != Melee)
				//_leftHandTransform = combatComponent->GetCurWeapon()->GetMesh()->GetSocketTransform()
		}
	}
}