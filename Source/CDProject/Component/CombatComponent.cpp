// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include <CDProject/HUD/CDHUD.h>

#include "Camera/CameraComponent.h"
#include "CDProject/Anim/CDAnimInstance.h"
#include "CDProject/Weapon/Weapon.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "CDProject/Interface/IsEnemyInterface.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Net/UnrealNetwork.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	//0: main / 1: sub / 2: melee / 3,4,5: ?
	_weapons.SetNumZeroed(6);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	_playerCharacter = Cast<ACDCharacter>(GetOwner());
	if (_playerCharacter->HasAuthority())
	{
		CreateDefaultWeapons();
		ServerChangeWeapon(1);
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	//Update Spread
	if (_playerCharacter->HasAuthority())
	{
		float newSpread = CalculateSpread();
		_curSpread = FMath::FInterpTo(_curSpread, newSpread, DeltaTime, 50.f);
	}
	SetHUDCrosshairs(_curSpread);
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UCombatComponent, _isAiming);
	DOREPLIFETIME(UCombatComponent, _weapons);
	DOREPLIFETIME(UCombatComponent, _weaponIndex);
	DOREPLIFETIME(UCombatComponent, _isCanAim);
	DOREPLIFETIME(UCombatComponent, _isCanFire);
	DOREPLIFETIME(UCombatComponent, _curSpread);
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
	if (_weaponIndex != -1 && _weapons[_weaponIndex])
	{
		APlayerController* playerController = Cast<APlayerController>(_playerCharacter->GetController());
		if (!playerController)
		{
			return;
		}
		
		FRotator CameraRot;
		FVector CameraLoc;
		playerController->GetPlayerViewPoint(CameraLoc, CameraRot);
		
		FVector FireDirection = CameraRot.Vector();
		
		ServerFire(FireDirection);
	}
}

void UCombatComponent::Reload()
{
	ServerReload();
}

void UCombatComponent::Aim()
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return;
	
	if (_weapons[_weaponIndex]->GetWeaponType() != EWeaponType::EWT_Speical)
	{
		_isAiming = true;
	}
}

void UCombatComponent::UnAim()
{
	_isAiming = false;
}

bool UCombatComponent::ChangeWeapon(int idx)
{
	if (idx == _weaponIndex || idx < 0 || idx >= _weapons.Num())
	{
		return false;
	}
	
	if (_weapons[idx])
	{
		ServerChangeWeapon(idx);
		return true;
	}
	return false;
}

void UCombatComponent::GetWeapon(AWeapon* weapon, bool isForceGet)
{ // Only Server Called Func
	switch (weapon->GetWeaponType())
	{
	case EWeaponType::EWT_Rifle:
	case EWeaponType::EWT_Sniper:
	case EWeaponType::EWT_Shotgun:
		if (isForceGet)
		{
			if (ChangeWeapon(0))
				DropWeapon();
		}
		if (!_weapons[0])
		{
			weapon->SetOwner(_playerCharacter);
			weapon->AttachToPlayer();
			_weapons[0] = weapon;
			ChangeWeapon(0);
				//여기 visible true로 변경
		}
		break;
	case EWeaponType::EWT_Pistol:
		if (isForceGet)
		{
			if (ChangeWeapon(1))
				DropWeapon();
		}
		if (!_weapons[1])
		{
			weapon->SetOwner(_playerCharacter);
			weapon->AttachToPlayer();
			_weapons[1] = weapon;
			ChangeWeapon(1);
		}
		break;
	}
}

void UCombatComponent::DropWeapon()
{ //avail visibility and update curWeaponIndex
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return;
	
	if (_weaponIndex != 2)
	{
		ServerDropWeapon();
	}
}

void UCombatComponent::SetHUDCrosshairs(float spread)
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return;
	
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (!character || !character->Controller || _weaponIndex == -1) return;

	ACDPlayerController* controller = Cast<ACDPlayerController>(character->Controller);
	if (controller)
	{
		HUD = HUD == nullptr ? Cast<ACDHUD>(controller->GetHUD()) : HUD;
		if (HUD)
		{
			if (_weapons[_weaponIndex])
			{
				HUDPackage.CrosshairCenter = _weapons[_weaponIndex]->CrosshairCenter;
				HUDPackage.CrosshairLeft = _weapons[_weaponIndex]->CrosshairLeft;
				HUDPackage.CrosshairRight = _weapons[_weaponIndex]->CrosshairRight;
				HUDPackage.CrosshairBottom = _weapons[_weaponIndex]->CrosshairBottom;
				HUDPackage.CrosshairTop = _weapons[_weaponIndex]->CrosshairTop;
			}
			else
			{
				HUDPackage.CrosshairCenter = nullptr;
				HUDPackage.CrosshairLeft = nullptr;
				HUDPackage.CrosshairRight = nullptr;
				HUDPackage.CrosshairBottom = nullptr;
				HUDPackage.CrosshairTop = nullptr;
			}
			HUDPackage.CrosshairColor = FLinearColor(0.1f, 1.f, 0.f, 1.f);
			HUDPackage.CrosshairSpread=spread;
			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

AWeapon* UCombatComponent::GetCurWeapon()
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return nullptr;
	return _weapons[_weaponIndex];
}

bool UCombatComponent::IsAmmoEmpty()
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return false;
	return _weapons[_weaponIndex]->AmmoIsEmpty();
}

