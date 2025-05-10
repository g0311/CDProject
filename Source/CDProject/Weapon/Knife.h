// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Knife.generated.h"

UCLASS()
class CDPROJECT_API AKnife : public AWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AKnife();
	virtual void Fire(const FVector& HitTarget)override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Slash();
	
	UPROPERTY(EditAnywhere, Category="Knife")
	float SlashRange = 150.f;

	UPROPERTY(EditAnywhere, Category="Knife")
	float SlashDamage = 30.f;

	UPROPERTY(EditAnywhere, Category="Knife")
	class UAnimMontage* SlashMontage;


public:
	// Called every frame

};
