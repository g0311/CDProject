// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include <CDProject/HUD/CDHUD.h>

#include "Camera/CameraComponent.h"
#include "CDProject/Anim/CDAnimInstance.h"
#include "CDProject/Weapon/Weapon.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "CDProject/Weapon/ProjectileC4.h"
#include "CDServer/Player/Team.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "CDProject/Character/CDGameplayTag.h"
#include "CDProject/GameMode/RoundGameMode.h"

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
	if (_playerCharacter)
	{
		if (_playerCharacter->IsLocallyControlled())
		{
			FHitResult Hit;
			FVector traceStart = _playerCharacter->GetCamera()->GetComponentLocation();
			FVector traceEnd = traceStart + _playerCharacter->GetCamera()->GetForwardVector() * 10000.f;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(_playerCharacter);
			if (GetWorld()->LineTraceSingleByChannel(Hit, traceStart, traceEnd, ECC_Visibility, Params))
			{
				_aimedActor = Hit.GetActor();
			}
			else
			{
				_aimedActor = nullptr;
			}
		}
	}
	
	//Update Spread
	if (_playerCharacter->HasAuthority())
	{
		float newSpread = CalculateSpread();
		_curSpread = FMath::FInterpTo(_curSpread, newSpread, DeltaTime, 50.f);
	}
	SetHUDCrosshairs(_curSpread);
	
	//If Defusing, Check the Aimed Actor
	if (_playerCharacter && _playerCharacter->IsLocallyControlled() &&
		IsInCombatState(CombatTags::State_Combat_DefusingC4))
	{
		if (!Cast<AProjectileC4>(_aimedActor))
		{
			ServerC4Defuse(false);
		}
	}

	//Update C4 Interact Time
	if (GetOwner()->HasAuthority() && bIsC4Interacting)
	{
		C4InteractTime += DeltaTime;
		if (C4InteractTime >= 5.f)
		{
			ACDCharacter* CDCharacter = Cast<ACDCharacter>(GetOwner());
			if (CDCharacter)
			{
				if (CDCharacter->GetTeam() == ETeam::ET_RedTeam)
				{
					ServerC4Plant(true);
				}
				else
				{
					ServerC4Defuse(true);
				}
			}
		}
	}
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UCombatComponent, _weapons);
	DOREPLIFETIME(UCombatComponent, _weaponIndex);
	DOREPLIFETIME(UCombatComponent, _curSpread);
	DOREPLIFETIME(UCombatComponent, _combatStateTags);
	DOREPLIFETIME(UCombatComponent, _isC4Area);
	DOREPLIFETIME(UCombatComponent, C4InteractTime);
	DOREPLIFETIME(UCombatComponent, bIsC4Interacting);
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

	if (GetWorld())
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		if (TimerManager.IsTimerActive(_clientFireTimerHandle))
		{
			TimerManager.ClearTimer(_clientFireTimerHandle);
		}
		if (TimerManager.IsTimerActive(_fireAimAbleTimerHandle))
		{
			TimerManager.ClearTimer(_fireAimAbleTimerHandle);
		}
	}
	if (IsInCombatState(CombatTags::State_Combat_DefusingC4) || IsInCombatState(CombatTags::State_Combat_PlantingC4))
	{
		ServerC4Defuse(false);
		ServerC4Plant(false);
	}
}

void UCombatComponent::DeadAction()
{
	DropAllWeapons();
	_combatStateTags.Reset();
	
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	_weaponIndex = -1;
}

void UCombatComponent::InsertCombatState(FGameplayTag StateTag)
{
	_combatStateTags.AddTag(StateTag);
}

void UCombatComponent::RemoveCombatState(FGameplayTag StateTag)
{
	_combatStateTags.RemoveTag(StateTag);
}

bool UCombatComponent::IsInCombatState(FGameplayTag StateTag) const
{
	return _combatStateTags.HasTagExact(StateTag);
}

