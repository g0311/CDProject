// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include <CDProject/HUD/CDHUD.h>

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
		ChangeWeapon(1);
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	//Update Spread
	if (_playerCharacter->HasAuthority())
	{
		float newSpread = CaculateSpread();
		_continuedFireCount = FMath::FInterpTo(_continuedFireCount, 0.f, DeltaTime, 3.f);
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
	DOREPLIFETIME(UCombatComponent, _curSpread);
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
		ServerFire();
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
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return;
	
	if (_weapons[_weaponIndex]->GetWeaponType() != EWeaponType::EWT_Speical)
	{
		_isAiming = false;
	}
}

bool UCombatComponent::ChangeWeapon(int idx)
{ //avail visibility and update curWeaponIndex
	if (idx == _weaponIndex)
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
			weapon->SetHUDAmmo();
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
			weapon->SetHUDAmmo();
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
	if (_weaponIndex != -1 && _weapons[_weaponIndex])
		return _weapons[_weaponIndex];
	return nullptr;
}

bool UCombatComponent::IsAmmoEmpty()
{
	return _weapons[_weaponIndex]->AmmoIsEmpty();
}

bool UCombatComponent::IsTotalAmmoEmpty()
{
	return false;
}

uint8 UCombatComponent::GetCurWeaponType()
{
	if (_weaponIndex != -1 && _weapons[_weaponIndex])
		return static_cast<uint8>(_weapons[_weaponIndex]->GetWeaponType());
	return -1;
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

float UCombatComponent::CaculateSpread()
{
	if (!_playerCharacter)
		return 0;
	
	float spread = 1.0f;
	
	float Speed = _playerCharacter->GetVelocity().Size();
	spread += (Speed / 470.f) * 1.8f; //MaxSpeed
	
	if (_playerCharacter->GetMovementComponent()->IsFalling())
	{
		spread += 3.f;
	}
	if (_playerCharacter->bIsCrouched)
	{
		spread -= 0.3f;  // 앉으면 감소
	}
	if (_isAiming)
	{
		spread -= 0.3f;
	}
	float continuouedFireFactor = FMath::Clamp(_continuedFireCount * 3 / 5.0f /* x Weapon Spread */, 0.f, 3.f); 
	spread += continuouedFireFactor;
	
	return FMath::Clamp(spread, 0.4f, 5.f);
}

void UCombatComponent::SetWeaponVisible(bool tf)
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return;
	
	_weapons[_weaponIndex]->GetWeaponMesh()->SetVisibility(tf);
	_weapons[_weaponIndex]->GetWeaponMesh3p()->SetVisibility(tf);
}

void UCombatComponent::ServerFire_Implementation()
{
	//Fire Delay
	NetMulticastSetIsCanFire(false);
	GetWorld()->GetTimerManager().SetTimer(_fireTimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		NetMulticastSetIsCanFire(true);
	}), _fireDelay, false);
	_continuedFireCount++;
	
	//Trace
	APlayerController* playerController = Cast<APlayerController>(_playerCharacter->GetController());
	if (!playerController) return;

	FVector CameraLocation;
	FRotator CameraRotation;
	playerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

	int32 ViewportSizeX, ViewportSizeY;
	playerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
    
	float CrosshairSpread = _curSpread * 16.f;

	FVector2D CrosshairScreenPosition(
		(ViewportSizeX / 2.f) + FMath::RandRange(-CrosshairSpread, CrosshairSpread),
		(ViewportSizeY / 2.f) + FMath::RandRange(-CrosshairSpread, CrosshairSpread)
	);

	FVector CrosshairWorldPosition, CrosshairWorldDirection;
	if (playerController->DeprojectScreenPositionToWorld(
			CrosshairScreenPosition.X,
			CrosshairScreenPosition.Y,
			CrosshairWorldPosition,
			CrosshairWorldDirection))
	{
		FVector traceStart = CameraLocation;
		FVector traceEnd = traceStart + (CrosshairWorldDirection * 10000.f);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetOwner());
		QueryParams.AddIgnoredActor(_weapons[_weaponIndex]);

		FHitResult hit;
		if (GetWorld()->LineTraceSingleByChannel(hit, traceStart, traceEnd, ECC_Visibility, QueryParams))
		{
			if (hit.GetActor() && hit.GetActor()->Implements<UIsEnemyInterface>())
				DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor::Red, false, 2.0f, 0, 0.1f);
			else
				DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor::Green, false, 2.0f, 0, 0.1f);
			// DrawDebugSphere(GetWorld(), hit.Location, 10.f, 
			// 			12,   
			// 			FColor::Green,
			// 			false, 
			// 			5.0f,  
			// 			0,
			// 			2.0f);
			// if (hit.GetActor())
			// 	UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *hit.GetActor()->GetName());
			// if (hit.GetActor())
			// 	UE_LOG(LogTemp, Warning, TEXT("Hit Component: %s"), *hit.GetComponent()->GetName());
			NetMulticastFire(hit.Location);
		}
	}
}

void UCombatComponent::NetMulticastFire_Implementation(FVector target)
{
	if (!_playerCharacter)
		return;

	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
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
	if (!_playerCharacter)
		return;
	//isammo full => return
	//if (_weapons[_weaponIndex]->ammo)
	
	NetMulticastSetIsCanFire(false);
	NetMulticastReload();
}

void UCombatComponent::NetMulticastReload_Implementation()
{
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
	NetMulticastChangeWeapon(idx);
}

void UCombatComponent::NetMulticastChangeWeapon_Implementation(int idx)
{
	if (!_playerCharacter)
		return;
	
	if (_weaponIndex != -1 && _weapons[_weaponIndex])
	{
		SetWeaponVisible(false);
	}
	
	_weaponIndex = idx;
	SetWeaponVisible(true);
	
	UCDAnimInstance* bodyAnim = Cast<UCDAnimInstance>(_playerCharacter->GetMesh()->GetAnimInstance());
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());

	if (bodyAnim)
		bodyAnim->PlayEquipMontage();
	if (armAnim)
		armAnim->PlayEquipMontage();
	
	_fireDelay = (_weapons[_weaponIndex]->FireDelay);
	_fireDelay = 0.1f;
	
	_isCanFire = true;
	_isCanAim = true;
}

void UCombatComponent::NetMulticastSetIsCanFire_Implementation(bool tf)
{
	_isCanFire = tf;
}

void UCombatComponent::ServerDropWeapon_Implementation()
{
	if (!_playerCharacter)
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

void UCombatComponent::OnRep_WeaponID()
{
	SetWeaponVisible(true);
}

void UCombatComponent::OnRep_Weapons()
{
	SetWeaponVisible(true);
}

void UCombatComponent::ServerSetFireAvail_Implementation()
{
	_isCanFire = true;
}

void UCombatComponent::ServerSetAimAvail_Implementation()
{
	_isCanAim = true;
}
