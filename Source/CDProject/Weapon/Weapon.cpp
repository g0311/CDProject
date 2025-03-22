// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Components/WidgetComponent.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	WeaponMesh=CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);

	AreaSphere=CreateDefaultSubobject<USphereComponent>("AreaSphere");
	AreaSphere->SetupAttachment(RootComponent);

	PickupWidget=CreateDefaultSubobject<UWidgetComponent>("PickupWidget");
	PickupWidget->SetupAttachment(RootComponent);
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	DrawDebugSphere(GetWorld(), GetActorLocation(), 15, 10, FColor::Red, false);
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
	if (AreaSphere)
	{
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereBeginOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}
	UE_LOG(LogTemp, Warning, TEXT("Begin Weapon"));
	
}

void AWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ShowPickUpWidget(true);
	UE_LOG(LogTemp, Display, TEXT("Weapon Collision"));
	
	//PickUpSystem, Widget Open
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ShowPickUpWidget(false);
	//PickUpSystem, Widget Down
}



void AWeapon::Fire(const FVector& HitTarget)
{
	//Play Fire Animation
}

void AWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
}

void AWeapon::ShowPickUpWidget(bool bShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AWeapon::SetWeaponState(EWeaponState state)
{
	WeaponState=state;
	switch (WeaponState)
	{
	case EWeaponState::EWS_Dropped:
		 ShowPickUpWidget(true);
		//Collision Enable(true)
	case EWeaponState::EWS_Equipped:
		ShowPickUpWidget(false);
		//Collision  Enable(false)
	case EWeaponState::EWS_Initial:
		//Collision Enable(true)
		break;
	}
}

