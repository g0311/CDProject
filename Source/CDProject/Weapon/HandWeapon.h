// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileWeapon.h"
#include "HandWeapon.generated.h"

UCLASS()
class CDPROJECT_API AHandWeapon : public AProjectileWeapon
{
	GENERATED_BODY()
//Used for Grenade or C4
public:
	// Sets default values for this actor's properties
	AHandWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Fire(const FVector& HitTarget) override;

	UPROPERTY(EditAnywhere)
	float ThrowPower=1000.f;

	void DrawTrajectory(const FVector& StartLocation, const FVector& LaunchVelocity);


	TSubclassOf<class AProjectileGrenade> Grenadeclass;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
