// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileGrenade.generated.h"

UCLASS()
class CDPROJECT_API AProjectileGrenade : public AProjectile
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileGrenade();
	virtual void Destroyed() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult,const FVector& ImpactVelocity);

	bool IsThrow=false;
private:
	UPROPERTY(EditAnywhere)
	class USoundCue* BounceSound;
};
