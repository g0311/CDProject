// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
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
	uint8 _weaponType;
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
	FTransform _leftHandTransform;

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
	TObjectPtr<UAnimMontage> _equipRifleMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> _equipPistolMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage")
	TObjectPtr<UAnimMontage> _deadMontage;
	void PlayFireMontage(float fireRate);
	void PlayReloadMontage();
	void PlayEquipMontage(class AWeapon* nextWeapon);
	void PlayDeadMontage();
	
	void UpdateFullBodyProperty();
	void UpdateUpperBodyProperty();

	float GetReloadTime();
	float GetEquipTime(AWeapon* nextWeapon);
};