void UCombatComponent::ExcuteMeleeAttack()
{
	RequestChange(2);
}

int UCombatComponent::GetCurAmmo()
{
	if (_weaponIndex>=0)
	{
		if (_weapons[_weaponIndex])
		{
			return _weapons[_weaponIndex]->GetAmmo();
		}
	}
		
	return 0;
}

int UCombatComponent::GetCarriedAmmo()
{
	if (_weaponIndex != -1)
	{
		if (_weapons[_weaponIndex])
		{
			return _weapons[_weaponIndex]->GetCarriedAmmo();
		}
	}
	return 0;
}

int UCombatComponent::GetAmmoCapacity()
{
	if (_weaponIndex>=-1)
	{
		if (_weapons[_weaponIndex])
		{
			return _weapons[_weaponIndex]->GetAmmoCapacity();
		}
	}
	return 0;
}

int UCombatComponent::SetAmmoCapacity(int NewAmmoCount)
{
	if (_weaponIndex>=-1)
	{
		if (_weapons[_weaponIndex])
		{
			_weapons[_weaponIndex]->SetAmmoCapacity(NewAmmoCount);
		}
	}
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
		{
			_weapons[1]->Destroy();
		}
		
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
		{
			_weapons[2]->Destroy();
		}
		
		_weapons[2] = GetWorld()->SpawnActor<AWeapon>(_defaultMeleeWeapon, FVector::ZeroVector, FRotator::ZeroRotator);
		if (_weapons[2])
		{
			if (_playerCharacter->IsLocallyControlled())
				UE_LOG(LogTemp, Log, TEXT("Combat Create"));
			_weapons[2]->SetOwner(_playerCharacter);
			_weapons[2]->AttachToPlayer();
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
	if (IsInCombatState(CombatTags::State_Combat_Aiming))
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
	if (IsInCombatState(CombatTags::State_Combat_Reloading) || 
		IsInCombatState(CombatTags::State_Combat_ChangingWeapon) || 
		IsInCombatState(CombatTags::State_Combat_DefusingC4))
	{
		return;
	}

	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return;
	
	if (GetCurWeaponType() == EWeaponType::EWT_Shotgun &&
		IsInCombatState(CombatTags::State_Combat_Reloading) &&
		!IsAmmoEmpty())
	{
		ServerCancelReload();
	}
	
	if (IsAmmoEmpty())
	{
		ServerAim(false);
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
	
	if (GetCurWeaponType() == EWeaponType::EWT_Hand && IsInCombatState(CombatTags::State_Combat_GrenadeReady))
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
		if (_isC4Area)
		{
			ServerSetC4Interact();
		}
		return;
	}
	
	if (IsValid(_aimedActor))
	{
		UE_LOG(LogTemp, Log, TEXT("%s"), *_aimedActor->GetName());
		if (AProjectileC4* c4 = Cast<AProjectileC4>(_aimedActor))
		{
			if (c4->IsDefused() || _playerCharacter->GetTeam() == ETeam::ET_RedTeam)
				return;
			
			ServerSetC4Interact();
		}
	}
}

void UCombatComponent::RequestInteractEnd()
{
	if (GetCurWeaponType() == EWeaponType::EWT_C4)
	{
		if (IsInCombatState(CombatTags::State_Combat_PlantingC4))
			ServerC4Plant(false);
		return;
	}
	
	if (_aimedActor)
	{
		AProjectileC4* plantedC4 = Cast<AProjectileC4>(_aimedActor);
		if (plantedC4 && IsInCombatState(CombatTags::State_Combat_DefusingC4))
		{
			ServerC4Defuse(false);
		}
	}
}

void UCombatComponent::RequestChange(int idx)
{
	ServerChangeWeapon(idx);
}

void UCombatComponent::SetWeaponVisible(bool tf)
{
	if (!IsValid(this))
		return;
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return;

	_weapons[_weaponIndex]->SetWeaponVisible(tf);
}

void UCombatComponent::SetBefWeaponVisible(bool tf)
{
	if (!IsValid(this))
		return;
	if (_befIndex < 0 || _befIndex > 4 || !_weapons[_befIndex] || _befIndex == _weaponIndex)
	{
		_befIndex = _weaponIndex;
		return;
	}

	_weapons[_befIndex]->SetWeaponVisible(tf);

	_befIndex = _weaponIndex;
}

ARoundGameMode* UCombatComponent::GetRoundGameMode()
{
	if (GetWorld())
	{
		if (GetWorld()->GetAuthGameMode())
		{
			if (Cast<ARoundGameMode>(GetWorld()->GetAuthGameMode()))
			{
				return Cast<ARoundGameMode>(GetWorld()->GetAuthGameMode());
			}
		}
	}
	return nullptr;
}

void UCombatComponent::ServerFire_Implementation(FVector fireDir)
{
	if (IsInCombatState(CombatTags::State_Combat_Reloading) ||
		IsInCombatState(CombatTags::State_Combat_ChangingWeapon) ||
		IsInCombatState(CombatTags::State_Combat_Firing))
		return;
	Fire(fireDir);
}

void UCombatComponent::ServerReload_Implementation()
{
	if (!IsInCombatState(CombatTags::State_Combat_Reloading))
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
		if (_playerCharacter->GetTeam() == ETeam::ET_BlueTeam)
			return;
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
		if (isPlanting && _isC4Area)
		{
			RequestFire();
			GetWeapons()[GetWeaponIndex()] = nullptr;
			ChangeToNextWeapon();
			ServerC4Plant(false);
		}
		else
		{
			bIsC4Interacting = false;
			C4InteractTime = -1.f;
			RemoveCombatState(CombatTags::State_Combat_PlantingC4);
		}
	}
}

