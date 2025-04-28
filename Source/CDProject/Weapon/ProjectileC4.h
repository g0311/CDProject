// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileC4.generated.h"

UCLASS()
class CDPROJECT_API AProjectileC4 : public AProjectile
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileC4();
	virtual void Destroyed() override;

	FORCEINLINE float GetDefusingtime() const { return _defusingTime; }
	void Defused();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	float _defusingTime = 10.f;
};
