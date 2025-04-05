// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "Cartridge.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates=true;
	SetReplicateMovement(true);
	
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	
	WeaponMesh->SetOnlyOwnerSee(true);
	WeaponMesh->SetOwnerNoSee(false);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	
	WeaponMesh3p = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh3p"));
	//WeaponMesh3p->SetupAttachment(RootComponent); 
	
	WeaponMesh3p->SetOnlyOwnerSee(false);
	WeaponMesh3p->SetOwnerNoSee(true);
	WeaponMesh3p->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh3p->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh3p->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh3p->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	
	AreaSphere=CreateDefaultSubobject<USphereComponent>("AreaSphere");
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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

void AWeapon::SpendCarriedAmmo(int32 ReloadAmount)
{
	CarriedAmmo = FMath::Max(CarriedAmmo - ReloadAmount, 0);
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
	if (WeaponState ==  EWeaponState::EWS_Dropped && CDCharacter)
	{
		CDCharacter->GetWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ShowPickUpWidget(false);
	//PickUpSystem, Widget Down
}

void AWeapon::OnRep_Ammo()
{
	SetHUDAmmo();
}

void AWeapon::OnRep_CarriedAmmo()
{
	SetHUDAmmo();
}

void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Dropped:
		ShowPickUpWidget(false);
		WeaponMesh->SetEnableGravity(true);
		// WeaponMesh->SetVisibility(true);
		// WeaponMesh3p->SetVisibility(true);
	case EWeaponState::EWS_Equipped:
		ShowPickUpWidget(false);
		WeaponMesh->SetEnableGravity(false);
	}
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
					CartridgeClass,
					AmmoEjectTransform.GetLocation(),
					AmmoEjectTransform.GetRotation().Rotator()
					);
			}//Edit Need
		}
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon, Ammo);
	DOREPLIFETIME(AWeapon, CarriedAmmo);
	DOREPLIFETIME(AWeapon, WeaponState);
	
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner==nullptr)
	{
		OwnerCharacter=nullptr;
		OwnerController=nullptr;
	}
	else
	{
	
		SetHUDAmmo();
	}
}

void AWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	WeaponMesh3p->DetachFromComponent(DetachRules);
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
	if (OwnerCharacter == nullptr)
	{
		OwnerCharacter = Cast<ACDCharacter>(GetOwner());
	}
	if (OwnerCharacter && OwnerController == nullptr)
	{
		OwnerController = Cast<ACDPlayerController>(OwnerCharacter->GetController());
	}
	if (OwnerController)
	{
		OwnerController->SetHUDWeaponAmmo(Ammo);
		OwnerController->SetHUDWeaponInfo(this);
	}
}


void AWeapon::SetWeaponState(EWeaponState state)
{
	WeaponState=state;
	switch (WeaponState)
	{
	case EWeaponState::EWS_Dropped:
		ShowPickUpWidget(true);
		WeaponMesh->SetEnableGravity(true);
		//Collision Enable(true)
	case EWeaponState::EWS_Equipped:
		ShowPickUpWidget(false);
		WeaponMesh->SetEnableGravity(false);
		//Collision  Enable(false)
	}
}

void AWeapon::AddAmmo(int32 AmmoToAdd)
{
	Ammo=FMath::Clamp(Ammo-AmmoToAdd,0,AmmoCapacity);
	SetHUDAmmo();
}
void AWeapon::Reload()
{
	 CarriedAmmo=GetCarriedAmmo();
	 int32 AmmoNeed=AmmoCapacity-Ammo;
	 int32 ReloadAmount=FMath::Min(AmmoNeed, CarriedAmmo);
	 Ammo+=ReloadAmount;
	 SpendCarriedAmmo(ReloadAmount);
	 SetHUDAmmo();
}
