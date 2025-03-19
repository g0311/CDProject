// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
<<<<<<< Updated upstream
=======

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

>>>>>>> Stashed changes

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AProjectile::Destroyed()
{
	if (ImpactParticle) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, GetActorLocation(), FRotator::ZeroRotator);
	if (ImpactSound) UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	UE_LOG(LogTemp, Warning, TEXT("Destroyed"));
	Super::Destroyed();
}


void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();//Destroy -> Destroyed
}

