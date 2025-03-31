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
	void Reset();
	
	void Fire();
	void Reload();
	void Aim(bool tf);
	bool ChangeWeapon(int idx);
	void GetWeapon(class AWeapon* weapon, bool isForceGet = false);
	void DropWeapon();
	
	bool IsAmmoEmpty();
	AWeapon* GetCurWeapon();
	uint8 GetCurWeaponType();
	bool IsAimng();
	bool IsFireAvail();

	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> _defaultSubWeapon;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> _defaultMeleeWeapon;
private:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	int _weaponIndex;
	
	UPROPERTY(VisibleAnywhere)
	TArray<class AWeapon*> _weapons;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class AWeapon> _subWeapon;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class AWeapon> _meleeWeapon;
	
	UPROPERTY(VisibleAnywhere)
	bool _isAiming;

	float FireRate = 0.23f;
	float LastFireTime = 0.0f;
	
	void CreateDefaultWeapons();
	void AttatchMeshToChar(class AWeapon* weapon);

	void SetHUDCrosshairs();
};