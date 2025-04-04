// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CDPROJECT_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void Reset();
	
	void Fire(float curSpread);
	void Reload();
	void Aim();
	void UnAim();
	bool ChangeWeapon(int idx);
	void GetWeapon(class AWeapon* weapon, bool isForceGet = false);
	void DropWeapon();
	void SetHUDCrosshairs(float spread);
	
	FORCEINLINE	AWeapon* GetCurWeapon() { return _weapons[_weaponIndex]; }
	FORCEINLINE	bool IsAimng() { return _isAiming; }
	FORCEINLINE	bool IsAimAvail() { return _isCanAim; }
	FORCEINLINE void SetAimAvail() { _isCanAim = true; }
	FORCEINLINE bool IsFireAvail() { return _isCanFire; }
	FORCEINLINE void SetFireAvail() { _isCanFire = true; }
	FORCEINLINE float GetFireDelay() { return _fireDelay; }
	FORCEINLINE float GetContinuedFireCount() { return _continuedFireCount; }

	bool IsAmmoEmpty();
	bool IsTotalAmmoEmpty();
	uint8 GetCurWeaponType();
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> _defaultSubWeapon;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> _defaultMeleeWeapon;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCameraShakeBase> _fireCameraShakeClass;
private:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	class ACDCharacter* _playerCharacter;
	UPROPERTY(VisibleAnywhere)
	class ACDHUD* HUD;
	
	UPROPERTY(VisibleAnywhere)
	int _weaponIndex = 0;
	
	UPROPERTY(VisibleAnywhere)
	TArray<class AWeapon*> _weapons;
	
	UPROPERTY(VisibleAnywhere)
	TSubclassOf<class AWeapon> _subWeapon;
	
	UPROPERTY(VisibleAnywhere)
	TSubclassOf<class AWeapon> _meleeWeapon;
	
	UPROPERTY(VisibleAnywhere, Replicated)
	bool _isAiming;

	float _continuedFireCount;
	
	FTimerHandle _fireTimerHandle;
	float _fireDelay = 0.23f;
	bool _isCanFire = true;
	bool _isCanAim = true;
	
	void CreateDefaultWeapons();
	void AttatchMeshToChar(class AWeapon* weapon);
};