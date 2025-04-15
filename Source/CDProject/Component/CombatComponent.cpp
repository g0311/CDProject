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
		//UE_LOG(LogTemp, Log, TEXT("Combat Begin"));
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
	DOREPLIFETIME(UCombatComponent, _isChanging);
}

void UCombatComponent::Reset()
{
	//update weapon stock


	//Switch Weapon
	for (int i = 0; i < _weapons.Num(); i++)
	{
		if (_weapons[i])
		{
			ChangeWeapon(i);
			break;
		}
	}
}

int UCombatComponent::GetCurAmmo()
{
	if (_weapons[_weaponIndex])
		return _weapons[_weaponIndex]->GetAmmo();
	return 0;
}

int UCombatComponent::GetCarriedAmmo()
{
	if (_weapons[_weaponIndex])
		return _weapons[_weaponIndex]->GetCarriedAmmo();
	return 0;
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
		if (_weapons[1])
		{
			//UE_LOG(LogTemp, Log, TEXT("Combat Create"));
			_weapons[1]->SetOwner(_playerCharacter);
			_weapons[1]->AttachToPlayer();
		}
	}
	if (_defaultMeleeWeapon)
	{
		_weapons[0] = GetWorld()->SpawnActor<AWeapon>(_defaultMeleeWeapon, FVector::ZeroVector, FRotator::ZeroRotator);
		if (_weapons[0])
		{
			_weapons[0]->SetOwner(_playerCharacter);
			_weapons[0]->AttachToPlayer();
		}
	}
}

float UCombatComponent::CalculateSpread()
{
	if (!_playerCharacter)
		return 0;
	
	float spread = 1.f;
	
	float Speed = _playerCharacter->GetVelocity().Size();
	spread += (Speed / MAXSPEED) * 1.8f;
	
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

FVector UCombatComponent::CreateTraceDir()
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return FVector::ZeroVector;
	if (!_playerCharacter || !_playerCharacter->GetController())
		return FVector::ZeroVector;
	APlayerController* playerController = Cast<APlayerController>(_playerCharacter->GetController());
	if (!playerController)
		return FVector::ZeroVector;
	
	FRotator camRotation = playerController->PlayerCameraManager->GetCameraRotation();
	FVector baseDirection = camRotation.Vector();
	
	float spreadAngleRad = FMath::DegreesToRadians(_curSpread);
	FVector right = FVector::CrossProduct(baseDirection, FVector::UpVector).GetSafeNormal();
	FVector up = FVector::CrossProduct(right, baseDirection).GetSafeNormal();

	float randYaw = FMath::FRandRange(-spreadAngleRad, spreadAngleRad);
	float randPitch = FMath::FRandRange(-spreadAngleRad, spreadAngleRad);

	FVector spreadDirection = baseDirection
		.RotateAngleAxis(FMath::RadiansToDegrees(randYaw), up)
		.RotateAngleAxis(FMath::RadiansToDegrees(randPitch), right)
		.GetSafeNormal();
	
	return spreadDirection;
}

void UCombatComponent::RequestFire()
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return;
	
	_isCanFire = false;
	FVector traceDir = CreateTraceDir();
	ServerFire(traceDir);
}

void UCombatComponent::RequestChange(int idx)
{
	_isChanging = true;
	ServerChangeWeapon(idx);
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

	// if (_playerCharacter->IsLocallyControlled())
	// 	UE_LOG(LogTemp,Log,TEXT("%d"), _befIndex);

	_weapons[_befIndex]->GetWeaponMesh()->SetVisibility(tf);
	_weapons[_befIndex]->GetWeaponMesh3p()->SetVisibility(tf);

	_befIndex = _weaponIndex;
}

void UCombatComponent::ServerFire_Implementation(FVector fireDir)
{
	Fire(fireDir);
}

void UCombatComponent::ServerReload_Implementation()
{
	Reload();
}

void UCombatComponent::ServerChangeWeapon_Implementation(int idx)
{
	ChangeWeapon(idx);
}

void UCombatComponent::ServerDropWeapon_Implementation()
{
	DropWeapon();
}

void UCombatComponent::ServerAim_Implementation(bool tf)
{
	Aim(tf);
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
			if (_weapons[0])
			{
				ChangeWeapon(0);
				DropWeapon();
			}
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
			if (_weapons[1])
			{
				ChangeWeapon(1);
				DropWeapon();
			}
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

void UCombatComponent::Aim(bool tf)
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
	{
		_isAiming = false;
		return;
	}
	
	if (_weapons[_weaponIndex]->GetWeaponType() == EWeaponType::EWT_Rifle ||
		_weapons[_weaponIndex]->GetWeaponType() == EWeaponType::EWT_Sniper ||
		_weapons[_weaponIndex]->GetWeaponType() == EWeaponType::EWT_Speical)
	{
		_isAiming = tf;
	}
	else
	{
		_isAiming = false;
	}
}