bool UCombatComponent::IsTotalAmmoEmpty()
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return false;

	return
		(_weapons[_weaponIndex]->GetCarriedAmmo() == 0
			&& _weapons[_weaponIndex]->GetAmmo() == 0);
}

uint8 UCombatComponent::GetCurWeaponType()
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return -1;

	return static_cast<uint8>(_weapons[_weaponIndex]->GetWeaponType());
}

void UCombatComponent::CreateDefaultWeapons()
{
	if (_defaultSubWeapon)
	{
		_weapons[1] = GetWorld()->SpawnActor<AWeapon>(_defaultSubWeapon, FVector::ZeroVector, FRotator::ZeroRotator);
		_weapons[1]->SetOwner(_playerCharacter);
		_weapons[1]->AttachToPlayer();
	}
	//Debug
	if (_defaultMeleeWeapon)
	{
		_weapons[0] = GetWorld()->SpawnActor<AWeapon>(_defaultMeleeWeapon, FVector::ZeroVector, FRotator::ZeroRotator);
		_weapons[0]->SetOwner(_playerCharacter);
		_weapons[0]->AttachToPlayer();
	}
}

float UCombatComponent::CalculateSpread()
{
	if (!_playerCharacter)
		return 0;
	
	float spread = 1.f;
	
	float Speed = _playerCharacter->GetVelocity().Size();
	spread += (Speed / 470.f) * 1.8f; //MaxSpeed
	
	if (_playerCharacter->GetMovementComponent()->IsFalling())
	{
		spread += 3.f;
	}
	if (_playerCharacter->bIsCrouched)
	{
		spread -= 0.5f;  // 앉으면 감소
	}
	if (_isAiming)
	{
		spread -= 0.4f;
	}
	return FMath::Clamp(spread, 0.1f, 5.f);
}

void UCombatComponent::SetWeaponVisible(bool tf)
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return;
	
	_weapons[_weaponIndex]->GetWeaponMesh()->SetVisibility(tf);
	_weapons[_weaponIndex]->GetWeaponMesh3p()->SetVisibility(tf);
}

void UCombatComponent::SetBefWeaponVisible(bool tf)
{
	if (_befIndex == -1 || !_weapons[_befIndex] || _befIndex == _weaponIndex)
	{
		_befIndex = _weaponIndex;
		return;
	}

	if (_playerCharacter->IsLocallyControlled())
		UE_LOG(LogTemp,Log,TEXT("%d"), _befIndex);

	_weapons[_befIndex]->GetWeaponMesh()->SetVisibility(tf);
	_weapons[_befIndex]->GetWeaponMesh3p()->SetVisibility(tf);

	_befIndex = _weaponIndex;
}

void UCombatComponent::ServerFire_Implementation(FVector fireDir)
{
	FVector traceStart = _playerCharacter->GetCamera()->GetComponentLocation();

    float spreadAngleDeg = _curSpread;
	UE_LOG(LogTemp, Log, TEXT("%f"), _curSpread);
	float spreadAngleRad = FMath::DegreesToRadians(spreadAngleDeg);
    
    FVector rightVector = FVector::CrossProduct(fireDir, FVector::UpVector).GetSafeNormal();
    FVector upVector = FVector::CrossProduct(rightVector, fireDir).GetSafeNormal();
    
    float randYaw = FMath::FRandRange(-spreadAngleRad, spreadAngleRad);
    float randPitch = FMath::FRandRange(-spreadAngleRad, spreadAngleRad);
    
    FVector spreadDirection = 
    	fireDir.RotateAngleAxis(FMath::RadiansToDegrees(randYaw), upVector)
    	             .RotateAngleAxis(FMath::RadiansToDegrees(randPitch), rightVector)
    	             .GetSafeNormal();

	FVector traceEnd = traceStart + spreadDirection * 10000.f;
	
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(GetOwner());
	queryParams.AddIgnoredActor(_weapons[_weaponIndex]);
	FHitResult hit;
	if (GetWorld()->LineTraceSingleByChannel(hit, traceStart, traceEnd, ECC_Visibility, queryParams))
	{
		//DrawDebugLine(GetWorld(), traceStart, hit.Location, FColor::Red, false, 5.0f, 0, 0.5f);
		NetMulticastFire(hit.Location);
		DrawDebugSphere(GetWorld(), hit.Location, 20.f, 20, FColor::Red, false, 5.0f);
	}
	else
	{
		//DrawDebugLine(GetWorld(), traceStart, hit.Location, FColor::Red, false, 5.0f, 0, 0.5f);
		NetMulticastFire(traceEnd);
	}
	
	//Fire Delay
	NetMulticastSetIsCanFire(false);
	GetWorld()->GetTimerManager().SetTimer(_fireTimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		NetMulticastSetIsCanFire(true);
	}), _fireDelay, false);
}

