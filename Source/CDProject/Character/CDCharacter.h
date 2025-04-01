// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "CDCharacter.generated.h"

UCLASS()
class CDPROJECT_API ACDCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACDCharacter();
	void RespawnPlayer();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	//Component
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UCameraComponent> _camera;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> _armMesh;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UCombatComponent> _combat;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UFootIKComponent> _footIK;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	float _eyeHeight = 50.f;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	FTransform _defaultArmTransform;
	UPROPERTY(EditAnywhere, Category = "Components")
	FTransform  _aimArmTransform;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	FTransform  _currentArmTransform;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	FTransform  _targetArmTransform;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float _defaultFOV = 90.0f;
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	float _targetFOV;

private:
	//Input
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputMappingContext> _inputMappingContext;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputAction> _moveAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputAction> _lookAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputAction> _jumpAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputAction> _crouchAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputAction> _walkAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputAction> _fireAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputAction> _aimAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputAction> _reloadAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<TObjectPtr<class UInputAction>> _changeWeaponActions;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputAction> _dropWeaponAction;
	
	void Move(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	void Crouch(bool bClientSimulation = false) override;
	void Walk();
	void UnWalk();
	void Fire();
	void Aim();
	void UnAim();
	void Reload();
	void ChangeWeapon(int weaponIndex);
	void GetWeapon(class AWeapon* weapon);
	void DropWeapon();
public:
	//GAS
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> _defaultAttributes;
private:
	TObjectPtr<class UAbilitySystemComponent> _abilitySystemComponent;
	TObjectPtr<class UCDCharacterAttributeSet> _attributeSet;
	
	void InitializeAttributes();
};