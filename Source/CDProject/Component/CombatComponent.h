// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CDProject/HUD/CDHUD.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CDPROJECT_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void Reset();
	
	FORCEINLINE	bool IsAimng() { return _isAiming; }
	FORCEINLINE	bool IsAimAvail() { return _isCanAim; }
	FORCEINLINE void SetAimAvail() { _isCanAim = true; }
	FORCEINLINE bool IsFireAvail() { return _isCanFire; }
	FORCEINLINE void SetFireAvail() { _isCanFire = true; }
	FORCEINLINE float GetFireDelay() { return _fireDelay; }
	FORCEINLINE TArray<AWeapon*> GetWeapons() { return _weapons; }

	AWeapon* GetCurWeapon();
	bool IsAmmoEmpty();
	bool IsTotalAmmoEmpty();
	uint8 GetCurWeaponType();
	void SetWeaponVisible(bool tf);
	void SetBefWeaponVisible(bool tf);
	
	FHUDPackage HUDPackage;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> _defaultSubWeapon;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> _defaultMeleeWeapon;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCameraShakeBase> _fireCameraShakeClass;
private:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	class ACDCharacter* _playerCharacter;
	UPROPERTY(VisibleAnywhere)
	class ACDHUD* HUD;
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_WeaponID)
	int _weaponIndex = -1;
	UPROPERTY(VisibleAnywhere)
	int _befIndex = -1;
	UPROPERTY(VisibleAnywhere, Replicated)
	TArray<class AWeapon*> _weapons;
	UPROPERTY(VisibleAnywhere, Replicated)
	bool _isAiming;
	
	FTimerHandle _fireTimerHandle;
	float _fireDelay = 0.23f;
	FTimerHandle _fireAimAbleTimerHandle;
	
	UPROPERTY(VisibleAnywhere, Replicated)
	bool _isCanFire = true;
	UPROPERTY(VisibleAnywhere, Replicated)
	bool _isCanAim = true;
	//보안용 레플리케이트
	
	void CreateDefaultWeapons();
	float CalculateSpread();
	FVector CreateTraceDir();
public:
	UPROPERTY(VisibleAnywhere, Replicated, Category = "Network")
	float _curSpread = 0.f;

	void RequestFire();
	//ServerCall
	UFUNCTION(Server, Reliable)
	void ServerFire(FVector fireDir);
	UFUNCTION(Server, Reliable)
	void ServerReload();
	UFUNCTION(Server, Reliable)
	void ServerChangeWeapon(int idx);
	UFUNCTION(Server, Reliable)
	void ServerDropWeapon();
	UFUNCTION(Server, Reliable)
	void ServerAim(bool tf);
	void GetWeapon(class AWeapon* weapon, bool isForceGet = false);
		//Both Call
		void Aim(bool tf);
private:
	//Implementation
	void Fire(FVector fireDir);
	void Reload();
	void ChangeWeapon(int idx);
	void DropWeapon();
	void SetHUDCrosshairs(float spread);

	
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastFire(FVector target);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastReload();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastDropWeapon(AWeapon* weapon);
	UFUNCTION()
	void OnRep_WeaponID();

	//deprecated
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastChangeWeapon(int idx);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastSetIsCanFire(bool tf);
	UFUNCTION(Server, Reliable)
	void ServerSetFireAvail();
	UFUNCTION(Server, Reliable)
	void ServerSetAimAvail();
};