void UCombatComponent::ServerC4Defuse_Implementation(bool isDefused)
{
	if (_playerCharacter->GetTeam() == ETeam::ET_RedTeam)
		return;

	if (GetWorld())
	{
		if (isDefused)
		{
			AProjectileC4* c4Projectile = Cast<AProjectileC4>(_aimedActor);
			if (!c4Projectile)
				return;
			c4Projectile->Defused();
			ServerC4Defuse(false);
		}
		else
		{
			bIsC4Interacting = false;
			C4InteractTime = -1.f;
			RemoveCombatState(CombatTags::State_Combat_DefusingC4);
		}
	}
}

void UCombatComponent::ServerReadyGrenade_Implementation()
{
	NetMulticastGrenadeReady();
}

void UCombatComponent::ServerThrowGrenade_Implementation()
{
	RemoveCombatState(CombatTags::State_Combat_GrenadeReady);
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
	RemoveCombatState(CombatTags::State_Combat_Reloading);
	NetMulticastCancelReload();
}

void UCombatComponent::Aim(bool tf)
{
	if (IsInCombatState(CombatTags::State_Combat_Reloading) || IsInCombatState(CombatTags::State_Combat_ChangingWeapon))
		return;
	
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
	{
		if (IsInCombatState(CombatTags::State_Combat_Aiming))
		{
			RemoveCombatState(CombatTags::State_Combat_Aiming);
		}
		return;
	}
	
	if (_weapons[_weaponIndex]->GetWeaponType() == EWeaponType::EWT_Rifle ||
		_weapons[_weaponIndex]->GetWeaponType() == EWeaponType::EWT_Sniper ||
		_weapons[_weaponIndex]->GetWeaponType() == EWeaponType::EWT_Speical)
	{
		if (IsInCombatState(CombatTags::State_Combat_Aiming) != tf)
		{
			if (tf)
				InsertCombatState(CombatTags::State_Combat_Aiming);
			else
				RemoveCombatState(CombatTags::State_Combat_Aiming);
				
			if (GetCurWeapon() && GetCurWeapon()->GetWeaponType() == EWeaponType::EWT_Sniper)
			{
				NetMulticastAim(tf);
				SetWeaponVisible(!tf);
			}
		}
	}
	else
	{
		RemoveCombatState(CombatTags::State_Combat_Aiming);	
	}
}

