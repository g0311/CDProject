// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CDProject/HUD/CDHUD.h"
#include "CDProject/Weapon/Weapon.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "CDProject/Character/CDGameplayTag.h"
#include "CombatComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CDPROJECT_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void Reset(bool isDead);
	void DeadAction();

	void InsertCombatState(FGameplayTag StateTag);
	void RemoveCombatState(FGameplayTag StateTag);
	bool IsInCombatState(FGameplayTag StateTag) const;
	FORCEINLINE	bool IsAiming() { return IsInCombatState(CombatTags::State_Combat_Aiming); }//오타 수정
	FORCEINLINE bool IsChanging() { return IsInCombatState(CombatTags::State_Combat_ChangingWeapon); }
	FORCEINLINE bool IsReloading() { return IsInCombatState(CombatTags::State_Combat_Reloading); }
	FORCEINLINE float GetFireDelay() { return _fireDelay; }

	//Blueprint
	UFUNCTION(BlueprintCallable, Category="Blueprint")
	int ExcuteGetCurAmmo(){return GetCurAmmo();};
	UFUNCTION(BlueprintCallable, Category="Blueprint")
	int ExcuteGetCarriedAmmo(){return GetCarriedAmmo();};
	UFUNCTION(BlueprintCallable, Category="Blueprint")
	int ExcuteGetAmmoCapacity(){return GetAmmoCapacity();};
	UFUNCTION(BlueprintCallable, Category="Blueprint")
	void ExcuteSetAmmoCapacity(int NewAmmoCount){SetAmmoCapacity(NewAmmoCount);}
	UFUNCTION(BlueprintCallable, Category="Blueprint")
	void ExcuteFire(FVector Target){Fire(Target);}
	UFUNCTION(BlueprintCallable, Category="Blueprint")
	void ExcuteReload(){Reload();}
	UFUNCTION(BlueprintCallable, Category="Blueprint")
	void ExcuteMeleeAttack();
	
	
	FORCEINLINE TArray<AWeapon*> GetWeapons() { return _weapons; }
	FORCEINLINE void SetC4Area(bool tf) { _isC4Area = tf; }
	int GetCurAmmo();
	int GetCarriedAmmo();
	int GetAmmoCapacity();
	int SetAmmoCapacity(int NewAmmoCount);
	
	AWeapon* GetCurWeapon();
	bool IsAmmoEmpty();
	bool IsTotalAmmoEmpty();
	EWeaponType GetCurWeaponType();
	void SetWeaponVisible(bool tf);
	void SetBefWeaponVisible(bool tf);
	class ARoundGameMode* GetRoundGameMode();
	
	FHUDPackage HUDPackage;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> _defaultSubWeapon;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> _defaultMeleeWeapon;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> _c4Weapon;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCameraShakeBase> _fireCameraShakeClass;
private:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	class ACDCharacter* _playerCharacter;
	UPROPERTY(VisibleAnywhere)
	class ACDHUD* HUD;
	UPROPERTY(VisibleAnywhere, Replicated)
	FGameplayTagContainer _combatStateTags;
	//State
	
	UPROPERTY(VisibleAnywhere, Replicated)
	int _weaponIndex = -1;
	UPROPERTY(VisibleAnywhere)
	int _befIndex = -1;
	UPROPERTY(VisibleAnywhere, Replicated)
	TArray<TObjectPtr<AWeapon>> _weapons;
	UPROPERTY(VisibleAnywhere, Replicated)
	bool _isC4Area = false;	


	FTimerHandle _clientFireTimerHandle;
	FTimerHandle _fireTimerHandle;
	float _fireDelay = 0.23f;
	FTimerHandle _fireAimAbleTimerHandle;
	FTimerHandle _weaponVisibleTimerHandle;
	FTimerHandle _c4TimerHandle;	
	
	UPROPERTY(VisibleAnywhere)
	AActor* _aimedActor;
	
	void CreateDefaultWeapons();
	float CalculateSpread();
	FVector CreateTraceDir(float spread);
public:
	UPROPERTY(VisibleAnywhere, Replicated, Category = "Network")
	float _curSpread = 0.f;

	void RequestFire();
	void RequestFireStart();
	void RequestFireEnd();
	void RequestInteractStart();
	void RequestInteractEnd();
	void RequestChange(int idx);
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
	UFUNCTION(Server, Reliable)
	void ServerReadyGrenade();
	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();
	UFUNCTION(Server, Reliable)
	void ServerC4Plant(bool isPlanting);
	UFUNCTION(Server, Reliable)
	void ServerC4Defuse(bool isDefusing);
	UFUNCTION(Server, Reliable)
	void ServerShotgunReload();
	UFUNCTION(Server, Reliable)
	void ServerCancelReload();
		//Both Call
		void Aim(bool tf);
	void DropAllWeapons();
	void ChangeToNextWeapon();
	void CreateC4Weapon();

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
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastChangeWeapon(int idx);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastGrenadeReady();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastGrenadeThrow();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastC4Plant(bool tf, float duration = 0.f);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastC4Defuse(bool tf, float duration = 0.f);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastCancelReload();
};
