// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "CDProject/Weapon/Weapon.h"
#include "CDProject/Character/CDCharacter.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	//0: main / 1: sub / 2: melee / 3,4,5: projectile
	_weapons.SetNumZeroed(6);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	CreateDefaultWeapons();
	_weaponIndex = 1;
	ChangeWeapon(_weaponIndex);
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	
}

void UCombatComponent::Reset()
{
	//update weapon stock


	//Switch Weapon
	for (int i = 0; i < _weapons.Num(); i++)
	{
		if (ChangeWeapon(i))
			break;
	}
}

void UCombatComponent::Fire()
{
	FHitResult hit;
	//GetWorld()->LineTraceSingleByChannel(hit, )
	_weapons[_weaponIndex]->Fire(FVector::ZeroVector);
	_isAiming = false;
}

void UCombatComponent::Reload()
{
	//_weapons[_weaponIndex]->;
}

void UCombatComponent::Aim(bool tf)
{
	if (_weapons[_weaponIndex]->GetWeaponType() == EWeaponType::EWT_Sniper)
	{
		_isAiming = tf;
		SetHUDCrosshairs();
	}
}

bool UCombatComponent::ChangeWeapon(int idx)
{ //avail visibility and update curWeaponIndex
	if (_weapons[idx])
	{
		_weapons[_weaponIndex]->GetWeaponMesh()->SetVisibility(false);
		//_weapons[_weaponIndex]->GetSecondWeaponMesh()->SetVisibility(false);
		_isAiming = false;
		
		_weaponIndex = idx;
		_weapons[_weaponIndex]->GetWeaponMesh()->SetVisibility(true);
		//_weapons[_weaponIndex]->GetSecondWeaponMesh()->SetVisibility(true);
		SetHUDCrosshairs();
		return true;
	}
	return false;
}

void UCombatComponent::GetWeapon(AWeapon* weapon, bool isForceGet)
{ // compare weapon type and save or dicard
	switch (weapon->GetWeaponType())
	{
	case EWeaponType::EWT_Rifle:
	case EWeaponType::EWT_Sniper:
	case EWeaponType::EWT_Shotgun:
		if (!_weapons[0])
		{
			_weapons[0] = weapon;
			_weapons[0]->SetWeaponState(EWeaponState::EWS_Equipped);
			ChangeWeapon(0);
		}
		else if (isForceGet)
		{
			DropWeapon();
			_weapons[0] = weapon;
			_weapons[0]->SetWeaponState(EWeaponState::EWS_Equipped);
			ChangeWeapon(0);
		}
		break;
	case EWeaponType::EWT_Speical:
		break;
	}
}

void UCombatComponent::DropWeapon()
{ //avail visibility and update curWeaponIndex
	if (_weapons[_weaponIndex] != _meleeWeapon)
	{
		_weapons[_weaponIndex]->GetWeaponMesh()->SetVisibility(true);
		_weapons[_weaponIndex]->Dropped();
		_weapons[_weaponIndex] = nullptr;

		// while (_weapons[_weaponIndex])
		// 	_weaponIndex = (_weaponIndex + 1) % _weapons.Num();

		for (int i = 0; i < _weapons.Num(); i++)
		{
			if (_weapons[(_weaponIndex + i) % _weapons.Num()])
			{
				_weaponIndex = (_weaponIndex + i) % _weapons.Num();
				break;
			}
		}

		if (_weapons[_weaponIndex])
			_weapons[_weaponIndex]->GetWeaponMesh()->SetVisibility(true);
	}
}

bool UCombatComponent::IsAmmoEmpty()
{
	return false;
}

AWeapon* UCombatComponent::GetCurWeapon()
{
	return _weapons[_weaponIndex];
}

uint8 UCombatComponent::GetCurWeaponType()
{
	if (_weapons[_weaponIndex])
		return static_cast<uint8>(_weapons[_weaponIndex]->GetWeaponType());
	return -1;
}

bool UCombatComponent::IsAimng()
{
	return _isAiming;
}

bool UCombatComponent::IsFireAvail()
{
	float curTime = GetWorld()->GetTimeSeconds();
	if (curTime - LastFireTime > FireRate)
	{
		LastFireTime = curTime;
		return true;
	}
	else
	{
		return false;
	}
}

void UCombatComponent::SetHUDCrosshairs()
{
	
}

void UCombatComponent::CreateDefaultWeapons()
{
	ACDCharacter* owner = Cast<ACDCharacter>(GetOwner());
	if (!owner)
		return;
	
	if (_defaultSubWeapon)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		_weapons[1] = GetWorld()->SpawnActor<AWeapon>(_defaultSubWeapon, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (_weapons[1])
		{
			_weapons[1]->AttachToComponent(
			owner->_armMesh,
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			TEXT("WeaponSocket")
			);
			_weapons[1]->GetWeaponMesh()->SetVisibility(false);
			
			// _weapons[1]->GetWeaponMesh2()->AttachToComponent(
			//owner->_armMesh,
			// FAttachmentTransformRules::SnapToTargetIncludingScale,
			// TEXT("WeaponSocket")
			// );
			//_weapons[1]->GetWeaponMesh2()->SetVisibility(false);
		}
	}
	//Debug
	if (_defaultSubWeapon /*_defaultMeleeWeapon*/)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		_weapons[2] = GetWorld()->SpawnActor<AWeapon>(_defaultSubWeapon, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (_weapons[2])
		{
			_weapons[2]->AttachToComponent(
			owner->_armMesh,
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			TEXT("WeaponSocket")
			);
			_weapons[2]->GetWeaponMesh()->SetVisibility(true);
			
			// _weapons[1]->GetWeaponMesh2()->AttachToComponent(
			//owner->_armMesh,
			// FAttachmentTransformRules::SnapToTargetIncludingScale,
			// TEXT("WeaponSocket")
			// );
			//_weapons[1]->GetWeaponMesh2()->SetVisibility(false);
		}
	}
}

void UCombatComponent::AttatchMeshToChar(class AWeapon* weapon)
{
	
}

