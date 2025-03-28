// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "CDProject/Weapon/Weapon.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	//0: main / 1: sub / 2: melee / 3,4,5: projectile
	_weapons.SetNumZeroed(6);
	//create default weapon (sub, knife)
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
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
	_curWeapon->Fire(FVector::ZeroVector);
}

void UCombatComponent::Reload()
{
	
}

void UCombatComponent::Aim(bool tf)
{
	if (_curWeapon /*if TYPE == Sniper*/)
	{
		_isAiming = tf;
		SetHUDCrosshairs();
	}
}

bool UCombatComponent::ChangeWeapon(int idx)
{
	if (_weapons[idx])
	{
		//_curWeapon->SetVisibilty(false);
		_curWeapon = _weapons[idx];
		SetHUDCrosshairs();
		//_curWeapon->SetVisibilty(true);
		return true;
	}
	return false;
}

void UCombatComponent::GetWeapon(AWeapon* weapon)
{
	//weapon type 검색 => 해당 타입 무기 존재 확인 => 거르거나 지정하기
}

void UCombatComponent::DropWeapon()
{
	if (_curWeapon != _meleeWeapon)
	{
		_curWeapon = nullptr;
		
	}
}

bool UCombatComponent::IsAmmoEmpty()
{
	return false;
}

AWeapon* UCombatComponent::GetCurWeapon()
{
	return _curWeapon;
}

uint8 UCombatComponent::GetCurWeaponType()
{
	return 0;
}

bool UCombatComponent::IsAimng()
{
	return _isAiming;
}

void UCombatComponent::SetHUDCrosshairs()
{
}

