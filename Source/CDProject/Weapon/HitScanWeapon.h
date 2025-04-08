// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

UCLASS()
class CDPROJECT_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	AHitScanWeapon();
	virtual void Fire(const FVector& HitTarget) override;

	bool bShowSniperScope();

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ShowSniperScope();

	
private:
	UPROPERTY(EditAnywhere)
	float Damage=20.f;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamParticleSystem;

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;
	
	UPROPERTY(EditAnyWhere)
	USoundCue* FireSound;

	UPROPERTY(EditAnyWhere)
	USoundCue* HitSound;
	
};