void UCombatComponent::Fire(FVector fireDir)
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return;
	
	FVector traceStart = _playerCharacter->GetCamera()->GetComponentLocation();
	FVector traceEnd = traceStart + fireDir * 10000.f;
	//DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor::Blue, false, 0.5);

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(GetOwner());
	queryParams.AddIgnoredActor(_weapons[_weaponIndex]);
	FHitResult hit;
	if (GetWorld()->LineTraceSingleByChannel(hit, traceStart, traceEnd, ECC_GameTraceChannel1, queryParams))
	{
		NetMulticastFire(hit.Location);
		DrawDebugSphere(GetWorld(), hit.Location, 20.f, 20, FColor::Red, false, 5.0f);
	}
	else
	{
		//DrawDebugLine(GetWorld(), traceStart, hit.Location, FColor::Red, false, 5.0f, 0, 0.5f);
		NetMulticastFire(traceEnd);
	}
	
	//Fire Delay
	_isCanFire = false;
	GetWorld()->GetTimerManager().SetTimer(_fireTimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		_isCanFire = true;
	}), _fireDelay, false);
}

void UCombatComponent::Reload()
{
	if (_weapons[_weaponIndex]->GetAmmo() == _weapons[_weaponIndex]->GetAmmoCapacity())
		return;
	if (_weapons[_weaponIndex]->GetCarriedAmmo() == 0)
		return;
	if (!_playerCharacter)	
		return;
	
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());
	if (!armAnim)
		return;
	
	NetMulticastReload();
	_isCanFire = false;
	_isCanAim = false;
	_isAiming = false;
	GetWorld()->GetTimerManager().SetTimer(_fireAimAbleTimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		_isCanFire = true;
		_isCanAim = true;
		_weapons[_weaponIndex]->Reload();
	}), armAnim->GetReloadTime(),false);
}

void UCombatComponent::ChangeWeapon(int idx)
{
	if (idx == _weaponIndex || idx < 0 || idx >= _weapons.Num() || !_weapons[idx])
	{
		return;
	}
	_isAiming = false;
	_befIndex = _weaponIndex;
	_weaponIndex = idx;
		
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());
	if (!armAnim)
		return;
	
	_isCanFire = false;
	_isCanAim = false;
	_isChanging = true;
	GetWorld()->GetTimerManager().SetTimer(_fireAimAbleTimerHandle, FTimerDelegate::CreateLambda([this]
	{
		_isCanFire = true;
		_isCanAim = true;
		_isChanging = false;
	}),
	armAnim->GetEquipTime(_weapons[_weaponIndex]), false);

	OnRep_WeaponID();
	//리슨 서버용
}

void UCombatComponent::DropWeapon()
{ //avail visibility and update curWeaponIndex
	if (!_playerCharacter)
		return;
	if (_weaponIndex == -1 || _weaponIndex == 2 || !_weapons[_weaponIndex])
		return;
	
	_isAiming = false;
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

void UCombatComponent::NetMulticastFire_Implementation(FVector target)
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return;
	
	//Except Local Player
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
		playerController->PlayerCameraManager->StartCameraShake(_fireCameraShakeClass);
	}
}

void UCombatComponent::NetMulticastReload_Implementation()
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return;
	if (!_playerCharacter)
		return;
	
	UCDAnimInstance* bodyAnim = Cast<UCDAnimInstance>(_playerCharacter->GetMesh()->GetAnimInstance());
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());
	if (bodyAnim)
		bodyAnim->PlayReloadMontage();
	if (armAnim)
		armAnim->PlayReloadMontage();
}

void UCombatComponent::NetMulticastDropWeapon_Implementation(AWeapon* weapon)
{
	weapon->GetWeaponMesh()->SetVisibility(true);
	weapon->GetWeaponMesh3p()->SetVisibility(false);
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
	} //Wait Until Weapon Replicated
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


//deprecated
void UCombatComponent::NetMulticastChangeWeapon_Implementation(int idx)
{
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

void UCombatComponent::NetMulticastSetIsCanFire_Implementation(bool tf)
{
	_isCanFire = tf;
}

void UCombatComponent::ServerSetFireAvail_Implementation()
{
	_isCanFire = true;
}

void UCombatComponent::ServerSetAimAvail_Implementation()
{
	_isCanAim = true;
}