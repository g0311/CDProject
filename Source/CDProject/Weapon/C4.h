// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "C4.generated.h"

/**
 * 
 */
UCLASS()
class CDPROJECT_API AC4 : public AWeapon
{
	GENERATED_BODY()
public:
	AC4();

private:
	virtual void Fire(const FVector& HitTarget) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	void CancelFire();
	
	UFUNCTION()
	void Plant();
	void Explode();

	FTimerHandle _explodeTimer;
	FTimerHandle _plantTimer;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float _plantingTime = 3.f;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float _explodeTime = 10.f;
};
