// Fill out your copyright notice in the Description page of Project Settings.


#include "AProjectileBullet.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AAProjectileBullet::AAProjectileBullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->bRotationFollowsVelocity = true;

	ProjectileMovementComponent->InitialSpeed = 15000.f;
	ProjectileMovementComponent->MaxSpeed = 15000.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;

	bReplicates = true;
	SetReplicateMovement(true);
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
		//UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
		UGameplayStatics::ApplyPointDamage(
			OtherActor,
			Damage,
			GetActorForwardVector(),
			Hit,
			OwnerController,
			this,
			UDamageType::StaticClass()
		);
	}
	
	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}

// Called every frame
void AAProjectileBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

