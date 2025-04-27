// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include <CDProject/HUD/CDHUD.h>

#include "Camera/CameraComponent.h"
#include "CDProject/Anim/CDAnimInstance.h"
#include "CDProject/Weapon/Weapon.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	//Need Line Trace For Distinguish Enemy and C4
	FHitResult Hit;
	FVector traceStart = _playerCharacter->GetCamera()->GetComponentLocation();
	FVector traceEnd = traceStart + _playerCharacter->GetCamera()->GetForwardVector() * 10000.f;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(_playerCharacter);
	if (GetWorld()->LineTraceSingleByChannel(Hit, traceStart, traceEnd, ECC_Visibility, Params))
	{
		_aimingActor = Hit.GetActor();
	}
	else
	{
		_aimingActor = nullptr;
	}

	
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
	DOREPLIFETIME(UCombatComponent, _isReloading);
}

void UCombatComponent::Reset(bool isDead)
{
	if (!_playerCharacter->HasAuthority())
		return;

	if (isDead)
	{
		//update weapon stock
		CreateDefaultWeapons();
		ChangeWeapon(1);
	}
	else
	{
		for (AWeapon* weapon : _weapons)
		{
			if (weapon)
			{
				weapon->ResetAmmo();
			}
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

EWeaponType UCombatComponent::GetCurWeaponType()
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return EWeaponType::EWT_None;

	return GetCurWeapon()->GetWeaponType();
}

void UCombatComponent::CreateDefaultWeapons()
{
	if (_defaultSubWeapon)
	{
		if (_weapons[1])
			return;
		
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
		if (_weapons[2])
			return;
		
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

FVector UCombatComponent::CreateTraceDir(float spread)
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
	
	float spreadAngleRad = FMath::DegreesToRadians(spread);
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
	
	if (GetCurWeaponType() == EWeaponType::EWT_Shotgun && _isReloading && !IsAmmoEmpty())
		ServerCancelReload();
	
	if (IsAmmoEmpty())
	{
		Aim(false);
		ServerReload();
		return;
	}
	FVector traceDir = FVector::ZeroVector;
	if (GetCurWeaponType() != EWeaponType::EWT_Hand ||
		GetCurWeaponType() != EWeaponType::EWT_C4 ||
		GetCurWeaponType() != EWeaponType::EWT_Knife)
		traceDir = CreateTraceDir(_curSpread);
	ServerFire(traceDir);
}

void UCombatComponent::RequestFireStart()
{
	if (!GetCurWeapon())
		return;
	if (GetCurWeaponType() == EWeaponType::EWT_C4)
		return;
	
	if (GetCurWeaponType() == EWeaponType::EWT_Hand)
	{
		//Grenade
		ServerReadyGrenade();
		return;
	}
	
	if (GetCurWeapon()->bAutomatic)
	{
		//Called in Client
		RequestFire();
		GetWorld()->GetTimerManager().SetTimer(_clientFireTimerHandle, this, &UCombatComponent::RequestFire, 0.01f, true);
	}
	else
	{
		RequestFire();
	}
}

void UCombatComponent::RequestFireEnd()
{
	if (!GetCurWeapon())
		return;
	if (GetCurWeaponType() == EWeaponType::EWT_C4)
		return;
	
	if (GetCurWeaponType() == EWeaponType::EWT_Hand && _isGrenadeReady)
	{
		//Grenade
		RequestFire();
		ServerThrowGrenade();
		return;
	}
	
	if (_clientFireTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(_clientFireTimerHandle);
	}
}

void UCombatComponent::RequestInteractStart()
{
	if (GetCurWeaponType() == EWeaponType::EWT_C4)
	{
		//Show HUD
		if (true /* Is Avail Location To Plant Bomb */)
			ServerC4Plant(true);
	}
	else if (true /*Is Looking C4*/)
	{
		//Show HUD
	}
}

void UCombatComponent::RequestInteractEnd()
{
	if (GetCurWeaponType() == EWeaponType::EWT_C4)
	{
		ServerC4Plant(false);
	}
}

void UCombatComponent::RequestChange(int idx)
{
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
	if (_isCanFire)
		Fire(fireDir);
}

void UCombatComponent::ServerReload_Implementation()
{
	if (!_isReloading)
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
	case EWeaponType::EWT_C4:
		if (!_weapons[5])
		{
			weapon->SetOwner(_playerCharacter);
			weapon->AttachToPlayer();
			_weapons[5] = weapon;
			ChangeWeapon(5);
		}
		break;
	}
}

void UCombatComponent::ServerC4Plant_Implementation(bool isPlanting)
{
	if (GetWorld())
	{
		if (isPlanting)
		{
			if (_playerCharacter)
			{
				_playerCharacter->GetCharacterMovement()->DisableMovement();
			}
			GetWorld()->GetTimerManager().SetTimer(_c4PlantHandle, FTimerDelegate::CreateLambda([this]
				{
					RequestFire();
					_weapons[_weaponIndex] = nullptr;
					ChangeToNextWeapon();
				}),
				_fireDelay, false);
			NetMulticastC4Plant(true);
		}
		else
		{
			if (_playerCharacter)
			{
				_playerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			}
			GetWorld()->GetTimerManager().ClearTimer(_c4PlantHandle);
			NetMulticastC4Plant(false);
		}
	}
}

void UCombatComponent::ServerC4Defuse_Implementation(bool isDefusing)
{
	
}

void UCombatComponent::ServerReadyGrenade_Implementation()
{
	NetMulticastGrenadeReady();
}

void UCombatComponent::ServerThrowGrenade_Implementation()
{
	NetMulticastGrenadeThrow();	
}

void UCombatComponent::ServerShotgunReload_Implementation()
{
	if (GetCurWeaponType() == EWeaponType::EWT_Shotgun)
	{
		GetCurWeapon()->Reload();
	}
}

void UCombatComponent::ServerCancelReload_Implementation()
{
	_isReloading = false;
	_isCanFire = true;
	_isCanAim = true;
	NetMulticastCancelReload();
}

void UCombatComponent::Aim(bool tf)
{
	if (!_isCanAim)
		return;
	
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
	{
		if (_isAiming)
		{
			_isAiming = false;
			if (GetCurWeapon() && GetCurWeapon()->GetWeaponType() == EWeaponType::EWT_Sniper)
			{
				ACDPlayerController* pc = Cast<ACDPlayerController>(_playerCharacter->GetController());	
				if(pc)
				{
					pc->ShowSniperScope();
				}
				SetWeaponVisible(true);
			}
		}
		return;
	}
	
	if (_weapons[_weaponIndex]->GetWeaponType() == EWeaponType::EWT_Rifle ||
		_weapons[_weaponIndex]->GetWeaponType() == EWeaponType::EWT_Sniper ||
		_weapons[_weaponIndex]->GetWeaponType() == EWeaponType::EWT_Speical)
	{
		if (_isAiming != tf)
		{
			_isAiming = tf;
			if (GetCurWeapon() && GetCurWeapon()->GetWeaponType() == EWeaponType::EWT_Sniper)
			{
				ACDPlayerController* pc = Cast<ACDPlayerController>(_playerCharacter->GetController());	
				if(pc)
				{
					pc->ShowSniperScope();
				}
				SetWeaponVisible(!tf);
			}
		}
	}
	else
	{
		if (_isAiming)
		{
			_isAiming = false;
			if (GetCurWeapon() && GetCurWeapon()->GetWeaponType() == EWeaponType::EWT_Sniper)
			{
				ACDPlayerController* pc = Cast<ACDPlayerController>(_playerCharacter->GetController());	
				if(pc)
				{
					pc->ShowSniperScope();
				}
				SetWeaponVisible(true);
			}
		}
	}
}

void UCombatComponent::DropAllWeapons()
{
	if (!_playerCharacter)
		return;

	Aim(false);
	for (int i = 0; i < _weapons.Num(); ++i)
	{
		if (i == 2) continue;

		if (_weapons[i])
		{
			FRotator controlRot = _playerCharacter->GetControlRotation();
			FVector lookDirection = controlRot.Vector();
			//Add Impulse
			
			NetMulticastDropWeapon(_weapons[i]);
			_weapons[i]->Dropped(lookDirection);
			_weapons[i] = nullptr;
		}
	}
	_weaponIndex = 2;
}

void UCombatComponent::ChangeToNextWeapon()
{
	for (int i = 1; i < _weapons.Num(); i++)
	{
		if (_weapons[(_weaponIndex + i) % _weapons.Num()])
		{
			ChangeWeapon((_weaponIndex + i) % _weapons.Num());
			return;
		}
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
	
	if (hit.GetActor())
		UE_LOG(LogTemp, Warning, TEXT("Server Trace Collided %s"), *hit.GetActor()->GetName());
	
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
	if (!_playerCharacter || !GetCurWeapon())
		return;
	if (GetCurWeaponType() == EWeaponType::EWT_Hand || GetCurWeaponType() == EWeaponType::EWT_Knife)
		return;
	if (_weapons[_weaponIndex]->GetAmmo() == _weapons[_weaponIndex]->GetAmmoCapacity())
		return;
	if (_weapons[_weaponIndex]->GetCarriedAmmo() == 0)
		return;
	
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());
	if (!armAnim)
		return;
	
	NetMulticastReload();
	_isCanFire = false;
	_isCanAim = false;
	_isReloading = true;
	Aim(false);
	if (GetCurWeaponType() != EWeaponType::EWT_Shotgun)
	{
		GetWorld()->GetTimerManager().SetTimer(_fireAimAbleTimerHandle, FTimerDelegate::CreateLambda([this]()
	   {
		   _isCanFire = true;
		   _isCanAim = true;
		   _isReloading = false;
		   _weapons[_weaponIndex]->Reload();
	   }), armAnim->GetReloadTime(),false);
	}
}

void UCombatComponent::ChangeWeapon(int idx)
{
	if (idx == _weaponIndex || idx < 0 || idx >= _weapons.Num() || !_weapons[idx])
	{
		return;
	}
	Aim(false);
	_befIndex = _weaponIndex;
	_weaponIndex = idx;
	
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());
	if (!armAnim)
		return;
	
	if (_fireAimAbleTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(_fireAimAbleTimerHandle);
	}
	if (_c4PlantHandle.IsValid())
	{
		if (_playerCharacter)
		{
			_playerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
		GetWorld()->GetTimerManager().ClearTimer(_c4PlantHandle);
	}
	
	_isCanFire = false;
	_isCanAim = false;
	_isChanging = true;
	_isReloading = false;
	GetWorld()->GetTimerManager().SetTimer(_fireAimAbleTimerHandle, FTimerDelegate::CreateLambda([this]
	{
		_isCanFire = true;
		_isCanAim = true;
		_isChanging = false;
	}),
	armAnim->GetEquipTime(_weapons[_weaponIndex]), false);

	if (_playerCharacter->HasAuthority())
		OnRep_WeaponID();
	//리슨 서버용
}

void UCombatComponent::DropWeapon()
{ //avail visibility and update curWeaponIndex
	if (!_playerCharacter)
		return;
	if (_weaponIndex == -1 || _weaponIndex == 2 || !_weapons[_weaponIndex])
		return;
	
	Aim(false);
	FRotator controlRot = _playerCharacter->GetControlRotation();
	FVector lookDirection = controlRot.Vector();

	NetMulticastDropWeapon(_weapons[_weaponIndex]);
	_weapons[_weaponIndex]->Dropped(lookDirection);
	_weapons[_weaponIndex] = nullptr;
	ChangeToNextWeapon();
}

void UCombatComponent::SetHUDCrosshairs(float spread)
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return;
	
	ACDCharacter* character = Cast<ACDCharacter>(GetOwner());
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
			if (_aimingActor)
			{
				ACDCharacter* aimingCharacter = Cast<ACDCharacter>(_aimingActor);
				if (aimingCharacter)
				{
					if (character->GetTeam() == ETeam::ET_NoTeam || aimingCharacter->GetTeam() != character->GetTeam())
					{
						HUDPackage.CrosshairColor = FLinearColor(1.0f, 0.f, 0.f, 1.f);
					}
				}
			}
			else
			{
				HUDPackage.CrosshairColor = FLinearColor(0.1f, 1.f, 0.f, 1.f);
			}
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

	if (GetCurWeapon()->GetWeaponType() == EWeaponType::EWT_Sniper)
	{
		Aim(false);
		//Play Sniper Action?
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

void UCombatComponent::NetMulticastGrenadeReady_Implementation()
{
	UCDAnimInstance* bodyAnim = Cast<UCDAnimInstance>(_playerCharacter->GetMesh()->GetAnimInstance());
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());
	if (bodyAnim)
		bodyAnim->PlayGrenadeReadyMontage();
	if (armAnim)
		armAnim->PlayGrenadeReadyMontage();
	if (_playerCharacter->IsLocallyControlled())
	{
		_isGrenadeReady = true;
	}
}

void UCombatComponent::NetMulticastGrenadeThrow_Implementation()
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return;
	
	UCDAnimInstance* bodyAnim = Cast<UCDAnimInstance>(_playerCharacter->GetMesh()->GetAnimInstance());
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());
	if (bodyAnim)
		bodyAnim->PlayFireMontage(_fireDelay);
	if (armAnim)
		armAnim->PlayFireMontage(_fireDelay);
	if (_playerCharacter->IsLocallyControlled())
	{
		_isGrenadeReady = false;
	}
	if (_playerCharacter->HasAuthority())
	{
		//GetWorld()->GetTimerManager().SetTimer(_clientFireTimerHandle, this, &UCombatComponent::ChangeToNextWeapon, armAnim->GetGrenadeThrowTime() / 2, false);
		GetWorld()->GetTimerManager().SetTimer(_clientFireTimerHandle, FTimerDelegate::CreateLambda([this]
			{
				_weapons[_weaponIndex] = nullptr;
				ChangeToNextWeapon();
			}), armAnim->GetGrenadeThrowTime() / 2, false);
	}
}

