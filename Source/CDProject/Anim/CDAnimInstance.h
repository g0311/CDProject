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
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	class ACharacter* _playerPawn;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool _isJumping;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool _isCrouching;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	bool _isAiming;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Animation")
	int _weaponType;
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
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeInitializeAnimation() override;
};
