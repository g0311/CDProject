// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class CDPROJECT_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();
	virtual void Destroyed() override;//Pawn Cascade

	UPROPERTY(EditAnywhere)
	float InitialSpeed=15000.f;

	UPROPERTY(VisibleAnywhere)
	float Damage=10.f;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;
	//Niagara
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TrailSystem;

	UPROPERTY()
	class UNiagaraComponent* TrailSystemComponent;
	
	//Impact Particle
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticle;
	
	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;
	//
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere)
	float DamageInnerRadius=200.f;
	
	UPROPERTY(EditAnywhere)
	float DamageOuterRadius=500.f;
	
	
protected:
	virtual void BeginPlay() override;

	void StartDestroyTimer();
	void FinishedDestroyTimer();
	void SpawnTrailSystem();
	void ExplodeDamage();

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit);
	

	//
private:
	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
	float DestroyTime=3.f;
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* Tracer;

	UPROPERTY(EditAnywhere)
	class UParticleSystemComponent* TracerComponent;
	

	
	
};
