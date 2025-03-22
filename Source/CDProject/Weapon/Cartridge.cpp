// Fill out your copyright notice in the Description page of Project Settings.


#include "Cartridge.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


// Sets default values
ACartridge::ACartridge()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CartridgeMesh=CreateDefaultSubobject<UStaticMeshComponent>("CartridgeMesh");
	SetRootComponent(CartridgeMesh);
	CartridgeMesh->SetSimulatePhysics(true);
	CartridgeMesh->SetEnableGravity(true);
	
}

// Called when the game starts or when spawned
void ACartridge::BeginPlay()
{
	Super::BeginPlay();

	CartridgeMesh->OnComponentHit.AddDynamic(this, &ACartridge::OnHit);
}

void ACartridge::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (FallSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FallSound, GetActorLocation());
	}
}

// Called every frame
void ACartridge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

