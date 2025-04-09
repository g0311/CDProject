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
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
	
	void RespawnPlayer();
	void UpdateVisibilityForSpectator(bool isWatching);
private:
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

	UPROPERTY(EditAnywhere, Category = "Camera")
	float _defaultFOV = 90.0f;
	
public:
	FORCEINLINE USkeletalMeshComponent* GetArmMesh() { return _armMesh; }
	FORCEINLINE UCombatComponent* GetCombatComponent() { return _combat; }
	FORCEINLINE bool IsFirstPersonMesh(USkeletalMeshComponent* mesh) { return mesh == _armMesh; };
	FORCEINLINE UCameraComponent* GetCamera() { return _camera; }
	
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
	
	void RequestFire();
	void RequestAim();
	void RequestUnAim();
	void RequestReload();
	void RequestChangeWeapon(int weaponIndex);
	void RequestDropWeapon();
public:
	void GetWeapon(class AWeapon* weapon);
	
private:
	//Network Property
	UPROPERTY(VisibleAnywhere, Replicated, Category = "Network")
	FRotator _controlRotation;
	UPROPERTY(VisibleAnywhere, Replicated, Category = "Network")
	FRotator _cameraRotation;
	UFUNCTION(Server, Reliable)
	void ServerSetControlCameraRotation(FRotator control, FRotator camera);
	
public:
	FORCEINLINE FRotator GetControlRotation() { return _controlRotation; }

	
private:
	//GAS
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UAbilitySystemComponent> _abilitySystemComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UCDCharacterAttributeSet> _attributeSet;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(AllowPrivateAccess), Category = "Abilities")
	TSubclassOf<class UGameplayEffect> _defaultAttributeEffect;
public:
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	class UCDCharacterAttributeSet* GetAttributeSet();
	void InitializeAttributes();
};
