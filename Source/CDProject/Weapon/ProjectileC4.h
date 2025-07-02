// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectileC4.generated.h"

UCLASS()
class CDPROJECT_API AProjectileC4 : public AProjectile
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileC4();
	virtual void Destroyed() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	void Defused();
	
	FORCEINLINE float GetDefusingtime() const { return _defusingTime; }
	FORCEINLINE float IsDefused() const { return _isDefused; }
protected:
	UFUNCTION(NetMulticast, Unreliable)
	void NetMulticastPlayDefuseSound();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void FinishedDestroyTimer() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastCreateExplodeEffect();
	
	UPROPERTY(EditAnywhere)
	float _defusingTime = 10.f;

	UPROPERTY(VisibleAnywhere, Replicated)
	bool _isDefused = false;
	
	UPROPERTY(EditAnywhere)
	USoundBase* _defuseSound;
};

