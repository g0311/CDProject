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
	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);
	
	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnyWhere)
	USoundCue* HitBodySound;

	UPROPERTY(EditAnyWhere)
	USoundCue* HitSurfaceSound;

	UPROPERTY(EditAnywhere)
	float Damage=20.f;

	

	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget,FHitResult& OutHit);
private:
	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamParticleSystem;

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash;
	
	UPROPERTY(EditAnyWhere)
	USoundCue* FireSound;
	
	UPROPERTY(EditAnyWhere)
	float DistanceToSphere=800.f;
	
	UPROPERTY(EditAnyWhere)
	float SphereRadius=75.f;
	
	UPROPERTY(EditAnyWhere)
	bool bUseScatter=false;
	
};
