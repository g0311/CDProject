// Fill out your copyright notice in the Description page of Project Settings.


#include "CDAnimInstance.h"

#include "AnimationCompression.h"
#include "CDProject/Component/FootIKComponent.h"
#include "KismetAnimationLibrary.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Component/CombatComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PawnMovementComponent.h"
#include "CDProject/Weapon/Weapon.h"

void UCDAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	if (_playerCharacter == nullptr)
	{
		_playerCharacter = Cast<ACDCharacter>(TryGetPawnOwner());
	}
}

void UCDAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (_playerCharacter == nullptr)
		_playerCharacter = Cast<ACDCharacter>(TryGetPawnOwner());
	
	if (_playerCharacter)
	{
		if (_isFullBody)
		{
			UpdateFullBodyProperty(DeltaSeconds);
		}
		else
		{
			UpdateUpperBodyProperty(DeltaSeconds);
		}
	}
}

void UCDAnimInstance::PlayFireMontage(float fireRate)
{
	if (_isFullBody)
	{
		UE_LOG(LogTemp, Warning, TEXT("Called1"));
		if (_isAiming)
		{
			if (_aimFireMontage)
			{
				Montage_Play(_aimFireMontage, 1.f / fireRate);
				UE_LOG(LogTemp, Warning, TEXT("Called2"));

			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Called3"));
			if (_weaponType == static_cast<uint8>(EWeaponType::EWT_Pistol))
			{
				if (_pistolFireMontage)
				{
					Montage_Play(_pistolFireMontage, 1.f / fireRate);
					UE_LOG(LogTemp, Warning, TEXT("Called4"));
				}
			}
			else
			{
				if (_baseFireMontage)
				{
					Montage_Play(_baseFireMontage, 1.f / fireRate);
				}
			}
		}
	}
	else
	{
		if (_weaponType == static_cast<uint8>(EWeaponType::EWT_Pistol))
		{
			if (_pistolFireMontage)
			{
				Montage_Play(_pistolFireMontage, 1.f / fireRate);
			}
		}
		else if (_aimFireMontage)
		{
			Montage_Play(_aimFireMontage, 1.f / fireRate);
		}
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
			if (_rifleReloadMontage)
				Montage_Play(_rifleReloadMontage);
			break;
		case static_cast<uint8>(EWeaponType::EWT_Shotgun):
			if (_rifleReloadMontage)
				Montage_Play(_shotgunReloadMontage);
			break;
		case static_cast<uint8>(EWeaponType::EWT_Pistol):
			if (_pistolReloadMontage)
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
			if (_rifleReloadMontage)
				Montage_Play(_rifleReloadMontage);
			break;
		case static_cast<uint8>(EWeaponType::EWT_Shotgun):
			if (_shotgunReloadMontage)
				Montage_Play(_shotgunReloadMontage);
			break;
		case static_cast<uint8>(EWeaponType::EWT_Pistol):
			if (_pistolReloadMontage)
				Montage_Play(_pistolReloadMontage);
			break;
		}
	}
}

void UCDAnimInstance::PlayEquipMontage(class AWeapon* nextWeapon)
{
	switch (nextWeapon->GetWeaponType())
	{
	case (EWeaponType::EWT_Rifle):
	case (EWeaponType::EWT_Sniper):
	case (EWeaponType::EWT_Shotgun):
		if (_equipRifleMontage)
		{
			Montage_Play(_equipRifleMontage);
		}
		break;
	case (EWeaponType::EWT_Pistol):
		if (_equipPistolMontage)
		{
			Montage_Play(_equipPistolMontage);
		}
		break;
	default:
		break;
	}
}

void UCDAnimInstance::PlayDeadMontage()
{
	_leftHandIKAlpha = 0.f;
	if (_deadMontage)
		Montage_Play(_deadMontage);
	
}

void UCDAnimInstance::PlayHitMontage()
{
	if (_hitMontage.Num() == 0) return;

	int32 RandomIndex = FMath::RandRange(0, _hitMontage.Num() - 1);
	UAnimMontage* SelectedMontage = _hitMontage[RandomIndex];
	if (SelectedMontage)
	{
		Montage_Play(SelectedMontage);
	}
}