void UCombatComponent::NetMulticastCancelReload_Implementation()
{
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());
	UCDAnimInstance* bodyAnim = Cast<UCDAnimInstance>(_playerCharacter->GetMesh()->GetAnimInstance());

	if (armAnim && armAnim->Montage_IsPlaying(armAnim->_shotgunReloadMontage))
		armAnim->Montage_Stop(0.1f);

	if (bodyAnim && bodyAnim->Montage_IsPlaying(bodyAnim->_shotgunReloadMontage))
		bodyAnim->Montage_Stop(0.1f);
}

void UCombatComponent::NetMulticastC4Plant_Implementation(bool tf)
{
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());
	UCDAnimInstance* bodyAnim = Cast<UCDAnimInstance>(_playerCharacter->GetMesh()->GetAnimInstance());
	if (tf)
	{
		if (armAnim)
			armAnim->PlayFireMontage(_fireDelay);

		if (bodyAnim)
			bodyAnim->PlayFireMontage(_fireDelay);		
	}
	else
	{
		if (armAnim && armAnim->Montage_IsPlaying(armAnim->_shotgunReloadMontage))
			armAnim->Montage_Stop(0.1f);

		if (bodyAnim && bodyAnim->Montage_IsPlaying(bodyAnim->_shotgunReloadMontage))
			bodyAnim->Montage_Stop(0.1f);		
	}
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
	if (_isGrenadeReady)
		_isGrenadeReady = false;
	
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