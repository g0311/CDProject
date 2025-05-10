// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "C4Weapon.generated.h"

/**
 * 
 */
UCLASS()
class CDPROJECT_API AC4Weapon : public AWeapon
{
	GENERATED_BODY()
public:
	AC4Weapon();
	FORCEINLINE float GetPlantTime() {return _plantingTime;}
	
private:
	virtual void Fire(const FVector& HitTarget) override;
	float _plantingTime = 3.f;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TSubclassOf<class AProjectile> _projectileClass;
	
	UPROPERTY(EditAnywhere)
	USoundBase* _plantedSound;
};
