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
	//Using This function -> ProjectileWeapon

	//* Widget Set function
	void ShowPickUpWidget(bool bShowWidget);
	void SetHUDAmmo();
	
	void SetWeaponState(EWeaponState state);
	void AddAmmo(int32 AmmoToAdd);

	void Dropped();
	void Picked();

	
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

	bool AmmoIsEmpty();

	//FORCEINLINE
	FORCEINLINE USkeletalMeshComponent* GetSkeletalMeshComponent() const {return WeaponMesh;}
	FORCEINLINE USphereComponent* GetSphereComponent() const {return AreaSphere;}
	FORCEINLINE float GetZoomedFOV() const {return ZoomedFOV;}
	FORCEINLINE float GetZoomInterpSpeed() const {return ZoomInterpSpeed;}
	FORCEINLINE EWeaponType GetWeaponType() const {return WeaponType;}
	FORCEINLINE EWeaponState GetWeaponState() const {return WeaponState;}

protected:
	virtual void BeginPlay() override;
	
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



private:
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* AreaSphere;

	UPROPERTY(VisibleAnyWhere)
	class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category="Weapon Property")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACartridge> CartridgeClass;

	UPROPERTY(EditAnywhere)
	int32 Ammo;

	UPROPERTY(VisibleAnywhere, Category="Weapon Property")
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category="Weapon Property")
	EWeaponType WeaponType;
	
	//etc variable
	float Damage;

	
	

};
