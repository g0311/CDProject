// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "DataWrappers/ChaosVDParticleDataWrapper.h"

#include "Weapon.generated.h"



UENUM(BlueprintType)
enum class EWeaponState:uint8
{
	EWS_Initial UMETA(DisplayName = "Initial"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
};

UENUM(BlueprintType)
enum class EWeaponType:uint8
{
	EWT_Rifle UMETA(DisplayName = "Rifle"),
	EWT_Sniper UMETA(DisplayName = "Sniper"),
	EWT_RocketLauncher UMETA(DisplayName = "RocketLauncher"),
	EWT_Pistol UMETA(DisplayName="Pistol"),
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),
	EWT_Speical UMETA(DisplayName = "Special"),
};

UCLASS()
class CDPROJECT_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void Fire(const FVector& HitTarget);
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	//Using This function -> ProjectileWeapon

	//* Widget Set function
	void SetHUDAmmo();
	
	void SetWeaponState(EWeaponState state);
	void AddAmmo(int32 AmmoToAdd);
	void Reload();
	
	void Dropped(FVector& impactDir);
	void Picked();

	void AttachToPlayer();

	//WeaponState
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* WeaponImage;

	//Crosshair
	UPROPERTY(EditAnywhere, Category=Crosshair)
	class UTexture2D* CrosshairCenter;

	UPROPERTY(EditAnywhere, Category=Crosshair)
	UTexture2D* CrosshairRight;

	UPROPERTY(EditAnywhere, Category=Crosshair)
	UTexture2D* CrosshairLeft;

	UPROPERTY(EditAnywhere, Category=Crosshair)
	UTexture2D* CrosshairBottom;

	UPROPERTY(EditAnywhere, Category=Crosshair)
	UTexture2D* CrosshairTop;

	UPROPERTY(EditAnywhere, Category="Camera")
	float ZoomedFOV=30.f;

	UPROPERTY(EditAnywhere, Category="Camera")
	float ZoomInterpSpeed=20.f;

	UPROPERTY(EditAnywhere, Category="Weapon")
	bool bAutomatic=true;
	
	UPROPERTY(EditAnywhere, Category="Weapon")
	float FireDelay=10.f;
	
	UPROPERTY(EditAnywhere)
	class USoundCue* EquipSound;

	UPROPERTY(EditAnywhere)
	USoundCue* ReloadSound;
	
	bool AmmoIsEmpty();
	void SpendCarriedAmmo(int32 ReloadAmount);
	
	//FORCEINLINE
	FORCEINLINE USphereComponent* GetAreaComponent() const {return AreaSphere;}
	FORCEINLINE float GetZoomedFOV() const {return ZoomedFOV;}
	FORCEINLINE float GetZoomInterpSpeed() const {return ZoomInterpSpeed;}
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const {return WeaponMesh;}
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh3p() const {return WeaponMesh3p;}
	FORCEINLINE int32 GetAmmo() const {return Ammo;}
	FORCEINLINE int32 GetCarriedAmmo() const {return CarriedAmmo;}
	FORCEINLINE int32 GetAmmoCapacity() const {return AmmoCapacity;}
	FORCEINLINE EWeaponType GetWeaponType() const {return WeaponType;}
	FORCEINLINE EWeaponState GetWeaponState() const {return WeaponState;}
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing=OnRep_WeaponState,VisibleAnywhere, Category="Weapon Property")
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category="Weapon Property")
	EWeaponType WeaponType;

	UPROPERTY(VisibleAnywhere)
	class ACDCharacter* OwnerCharacter;

	UPROPERTY()
	class ACDPlayerController* OwnerController;
	
	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
		);

	UFUNCTION()
	void OnRep_Ammo();

	UFUNCTION()
	void OnRep_CarriedAmmo();
	
	UFUNCTION()
	void OnRep_WeaponState();

	

	void SpendAmmo();


private:
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* WeaponMesh3p;
	
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* AreaSphere;

	UPROPERTY(EditAnywhere, Category="Weapon Property")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACartridge> CartridgeClass;
//Ammo
	UPROPERTY(EditAnywhere,ReplicatedUsing=OnRep_Ammo)
	int32 Ammo;

	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_CarriedAmmo)
	int32 CarriedAmmo;
	
	UPROPERTY(EditAnywhere)
	int32 AmmoCapacity;



};