void UCombatComponent::DropAllWeapons()
{
	if (!_playerCharacter)
		return;

	Aim(false);
	for (int i = 0; i < _weapons.Num(); ++i)
	{
		_weaponIndex = i;
		if (_weapons[_weaponIndex])
		{
			if (_weapons[_weaponIndex]->GetWeaponType() == EWeaponType::EWT_Knife)
				continue;
			
			DropWeapon();
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

void UCombatComponent::CreateC4Weapon()
{
	if (_c4Weapon)
	{
		if (_weapons[4])
		{
			_weapons[4]->Destroy();
		}
		
		_weapons[4] = GetWorld()->SpawnActor<AWeapon>(_c4Weapon, FVector::ZeroVector, FRotator::ZeroRotator);
		if (_weapons[4])
		{
			_weapons[4]->SetOwner(_playerCharacter);
			_weapons[4]->AttachToPlayer();
		}
	}
}

void UCombatComponent::ServerSetC4Interact_Implementation()
{
	bIsC4Interacting = true;
	C4InteractTime = 0.f;
	ACDCharacter* CDCharacter = Cast<ACDCharacter>(GetOwner());
	if (CDCharacter)
	{
		if (CDCharacter->GetTeam() == ETeam::ET_RedTeam)
		{
			InsertCombatState(CombatTags::State_Combat_PlantingC4);
		}
		else
		{
			InsertCombatState(CombatTags::State_Combat_PlantingC4);
		}
	}
}

void UCombatComponent::Fire(FVector fireDir)
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return;

	FVector traceStart;
	if (_playerCharacter && _playerCharacter->GetCamera())
	{
		traceStart = _playerCharacter->GetCamera()->GetComponentLocation();
	}
	if (_playerCharacter->ActorHasTag("Bot"))
	{
		traceStart = _weapons[_weaponIndex]->GetWeaponMuzzle();
		UE_LOG(LogTemp, Warning, TEXT("AICharacter Camera Open"));
		fireDir=fireDir-traceStart;
	}
	FVector traceEnd = traceStart + fireDir * 10000.f;
	//DrawDebugLine(GetWorld(), traceStart, traceEnd, FColor::Purple, false, 0.5);

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(GetOwner());
	queryParams.AddIgnoredActor(_weapons[_weaponIndex]);
	FHitResult hit;
	
	if (hit.GetActor())
		UE_LOG(LogTemp, Warning, TEXT("Server Trace Collided %s"), *hit.GetActor()->GetName());
	
	if (GetWorld()->LineTraceSingleByChannel(hit, traceStart, traceEnd, ECC_GameTraceChannel1, queryParams))
	{
		NetMulticastFire(hit.Location);
		//DrawDebugSphere(GetWorld(), hit.Location, 20.f, 20, FColor::Red, false, 5.0f);
	}
	else
	{
		//DrawDebugLine(GetWorld(), traceStart, hit.Location, FColor::Red, false, 5.0f, 0, 0.5f);
		NetMulticastFire(traceEnd);
	}
	
	//Fire Delay
	InsertCombatState(CombatTags::State_Combat_Firing);
	GetWorld()->GetTimerManager().SetTimer(_fireTimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		if (!IsValid(this))
			return;
		RemoveCombatState(CombatTags::State_Combat_Firing);
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
	InsertCombatState(CombatTags::State_Combat_Reloading);
	Aim(false);
	if (GetCurWeaponType() != EWeaponType::EWT_Shotgun)
	{
		TWeakObjectPtr<UCombatComponent> WeakThis(this);
		GetWorld()->GetTimerManager().SetTimer(_fireAimAbleTimerHandle, FTimerDelegate::CreateLambda([WeakThis]()
	   {
			if (!WeakThis.IsValid())
				return;
		    WeakThis->RemoveCombatState(CombatTags::State_Combat_Reloading);
			if (IsValid(WeakThis->GetCurWeapon()))
			   WeakThis->GetCurWeapon()->Reload();
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
	
	RemoveCombatState(CombatTags::State_Combat_Firing);
	RemoveCombatState(CombatTags::State_Combat_GrenadeReady);
	RemoveCombatState(CombatTags::State_Combat_Reloading);
	if (IsInCombatState(CombatTags::State_Combat_PlantingC4))
	{
		ServerC4Plant(false);
	}
	if (IsInCombatState(CombatTags::State_Combat_DefusingC4))
	{
		ServerC4Defuse(false);
	}
	
	InsertCombatState(CombatTags::State_Combat_ChangingWeapon);
	TWeakObjectPtr<UCombatComponent> WeakThis(this);
	GetWorld()->GetTimerManager().SetTimer(_weaponVisibleTimerHandle, FTimerDelegate::CreateLambda([WeakThis]
	{
		if (!WeakThis.IsValid())
			return;
		UE_LOG(LogGameplayTags, Warning, TEXT("Visible called"));
		WeakThis->SetBefWeaponVisible(false);
		WeakThis->SetWeaponVisible(true);
	}),
	0.5f, false);
	
	GetWorld()->GetTimerManager().SetTimer(_weaponChangeTimerHandle, FTimerDelegate::CreateLambda([WeakThis]
	{
		if (!WeakThis.IsValid())
			return;
		WeakThis->RemoveCombatState(CombatTags::State_Combat_ChangingWeapon);
	}),
	armAnim->GetEquipTime(_weapons[_weaponIndex]), false);

	NetMulticastChangeWeapon(idx);
	// if (_playerCharacter->HasAuthority())
	// 	OnRep_WeaponID();
	// //리슨 서버용
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
	if (ARoundGameMode* gameMode = GetRoundGameMode())
	{
		gameMode->AddDestroyableActor(_weapons[_weaponIndex]);
	}
	_weapons[_weaponIndex] = nullptr;
	ChangeToNextWeapon();
}

void UCombatComponent::SetHUDCrosshairs(float spread)
{
	if (_weaponIndex == -1 || !_weapons[_weaponIndex]) return;
	
	ACDCharacter* character = Cast<ACDCharacter>(GetOwner());
	if (!character) return;

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
	if (_aimedActor)
	{
		if (ACDCharacter* aimedCharacter = Cast<ACDCharacter>(_aimedActor))
		{
			if (character->GetTeam() == ETeam::ET_NoTeam || aimedCharacter->GetTeam() != character->GetTeam())
			{
				HUDPackage.CrosshairColor = FLinearColor(1.0f, 0.f, 0.f, 1.f);
			}
		}
		else
		{
			HUDPackage.CrosshairColor = FLinearColor(0.1f, 1.f, 0.f, 1.f);
		}
	}
	else
	{
		HUDPackage.CrosshairColor = FLinearColor(0.1f, 1.f, 0.f, 1.f);
	}
	HUDPackage.CrosshairSpread=spread;
	
	OnCrossHairInfoChangedDelegate.Broadcast(HUDPackage);
}

void UCombatComponent::NetMulticastFire_Implementation(FVector target)
{
	if (!IsValid(this))
		return;
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
	if (playerController && playerController->PlayerCameraManager && _fireCameraShakeClass && GetCurWeaponType() != EWeaponType::EWT_C4)
	{
		playerController->PlayerCameraManager->StartCameraShake(_fireCameraShakeClass);
	}

	if (GetCurWeapon()->GetWeaponType() == EWeaponType::EWT_Sniper)
	{
		ServerAim(false);
	}
}

void UCombatComponent::NetMulticastReload_Implementation()
{
	if (!IsValid(this))
		return;
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
	if (!IsValid(this))
		return;
	if (!IsValid(weapon))
		return;
	weapon->GetWeaponMesh()->SetVisibility(true);
	weapon->GetWeaponMesh3p()->SetVisibility(false);
}

void UCombatComponent::NetMulticastChangeWeapon_Implementation(int idx)
{
	if (!IsValid(this))
		return;
	if (idx == -1)
		return;
	
	if (!_weapons[idx])
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this, idx]()
		{
			if (IsValid(this))
				NetMulticastChangeWeapon_Implementation(idx);
		});
		return;
	} //Wait Until Weapon Replicated
	
	_weapons[idx]->SetWeaponAmmoHUD();
	_weapons[idx]->SetWeaponInfoHUD();
	
	UCDAnimInstance* bodyAnim = Cast<UCDAnimInstance>(_playerCharacter->GetMesh()->GetAnimInstance());
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());
	
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

void UCombatComponent::NetMulticastGrenadeReady_Implementation()
{
	if (!IsValid(this))
		return;
	UCDAnimInstance* bodyAnim = Cast<UCDAnimInstance>(_playerCharacter->GetMesh()->GetAnimInstance());
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());
	if (bodyAnim)
		bodyAnim->PlayGrenadeReadyMontage();
	if (armAnim)
		armAnim->PlayGrenadeReadyMontage();

	if (_playerCharacter && _playerCharacter->HasAuthority())
	{
		TWeakObjectPtr<UCombatComponent> WeakThis(this);
		GetWorld()->GetTimerManager().SetTimer(_clientFireTimerHandle, FTimerDelegate::CreateLambda([WeakThis]
		   {
				if (!WeakThis.IsValid())
					return;
				WeakThis->InsertCombatState(CombatTags::State_Combat_GrenadeReady);
		   }), armAnim->GetGrenadeReadyTime(), false);
	}
}

void UCombatComponent::NetMulticastGrenadeThrow_Implementation()
{
	if (!IsValid(this))
		return;
	if (_weaponIndex == -1 || !_weapons[_weaponIndex])
		return;
	
	UCDAnimInstance* bodyAnim = Cast<UCDAnimInstance>(_playerCharacter->GetMesh()->GetAnimInstance());
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());
	if (bodyAnim)
		bodyAnim->PlayFireMontage(_fireDelay);
	if (armAnim)
		armAnim->PlayFireMontage(_fireDelay);

	if (_playerCharacter && _playerCharacter->HasAuthority())
	{
		TWeakObjectPtr<UCombatComponent> WeakThis(this);
		GetWorld()->GetTimerManager().SetTimer(_clientFireTimerHandle, FTimerDelegate::CreateLambda([WeakThis]
		   {
				if (!WeakThis.IsValid())
					return;
				WeakThis->GetWeapons()[WeakThis->GetWeaponIndex()] = nullptr;
				WeakThis->ChangeToNextWeapon();
		   }), armAnim->GetGrenadeThrowTime() / 2, false);
	}
}

void UCombatComponent::NetMulticastCancelReload_Implementation()
{
	if (!IsValid(this))
		return;
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(_playerCharacter->GetArmMesh()->GetAnimInstance());
	UCDAnimInstance* bodyAnim = Cast<UCDAnimInstance>(_playerCharacter->GetMesh()->GetAnimInstance());

	if (armAnim && armAnim->Montage_IsPlaying(armAnim->_shotgunReloadMontage))
		armAnim->Montage_Stop(0.1f);

	if (bodyAnim && bodyAnim->Montage_IsPlaying(bodyAnim->_shotgunReloadMontage))
		bodyAnim->Montage_Stop(0.1f);
}

void UCombatComponent::NetMulticastAim_Implementation(bool IsAiming)
{
	OnScopeUIChangedDelegate.Broadcast(IsAiming, false);
}

void UCombatComponent::OnRep_C4InteractTime()
{
	C4InteractDelegate.Broadcast(C4InteractTime);
}
