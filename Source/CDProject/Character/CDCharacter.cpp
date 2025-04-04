// Fill out your copyright notice in the Description page of Project Settings.


#include "CDCharacter.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"
#include "CDCharacterAttributeSet.h"
#include "CDProject/Component//FootIKComponent.h"
#include "CDProject/Component/CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CDProject/Weapon/Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACDCharacter::ACDCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	
	_camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	_camera->SetupAttachment(RootComponent);
	_camera->SetRelativeLocation(FVector(0, 0, _eyeHeight));
	
	_armMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arm Mesh"));
	_armMesh->SetupAttachment(_camera);
	_armMesh->bCastDynamicShadow = false;
	_armMesh->CastShadow = false;
	_armMesh->SetOnlyOwnerSee(true);
	GetMesh()->SetOwnerNoSee(true);

	_combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	_combat->SetIsReplicated(true);
	
	_footIK = CreateDefaultSubobject<UFootIKComponent>(TEXT("FootIK"));
	
	_abilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	_abilitySystemComponent->SetIsReplicated(true);
	_abilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	_attributeSet = CreateDefaultSubobject<UCDCharacterAttributeSet>(TEXT("AttributeSet"));
}

// Called when the game starts or when spawned
void ACDCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocallyControlled())
	{
		APlayerController* playerController = Cast<APlayerController>(Controller);
		if (playerController)
		{
			UEnhancedInputLocalPlayerSubsystem* subSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()); 
			if (subSystem)
			{
				subSystem->AddMappingContext(_inputMappingContext, 0);
			}
		}
	
		if (_abilitySystemComponent)
		{//이거 서버에서 해야되나?
			_abilitySystemComponent->InitAbilityActorInfo(this, this);
			InitializeAttributes();
		}
	}
	
	_currentArmTransform = _defaultArmTransform;
	_targetArmTransform = _currentArmTransform;
	_targetFOV = _defaultFOV;
}

// Called every frame
void ACDCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{
		if (Controller != nullptr)
			_controlRotation = Controller->GetControlRotation();
		_cameraRotation = _camera->GetRelativeRotation();
	}
	if (!IsLocallyControlled())
	{
		_camera->SetRelativeRotation(_cameraRotation);
	}
	
	//if >= 90 degree character rotate
	FRotator ControlRot = _controlRotation;
	FRotator ActorRot = GetActorRotation();
	float AimYaw = FMath::UnwindDegrees(ControlRot.Yaw - ActorRot.Yaw);
		
	if (AimYaw <= -45.f || AimYaw >= 45.f)
	{
		FRotator TargetRotation = FRotator(0.f, ControlRot.Yaw, 0.f);
		FRotator SmoothRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 2.5f); // 회전 속도 조절
		SetActorRotation(SmoothRotation);
	}
	
	//Update Arm Mesh Location
	float InterpSpeed = 10.0f;
	_currentArmTransform = UKismetMathLibrary::TInterpTo(_currentArmTransform, _targetArmTransform, DeltaTime, InterpSpeed);
	_armMesh->SetRelativeTransform(_currentArmTransform);

	float NewFOV = FMath::FInterpTo(_camera->FieldOfView, _targetFOV, DeltaTime, InterpSpeed);
	_camera->SetFieldOfView(NewFOV);

	//Update Spread
	if (_combat)
	{
		float newSpread = CalculateSpread();
		//UE_LOG(LogTemp, Log, TEXT("spread: %f"), newSpread);
		_curSpread = FMath::FInterpTo(_curSpread, newSpread, DeltaTime, 50.f);
		//UE_LOG(LogTemp, Display, TEXT("%f"), _curSpread);
		_combat->SetHUDCrosshairs(_curSpread);
	}
}

void ACDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (enhancedInputComponent)
	{
		enhancedInputComponent->BindAction(_moveAction, ETriggerEvent::Triggered, this, &ACDCharacter::Move);
		enhancedInputComponent->BindAction(_lookAction, ETriggerEvent::Triggered, this, &ACDCharacter::Look);
		enhancedInputComponent->BindAction(_jumpAction, ETriggerEvent::Triggered, this, &ACDCharacter::Jump);
		enhancedInputComponent->BindAction(_jumpAction, ETriggerEvent::Completed, this, &ACDCharacter::StopJumping);
		enhancedInputComponent->BindAction(_crouchAction, ETriggerEvent::Triggered, this, &ACDCharacter::Crouch, false);
		enhancedInputComponent->BindAction(_crouchAction, ETriggerEvent::Completed, this, &ACDCharacter::UnCrouch, false);
		enhancedInputComponent->BindAction(_walkAction, ETriggerEvent::Triggered, this, &ACDCharacter::Walk);
		enhancedInputComponent->BindAction(_walkAction, ETriggerEvent::Completed, this, &ACDCharacter::UnWalk);
		enhancedInputComponent->BindAction(_fireAction, ETriggerEvent::Triggered, this, &ACDCharacter::Fire);
		enhancedInputComponent->BindAction(_aimAction, ETriggerEvent::Completed, this, &ACDCharacter::Aim);
		enhancedInputComponent->BindAction(_reloadAction, ETriggerEvent::Completed, this, &ACDCharacter::Reload);
		enhancedInputComponent->BindAction(_changeWeaponActions[0], ETriggerEvent::Started, this, &ACDCharacter::ChangeWeapon, 0);
		enhancedInputComponent->BindAction(_changeWeaponActions[1], ETriggerEvent::Started, this, &ACDCharacter::ChangeWeapon, 1);
		enhancedInputComponent->BindAction(_changeWeaponActions[2], ETriggerEvent::Started, this, &ACDCharacter::ChangeWeapon, 2);
		enhancedInputComponent->BindAction(_changeWeaponActions[3], ETriggerEvent::Started, this, &ACDCharacter::ChangeWeapon, 3);
		enhancedInputComponent->BindAction(_changeWeaponActions[4], ETriggerEvent::Started, this, &ACDCharacter::ChangeWeapon, 4);
		enhancedInputComponent->BindAction(_dropWeaponAction, ETriggerEvent::Completed, this, &ACDCharacter::DropWeapon);
	}
}

float ACDCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float curHealth = _attributeSet->GetHealth();
	curHealth -= DamageAmount;
	_attributeSet->SetHealth(curHealth);
	
	return DamageAmount;
}

void ACDCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACDCharacter, _controlRotation);
	DOREPLIFETIME(ACDCharacter, _cameraRotation);
}

void ACDCharacter::RespawnPlayer()
{
}

void ACDCharacter::Move(const FInputActionValue& value)
{
	if (!Controller)
		return;
	
	FVector inputVal = value.Get<FVector>();
	
	const FRotator rotation = Controller->GetControlRotation();
	const FRotator yawRotation(0, rotation.Yaw, 0);

	const FVector forwardDir = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
	const FVector rightDir = FRotationMatrix(rotation).GetUnitAxis(EAxis::Y);
	
	AddMovementInput(forwardDir, inputVal.Y);
	AddMovementInput(rightDir, inputVal.X);
}

void ACDCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookAxisVector = value.Get<FVector2D>();
	
	if (Controller)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(-LookAxisVector.Y);
	}
}

void ACDCharacter::Crouch(bool bClientSimulation)
{
	if (GetCharacterMovement()->IsFalling())
		return;
	
	Super::Crouch(bClientSimulation);
}

void ACDCharacter::Walk()
{
	//need to change mechanism
	//server RPC or MovementComponent Refactoring
	GetCharacterMovement()->MaxWalkSpeed = 250.f;
}

void ACDCharacter::UnWalk()
{
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
}

void ACDCharacter::Fire()
{
	if (!_combat)
		return;
	
	if (_combat->IsFireAvail())
	{
		if (false/*_combat->IsAmmoEmpty()*/)
		{
			Reload();
		}
		else
		{
			_combat->Fire(_curSpread);
		}
	}
}

void ACDCharacter::Aim()
{
	if (!_combat)
		return;
	
	if(_combat->IsAimng())
	{
		_combat->UnAim();
		_targetArmTransform = _defaultArmTransform;
		_targetFOV = _defaultFOV;
	}
	else if (_combat->IsAimAvail())
	{
		_combat->Aim();
		_targetArmTransform = _aimArmTransform;
		_targetFOV = _combat->GetCurWeapon()->GetZoomedFOV();
	}
}

void ACDCharacter::UnAim()
{
	if (!_combat)
		return;
	
	_combat->UnAim();
	_targetArmTransform = _defaultArmTransform;
	_targetFOV = _defaultFOV;
}

void ACDCharacter::Reload()
{
	if (!_combat)
		return;
	
	UnAim();
	
	//Check Reload Avail
	if (_combat->IsTotalAmmoEmpty())
		return;
	
	_combat->Reload();
}

void ACDCharacter::ChangeWeapon(int weaponIndex)
{
	if (!_combat)
		return;
	if (_combat->ChangeWeapon(weaponIndex))
	{
		UnAim();
	}
}

void ACDCharacter::GetWeapon(AWeapon* weapon)
{
	if (!_combat)
		return;
	_combat->GetWeapon(weapon);
}

void ACDCharacter::DropWeapon()
{
	if (!_combat)
		return;
	UnAim();
	_combat->DropWeapon();
}

float ACDCharacter::CalculateSpread()
{
	float spread = 1.0f;
	
	float Speed = GetVelocity().Size();
	spread += (Speed / 470.f) * 1.8f; //MaxSpeed
	
	if (GetMovementComponent()->IsFalling())
	{
		spread += 3.f;
	}
	if (bIsCrouched)
	{
		spread -= 0.3f;  // 앉으면 감소
	}
	if (_combat->IsAimng())
	{
		spread -= 0.3f;
	}
	
	if (_combat) 
	{
		float continuouedFireFactor = FMath::Clamp(_combat->GetContinuedFireCount() * 3 / 5.0f /* x Weapon Spread */, 0.f, 3.f); 
		spread += continuouedFireFactor;
	}
	
	return FMath::Clamp(spread, 0.4f, 5.f);
}

UAbilitySystemComponent* ACDCharacter::GetAbilitySystemComponent() const
{
	return _abilitySystemComponent;
}

class UCDCharacterAttributeSet* ACDCharacter::GetAttributeSet()
{
	return _attributeSet;
}

void ACDCharacter::InitializeAttributes()
{
	FGameplayEffectContextHandle EffectContext = _abilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = _abilitySystemComponent->MakeOutgoingSpec(_defaultAttributes, 0, EffectContext);
	if(NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveHandle = 
			_abilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), _abilitySystemComponent.Get());
	}
}