void UCombatComponent::NetMulticastFire_Implementation(FVector target)
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return;
	if (!_playerCharacter)
		return;
	
	_weapons[_weaponIndex]->Fire(target);
	
	UCDAnimInstance* bodyAnim = Cast<UCDAnimInstance>(_playerCharacter->GetMesh()->GetAnimInstance());
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());
	if (bodyAnim)
		bodyAnim->PlayFireMontage(_fireDelay);
	if (armAnim)
		armAnim->PlayFireMontage(_fireDelay);
	
	APlayerController* playerController = Cast<APlayerController>(_playerCharacter->GetController());
	if (playerController && playerController->PlayerCameraManager && _fireCameraShakeClass)
	{
		// playerController->PlayerCameraManager->StartCameraShake(_fireCameraShakeClass);
	}
}

void UCombatComponent::ServerReload_Implementation()
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return;
	if (_weapons[_weaponIndex]->GetAmmo() == _weapons[_weaponIndex]->GetAmmoCapacity())
		return;
	if (!_playerCharacter)
		return;
	
	NetMulticastSetIsCanFire(false);
	NetMulticastReload();
}

void UCombatComponent::NetMulticastReload_Implementation()
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return;
	if (!_playerCharacter)
		return;
	
	UCDAnimInstance* bodyAnim = Cast<UCDAnimInstance>(_playerCharacter->GetMesh()->GetAnimInstance());
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());
	_isCanAim = false;
	if (bodyAnim)
		bodyAnim->PlayReloadMontage();
	if (armAnim)
		armAnim->PlayReloadMontage();
}

void UCombatComponent::ServerChangeWeapon_Implementation(int idx)
{
	//NetMulticastChangeWeapon(idx);
	_isCanFire = false;
	_isCanAim = false;
	_befIndex = _weaponIndex;
	_weaponIndex = idx;

	OnRep_WeaponID();
}

void UCombatComponent::NetMulticastChangeWeapon_Implementation(int idx)
{
	//deprecated
	if (!_playerCharacter || !_weapons[idx])
		return;
	
	_isCanFire = false;
	_isCanAim = false;
	
	UCDAnimInstance* bodyAnim = Cast<UCDAnimInstance>(_playerCharacter->GetMesh()->GetAnimInstance());
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());

	_weapons[idx]->SetHUDAmmo();
	if (bodyAnim)
	{
		bodyAnim->PlayEquipMontage(_weapons[idx]);
	}
	if (armAnim)
	{
		armAnim->PlayEquipMontage(_weapons[idx]);
	}
	_fireDelay = (_weapons[idx]->FireDelay);
}

void UCombatComponent::ServerDropWeapon_Implementation()
{
	if (!_playerCharacter)
		return;
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return;
		
	FRotator controlRot = _playerCharacter->GetControlRotation();
	FVector lookDirection = controlRot.Vector();

	NetMulticastDropWeapon(_weapons[_weaponIndex]);
	_weapons[_weaponIndex]->Dropped(lookDirection);
	_weapons[_weaponIndex] = nullptr;

	for (int i = 0; i < _weapons.Num(); i++)
	{
		if (_weapons[(_weaponIndex + i) % _weapons.Num()])
		{
			ChangeWeapon((_weaponIndex + i) % _weapons.Num());
			return;
		}
	}
}

void UCombatComponent::NetMulticastDropWeapon_Implementation(AWeapon* weapon)
{
	weapon->GetWeaponMesh()->SetVisibility(true);
	weapon->GetWeaponMesh3p()->SetVisibility(false);
}

void UCombatComponent::NetMulticastSetIsCanFire_Implementation(bool tf)
{
	_isCanFire = tf;
}

void UCombatComponent::OnRep_WeaponID()
{ //Change Weapon
	if (_weaponIndex == -1)
		return;
	
	if (!_weapons[_weaponIndex])
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			OnRep_WeaponID();
		});
		return;
	}
	if (!_playerCharacter)
		return;
	
	_weapons[_weaponIndex]->SetHUDAmmo();
	UCDAnimInstance* bodyAnim = Cast<UCDAnimInstance>(_playerCharacter->GetMesh()->GetAnimInstance());
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());

	if (bodyAnim)
	{
		bodyAnim->PlayEquipMontage(_weapons[_weaponIndex]);
	}
	if (armAnim)
	{
		armAnim->PlayEquipMontage(_weapons[_weaponIndex]);
	}
	_fireDelay = (_weapons[_weaponIndex]->FireDelay);
}

void UCombatComponent::ServerSetFireAvail_Implementation()
{
	_isCanFire = true;
}

void UCombatComponent::ServerSetAimAvail_Implementation()
{
	_isCanAim = true;
}