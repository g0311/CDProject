// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CDProject/Weapon/Weapon.h"
#include "CDAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class CDPROJECT_API UCDAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

//bool
public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeInitializeAnimation() override;
	
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	class ACDCharacter* _playerCharacter;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool _isJumping;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	bool _isCrouching;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	bool _isAiming;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	EWeaponType _weaponType;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	float _movementSpeed;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	float _direction;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	float _aimYaw;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	float _aimPitch;
	
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	FRotator _lFootRotator;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	FRotator _rFootRotator;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	float _hipOffset;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	float _lFootOffset;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	float _rFootOffset;
	
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	FVector _leftHandLocation;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	float _leftHandIKAlpha;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	bool _isFullBody = true;

	//Montage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> _rifleReloadMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> _shotgunReloadMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> _pistolReloadMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> _baseFireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> _aimFireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> _pistolFireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> _knifeFireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> _grenadeReadyMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> _grenadeThrowMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> _equipRifleMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> _equipPistolMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> _equipGrenadeMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> _deadMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	TArray<UAnimMontage*> _hitMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	TObjectPtr<USoundCue> _knifeFireSound;
	
	void PlayFireMontage(float fireRate);
	void PlayGrenadeReadyMontage();
	void PlayReloadMontage();
	void PlayEquipMontage(class AWeapon* nextWeapon);
	void PlayDeadMontage();
	void PlayHitMontage();
	
	void UpdateFullBodyProperty(float DeltaSeconds);
	void UpdateUpperBodyProperty(float DeltaSeconds);

	float GetReloadTime();
	float GetGrenadeReadyTime();
	float GetGrenadeThrowTime();
	float GetEquipTime(AWeapon* nextWeapon);
};