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
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit);
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;
	
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticle;
	
	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;
	
private:
	UPROPERTY(EditAnywhere)
	UParticleSystem* Tracer;

	UPROPERTY(EditAnywhere)
	class UParticleSystemComponent* TracerComponent;
	

	
	
};
