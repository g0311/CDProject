// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitScanWeapon.h"
#include "Shotgun.generated.h"

UCLASS()
class CDPROJECT_API AShotgun : public AHitScanWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AShotgun();
	virtual void Fire(const FVector& HitTraget) override;
	

public:
	UPROPERTY(EditAnywhere)
	uint32 NumberOfPellets=10;
};
