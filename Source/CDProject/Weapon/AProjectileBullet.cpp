// Fill out your copyright notice in the Description page of Project Settings.


#include "AProjectileBullet.h"

#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AAProjectileBullet::AAProjectileBullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	Movement->bRotationFollowsVelocity = true;
	Movement->InitialSpeed = 15000;
	Movement->MaxSpeed = 15000;
}

// Called when the game starts or when spawned
void AAProjectileBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAProjectileBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	ACharacter* OwnerCharacter=Cast<ACharacter>(GetOwner());
	AController* OwnerController=OwnerCharacter->GetController();
	if (OwnerCharacter && OwnerController)
	{
		UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
	}
	
	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}

// Called every frame
void AAProjectileBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

