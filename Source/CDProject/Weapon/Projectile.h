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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere)
	float Damage=10.f;
	//be Adjusted by Weapon's Damage

	
private:

	
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* Movement;

	class UParticleSystemComponent* TracerComponent;
	UPROPERTY(EditAnywhere)
	UParticleSystem* Tracer;

	UParticleSystemComponent* ImpactParticleComponent;
	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticle;

	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;
};
