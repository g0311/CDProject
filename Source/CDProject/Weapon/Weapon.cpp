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
	WeaponMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	WeaponMesh->SetVisibility(false);
	
	WeaponMesh3p = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh3p"));
	//WeaponMesh3p->SetupAttachment(RootComponent); 
	
	WeaponMesh3p->SetOnlyOwnerSee(false);
	WeaponMesh3p->SetOwnerNoSee(true);
	WeaponMesh3p->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh3p->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh3p->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh3p->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	WeaponMesh3p->SetVisibility(false);
	
	AreaSphere=CreateDefaultSubobject<USphereComponent>("AreaSphere");
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
	if (AreaSphere && HasAuthority())
	{
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereBeginOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}
}

void AWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
		return;
		
	ACDCharacter* CDCharacter=Cast<ACDCharacter>(OtherActor);
	if (WeaponState ==  EWeaponState::EWS_Dropped && CDCharacter)
	{
		CDCharacter->GetWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
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
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		break;
	case EWeaponState::EWS_Equipped:
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		break;
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
		if (OwnerCharacter->IsLocallyControlled())
			WeaponMesh->PlayAnimation(FireAnimation, false);
		else
			WeaponMesh3p->PlayAnimation(FireAnimation, false);
	}
	if (CartridgeClass)
	{
		if (OwnerCharacter->IsLocallyControlled())
		{
			const USkeletalMeshSocket* AmmoEjectSocket=WeaponMesh->GetSocketByName("AmmoEject");
			if (AmmoEjectSocket)
			{
				FTransform AmmoEjectTransform=AmmoEjectSocket->GetSocketTransform(WeaponMesh);
				UWorld* World = GetWorld();
				if (World)
				{
					ACartridge* Cartridge = World->SpawnActor<ACartridge>(
						CartridgeClass,
						AmmoEjectTransform.GetLocation(),
						AmmoEjectTransform.GetRotation().Rotator()
					);
					// if (Cartridge)
					// {
					// 	UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Cartridge->GetRootComponent());
					// 	if (PrimitiveComp && PrimitiveComp->IsSimulatingPhysics())
					// 	{
					// 		FVector Impulse = FVector(-150.f, 100.f, 150.f);
					// 		PrimitiveComp->AddImpulse(Impulse, NAME_None, true);
					// 	}
					// }
				}
				//Edit Need
			}
		}
		else
		{
			const USkeletalMeshSocket* AmmoEjectSocket=WeaponMesh3p->GetSocketByName("AmmoEject");
			if (AmmoEjectSocket)
			{
				FTransform AmmoEjectTransform=AmmoEjectSocket->GetSocketTransform(WeaponMesh3p);
				UWorld* World = GetWorld();
				if (World)
				{
					ACartridge* Cartridge = World->SpawnActor<ACartridge>(
						CartridgeClass,
						AmmoEjectTransform.GetLocation(),
						AmmoEjectTransform.GetRotation().Rotator()
					);
					if (Cartridge)
					{
						UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(Cartridge->GetRootComponent());
						if (PrimitiveComp && PrimitiveComp->IsSimulatingPhysics())
						{
							FVector Impulse = FVector(-150.f, 100.f, 150.f);
							PrimitiveComp->AddImpulse(Impulse, NAME_None, true);
						}
					}
				}
				//Edit Need
			}
		}
	}
	SpendAmmo();
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
		GetWeaponMesh()->SetOnlyOwnerSee(false);

		FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
		DetachFromActor(DetachRules);
		WeaponMesh->DetachFromComponent(DetachRules);
		WeaponMesh3p->DetachFromComponent(DetachRules);
		OwnerCharacter=nullptr;
		OwnerController=nullptr;
	}
	else
	{
		AttachToPlayer();
		SetHUDAmmo();
	}
}

void AWeapon::Dropped(FVector& impactDir)
{
	SetOwner(nullptr);

	//Server Set (On Rep)
	GetWeaponMesh()->SetOnlyOwnerSee(false);
	
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	DetachFromActor(DetachRules);
	WeaponMesh->DetachFromComponent(DetachRules);
	WeaponMesh3p->DetachFromComponent(DetachRules);
	OwnerCharacter=nullptr;
	OwnerController=nullptr;

	SetWeaponState(EWeaponState::EWS_Dropped);
	
	//Add Impulse
	if (WeaponMesh->IsSimulatingPhysics())
	{
		FVector Impulse = impactDir.GetSafeNormal() * 500.0f;
		WeaponMesh->AddImpulse(Impulse, NAME_None, true);
	}
}

void AWeapon::AttachToPlayer()
{
	OwnerCharacter = Cast<ACDCharacter>(Owner);
	if (!OwnerCharacter)
		return;
	OwnerController = Cast<ACDPlayerController>(OwnerCharacter->Controller);
	
	SetWeaponState(EWeaponState::EWS_Equipped);

	AttachToComponent(
		OwnerCharacter->GetArmMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		TEXT("WeaponSocket")
	);
	GetWeaponMesh()->SetOnlyOwnerSee(true);
	
	GetWeaponMesh3p()->AttachToComponent(
		OwnerCharacter->GetMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		TEXT("WeaponSocket")
	);
	GetWeaponMesh3p()->SetOwnerNoSee(true);
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
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		break;
	case EWeaponState::EWS_Equipped:
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
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
	 CarriedAmmo=GetCarriedAmmo();
	 int32 AmmoNeed=AmmoCapacity-Ammo;
	 int32 ReloadAmount=FMath::Min(AmmoNeed, CarriedAmmo);
	 Ammo+=ReloadAmount;
	 SpendCarriedAmmo(ReloadAmount);
	 SetHUDAmmo();
}
