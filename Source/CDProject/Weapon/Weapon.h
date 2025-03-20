// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"

#include "Weapon.generated.h"



UENUM(BlueprintType)
enum class EWeaponState:uint8
{
	EWS_Initial UMETA(DisplayName = "Initial"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
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
	virtual void Dropped();
	void ShowPickUpWidget(bool bShowWidget);
	
	void SetWeaponState(EWeaponState state);
	
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

	UPROPERTY(VisibleAnywhere)
	EWeaponState WeaponState;
	


private:
	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* AreaSphere;

	UPROPERTY(VisibleAnyWhere)
	class UWidgetComponent* PickupWidget;
	
	
	//PickUp Widget


	//etc variable
	float Damage;
	int32 Ammo;

	
	

};
