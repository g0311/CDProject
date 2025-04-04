// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include <CDProject/HUD/CDHUD.h>

#include "CDProject/Anim/CDAnimInstance.h"
#include "CDProject/Weapon/Weapon.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "Net/UnrealNetwork.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	//0: main / 1: sub / 2: melee / 3,4,5: projectile
	_weapons.SetNumZeroed(6);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	SetIsReplicated(true);
	_playerCharacter = Cast<ACDCharacter>(GetOwner());
	CreateDefaultWeapons();
	
	ChangeWeapon(1);
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UCombatComponent, _isAiming);
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
	if (!_playerCharacter)
		return;

	_isCanFire = false;
	GetWorld()->GetTimerManager().SetTimer(_fireTimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		_isCanFire = true;
	}), _fireDelay, false);
	
	UCDAnimInstance* bodyAnim = Cast<UCDAnimInstance>(_playerCharacter->GetMesh()->GetAnimInstance());
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());

	if (bodyAnim)
		bodyAnim->PlayFireMontage(_fireDelay);
	if (armAnim)
		armAnim->PlayFireMontage(_fireDelay);
	
	FHitResult hit;
	AController* controller= Cast<ACharacter>(GetOwner())->Controller;
	if (controller)
	{
		FVector CameraLocation;
		FRotator CameraRotation;
		controller->GetPlayerViewPoint(CameraLocation, CameraRotation);
		
		FVector traceStart = CameraLocation;
		FVector traceEnd = traceStart + (CameraRotation.Vector() * 10000.f);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetOwner()); // 플레이어 자신 제외

		if(GetWorld()->LineTraceSingleByChannel(hit, traceStart, traceEnd, ECC_Visibility, QueryParams))
			_weapons[_weaponIndex]->Fire(hit.Location);
	}
}

void UCombatComponent::Reload()
{
	//isammo full => return
	
	UCDAnimInstance* bodyAnim = Cast<UCDAnimInstance>(_playerCharacter->GetMesh()->GetAnimInstance());
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());

	_isCanFire = false;
	_isCanAim = false;
	if (bodyAnim)
		bodyAnim->PlayReloadMontage();
	if (armAnim)
		armAnim->PlayReloadMontage();
}

void UCombatComponent::Aim()
{
	if (_weapons[_weaponIndex]->GetWeaponType() != EWeaponType::EWT_Speical)
	{
		_isAiming = true;
	}
}

void UCombatComponent::UnAim()
{
	if (_weapons[_weaponIndex]->GetWeaponType() != EWeaponType::EWT_Speical)
	{
		_isAiming = false;
	}
}

bool UCombatComponent::ChangeWeapon(int idx)
{ //avail visibility and update curWeaponIndex
	if (idx == _weaponIndex)
		return false;
	
	UCDAnimInstance* bodyAnim = Cast<UCDAnimInstance>(_playerCharacter->GetMesh()->GetAnimInstance());
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());
	if (_weapons[idx])
	{
		if (_weapons[_weaponIndex])
		{
			_weapons[_weaponIndex]->GetWeaponMesh()->SetVisibility(false);
			//_weapons[_weaponIndex]->GetSecondWeaponMesh()->SetVisibility(false);
			_isAiming = false;
		}
		_weaponIndex = idx;
		_weapons[_weaponIndex]->GetWeaponMesh()->SetVisibility(true);
		//_weapons[_weaponIndex]->GetSecondWeaponMesh()->SetVisibility(true);
		
		_fireDelay = (_weapons[_weaponIndex]->FireDelay);
		
		if (bodyAnim)
		{
			bodyAnim->PlayEquipMontage();
		}
		if (armAnim)
		{
			armAnim->PlayEquipMontage();
		}

		_isCanFire = true;
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
		ChangeWeapon(_weaponIndex);
	}
}

void UCombatComponent::SetHUDCrosshairs(float spread)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());
	if (!character || !character->Controller) return;

	ACDPlayerController* controller = Cast<ACDPlayerController>(character->Controller);
	if (controller)
	{
		HUD = HUD == nullptr ? Cast<ACDHUD>(controller->GetHUD()) : HUD;
		if (HUD)
		{
			FHUDPackage HUDPackage;
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
			HUD->SetHUDPackage(HUDPackage);
		}
	}
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
	if (_weapons[_weaponIndex])
		return static_cast<uint8>(_weapons[_weaponIndex]->GetWeaponType());
	return -1;
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
			owner->GetArmMesh(),
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			TEXT("WeaponSocket")
			);
			_weapons[1]->GetWeaponMesh()->SetVisibility(true);
			
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
			owner->GetArmMesh(),
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			TEXT("WeaponSocket")
			);
			_weapons[2]->GetWeaponMesh()->SetVisibility(false);
			
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


/*
void 
UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine&&GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	FVector2D CrossHairLocation(ViewportSize.X/2.f,ViewportSize.Y/2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld=UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this,0),
		CrossHairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);//DeprojectScreenToWorld = ScreenPosition -> WorldPosition, WorldDirection 변환 역할
	if (bScreenToWorld)
	{
		FVector Start=CrosshairWorldPosition;
		DrawDebugSphere(GetWorld(), Start, 0.5f, 20, FColor::Yellow, false);
		if (Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start+=CrosshairWorldDirection*(DistanceToCharacter+100.f);
		}
		FVector End=Start+CrosshairWorldDirection*TRACE_LENGTH;
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
			);
 */