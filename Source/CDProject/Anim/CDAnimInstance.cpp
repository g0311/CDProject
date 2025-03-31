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
		if (_isFullBody)
			UpdateFullBodyProperty();
		else
			UpdateUpperBodyProperty();
	}
}

void UCDAnimInstance::PlayFireMontage()
{
	if (_isFullBody)
	{
		if (_isAiming)
			Montage_Play(_aimFireMontage);
		else
			Montage_Play(_baseFireMontage);
		UE_LOG(LogTemp, Log, TEXT("Called Body"));
	}
	else
	{
		Montage_Play(_aimFireMontage);
		UE_LOG(LogTemp, Log, TEXT("Called Arm"));
	}
}

void UCDAnimInstance::PlayReloadMontage()
{
	if (_isFullBody)
	{
		switch (_weaponType)
		{
		case static_cast<uint8>(EWeaponType::EWT_Rifle):
		case static_cast<uint8>(EWeaponType::EWT_Sniper):
			Montage_Play(_rifleReloadMontage);
			//Montage_SetEndDelegate()
			break;
		case static_cast<uint8>(EWeaponType::EWT_Shotgun):
			Montage_Play(_shotgunReloadMontage);
			break;
		case static_cast<uint8>(EWeaponType::EWT_Speical):
			Montage_Play(_pistolReloadMontage);
			break;
		}
	}
	else
	{
		switch (_weaponType)
		{
		case static_cast<uint8>(EWeaponType::EWT_Rifle):
		case static_cast<uint8>(EWeaponType::EWT_Sniper):
			Montage_Play(_rifleReloadMontage);
			break;
		case static_cast<uint8>(EWeaponType::EWT_Shotgun):
			Montage_Play(_shotgunReloadMontage);
			break;
		case static_cast<uint8>(EWeaponType::EWT_Speical):
			Montage_Play(_pistolReloadMontage);
			break;
		}
	}
}

void UCDAnimInstance::PlayEquipMontage()
{
	switch (_weaponType)
	{
	case static_cast<uint8>(EWeaponType::EWT_Rifle):
	case static_cast<uint8>(EWeaponType::EWT_Sniper):
	case static_cast<uint8>(EWeaponType::EWT_Shotgun):
		Montage_Play(_equipRifleMontage);
		break;
	case static_cast<uint8>(EWeaponType::EWT_Speical):
		Montage_Play(_equipPistolMontage);
		break;
	}
}

void UCDAnimInstance::UpdateFullBodyProperty()
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
	
	UpdateUpperBodyProperty();
}

void UCDAnimInstance::UpdateUpperBodyProperty()
{
	//Upper Part
	UCombatComponent* combatComponent = _playerPawn->GetComponentByClass<UCombatComponent>();
	if (combatComponent)
	{
		_weaponType = combatComponent->GetCurWeaponType();
		_isAiming = combatComponent->IsAimng();
		//if (combatComponent->GetCurWeapon()->GetWeaponInfo() != Melee)
		//	_leftHandTransform = combatComponent->GetCurWeapon()->GetMesh()->GetSocketTransform()
	}
}
