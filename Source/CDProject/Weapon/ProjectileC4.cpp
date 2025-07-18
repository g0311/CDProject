// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileC4.h"

#include "CDProject/GameMode/RoundGameMode.h"
#include "CDProject/GameMode/DemolitionGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"


// Sets default values
AProjectileC4::AProjectileC4()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("C4 Mesh");
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	bReplicates = true;
}

void AProjectileC4::Destroyed()
{
	if (HasAuthority())
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(DestroyTimer))
		{
			GetWorld()->GetTimerManager().ClearTimer(DestroyTimer);
		}
	}
	Super::Super::Destroyed();
}

void AProjectileC4::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProjectileC4, _isDefused);
}

void AProjectileC4::Defused()
{
	if (HasAuthority())
	{
		_isDefused = true;
		if (DestroyTimer.IsValid() && GetWorld()->GetTimerManager().IsTimerActive(DestroyTimer))
		{
			GetWorld()->GetTimerManager().ClearTimer(DestroyTimer);
		}

		if (GetWorld()->GetAuthGameMode())
        {
        	ADemolitionGameMode* DemolitionGameMode = Cast<ADemolitionGameMode>(GetWorld()->GetAuthGameMode());
        	if (DemolitionGameMode && DemolitionGameMode->GetCurMatchState() != ECurMatchState::EMS_CoolDown)
        	{
        		//DemolitionGameMode->SetC4Planted(false);
        		DemolitionGameMode->SetC4Defused();
        	}
        }
	}
	NetMulticastPlayDefuseSound();
}

void AProjectileC4::NetMulticastPlayDefuseSound_Implementation()
{
	if (!IsValid(this))
		return;
	if (IsValid(_defuseSound))
		UGameplayStatics::PlaySound2D(this, _defuseSound);
}

// Called when the game starts or when spawned
void AProjectileC4::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnTrailSystem();
	if (HasAuthority())
		StartDestroyTimer();
}

void AProjectileC4::FinishedDestroyTimer()
{
	ExplodeDamage();
	NetMulticastCreateExplodeEffect();
	
	Super::FinishedDestroyTimer();
}

// Called every frame
void AProjectileC4::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectileC4::NetMulticastCreateExplodeEffect_Implementation()
{
	if (!IsValid(this))
		return;
	
	if (ImpactParticle) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, GetActorLocation(), FRotator::ZeroRotator);
	if (ImpactSound) UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
}