void UCDAnimInstance::UpdateFullBodyProperty(float DeltaSeconds)
{
	//Lower Part
	FVector velocity = _playerCharacter->GetVelocity();
	velocity = FVector(velocity.X, velocity.Y, 0.f);
		
	_movementSpeed = FVector(velocity.X, velocity.Y, 0.f).Size();
	_direction = UKismetAnimationLibrary::CalculateDirection(velocity, _playerCharacter->GetActorRotation());
	//UE_LOG(LogTemp, Log, TEXT("anim : %f"), _direction);
	
	FRotator controlRot = _playerCharacter->GetControlRotation();
	FRotator actorRot = _playerCharacter->GetActorRotation();
	FRotator deltaRot = controlRot - actorRot;
		
	_aimPitch = FMath::UnwindDegrees(deltaRot.Pitch);
	_aimPitch = FMath::Clamp(_aimPitch, -75.f, 75.f);
	_aimYaw = FMath::UnwindDegrees(deltaRot.Yaw);
		
	_isJumping = _playerCharacter->GetMovementComponent()->IsFalling();
	_isCrouching = _playerCharacter->GetMovementComponent()->IsCrouching();
	
	UFootIKComponent* IKFootComp = _playerCharacter->FindComponentByClass<UFootIKComponent>();
	if (!IKFootComp) return;
	_lFootRotator = IKFootComp->_outValue._footRotation_Left;
	_rFootRotator = IKFootComp->_outValue._footRotation_Right;
	_hipOffset = IKFootComp->_outValue._hipOffset;
	_lFootOffset = IKFootComp->_outValue._effectorLocation_Left;
	_rFootOffset = IKFootComp->_outValue._effectorLocation_Right;
	
	UpdateUpperBodyProperty(DeltaSeconds);
}

void UCDAnimInstance::UpdateUpperBodyProperty(float DeltaSeconds)
{
	//Upper Part
	UCombatComponent* combatComponent = _playerCharacter->GetComponentByClass<UCombatComponent>();
	if (combatComponent)
	{
		_weaponType = combatComponent->GetCurWeaponType();
		_isAiming = combatComponent->IsAiming();

		if (combatComponent->IsChanging())
		{
			_leftHandIKAlpha = FMath::FInterpTo(_leftHandIKAlpha, 0.f, DeltaSeconds, 10.f);
		}
		else
		{
			_leftHandIKAlpha = FMath::FInterpTo(_leftHandIKAlpha, 0.85f, DeltaSeconds, 10.f);
		}

		if (combatComponent->GetCurWeapon())
		{
			if (combatComponent->GetCurWeapon()->GetWeaponType() != EWeaponType::EWT_Speical)
			{
				if (_isFullBody)
				{
					if(combatComponent->GetCurWeapon()->GetWeaponMesh3p()->DoesSocketExist(TEXT("LeftHandGrip")))
						_leftHandLocation = combatComponent->GetCurWeapon()->GetWeaponMesh3p()->GetSocketLocation(TEXT("LeftHandGrip"));
				}
				else
				{
					if(combatComponent->GetCurWeapon()->GetWeaponMesh3p()->DoesSocketExist(TEXT("LeftHandGrip")))
						_leftHandLocation = combatComponent->GetCurWeapon()->GetWeaponMesh()->GetSocketLocation(TEXT("LeftHandGrip"));
				}
			}
		}
	}
}

float UCDAnimInstance::GetReloadTime()
{
	switch (_weaponType)
	{
	case static_cast<uint8>(EWeaponType::EWT_Rifle):
	case static_cast<uint8>(EWeaponType::EWT_Sniper):
		if (_rifleReloadMontage)
			return _rifleReloadMontage->GetPlayLength();
		break;
	case static_cast<uint8>(EWeaponType::EWT_Shotgun):
		if (_shotgunReloadMontage)
			return _shotgunReloadMontage->GetPlayLength();
		break;
	case static_cast<uint8>(EWeaponType::EWT_Pistol):
		if (_pistolReloadMontage)
			return _pistolReloadMontage->GetPlayLength();
		break;	
	}
	return 0.f;
}

float UCDAnimInstance::GetEquipTime(AWeapon* nextWeapon)
{
	if (!nextWeapon)
		return 0.f;
	
	switch (nextWeapon->GetWeaponType())
	{
	case (EWeaponType::EWT_Rifle):
	case (EWeaponType::EWT_Sniper):
	case (EWeaponType::EWT_Shotgun):
		if (_equipRifleMontage)
		{
			return _equipRifleMontage->GetPlayLength();
		}
		break;
	case (EWeaponType::EWT_Pistol):
		if (_equipPistolMontage)
		{
			return _equipPistolMontage->GetPlayLength();
		}
		break;
	}
	return 0.f;
}
