// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "AProjectileBullet.generated.h"

UCLASS()
class CDPROJECT_API AAProjectileBullet : public AProjectile
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAProjectileBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void Tick(float DeltaTime) override;
};
