// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Cartridge.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"

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

bool AWeapon::AmmoIsEmpty()
{
	return Ammo<=0;
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
}

void AWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ShowPickUpWidget(true);
	ACDCharacter* CDCharacter=Cast<ACDCharacter>(OtherActor);
	if (CDCharacter)
	{
		//CDCharacter->SetWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ShowPickUpWidget(false);
	//PickUpSystem, Widget Down
}

void AWeapon::SpendAmmo()
{
	Ammo=FMath::Clamp(Ammo-1,0,AmmoCapacity);
	SetHUDAmmo();
}


void AWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	if (CartridgeClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket=WeaponMesh->GetSocketByName("AmmoEject");
		if (AmmoEjectSocket)
		{
			FTransform AmmoEjectTransform=AmmoEjectSocket->GetSocketTransform(WeaponMesh);
			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<ACartridge>(
					ACartridge::StaticClass(),
					AmmoEjectTransform.GetLocation(),
					AmmoEjectTransform.GetRotation().Rotator()
					);
			}//Edit Need
		}
	}
}

void AWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
}

void AWeapon::ShowPickUpWidget(bool bShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AWeapon::SetHUDAmmo()
{
	OwnerCharacter=Cast<ACDCharacter>(GetOwner());
	if (OwnerCharacter)
	{
		OwnerController=Cast<ACDPlayerController>(OwnerCharacter->GetController());
		if (OwnerController)
		{
			OwnerController->SetHUDWeaponAmmo(Ammo);
		}
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

void AWeapon::AddAmmo(int32 AmmoToAdd)
{
	Ammo=FMath::Clamp(Ammo-AmmoToAdd,0,AmmoCapacity);
	SetHUDAmmo();
}
void AWeapon::Reload()
{
	OwnerCharacter=Cast<ACDCharacter>(GetOwner());
	if (OwnerCharacter==nullptr) return;
	//int CarridAmmo=OwnerCharacter->GetCarriedAmmo();
	//int32 ReloadAmount=(AmmoCapacity-Ammo, CarriedAmmo);
	//Ammo+=ReloadAmount;
	//OwnerCharacter->SpendCarriedAmmo(ReloadAmount);
	//Need(GetCarriedAmmo(), SpendCarridAmmo();)
	SetHUDAmmo();
	
}
