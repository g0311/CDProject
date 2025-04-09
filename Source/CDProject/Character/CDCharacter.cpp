// Fill out your copyright notice in the Description page of Project Settings.


#include "CDCharacter.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"
#include "CDCharacterAttributeSet.h"
#include "CDProject/Component//FootIKComponent.h"
#include "CDProject/Component/CombatComponent.h"
#include "CDProject/Controller/CDPlayerController.h"
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
	_armMesh->bOnlyOwnerSee = true;
	_armMesh->bCastDynamicShadow = false;
	_armMesh->CastShadow = false;
	_armMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	GetMesh()->SetOwnerNoSee(true);
	
	_combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	_combat->SetIsReplicated(true);
	
	_footIK = CreateDefaultSubobject<UFootIKComponent>(TEXT("FootIK"));
	
	_abilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	_abilitySystemComponent->SetIsReplicated(true);
	_abilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	_attributeSet = CreateDefaultSubobject<UCDCharacterAttributeSet>(TEXT("AttributeSet"));

	ACDPlayerController* CDPlayerController=Cast<ACDPlayerController>(GetController());
	if (CDPlayerController) CDPlayerController->SetHUDHealth(90,100);
	//접근 안됨
}
// Called when the game starts or when spawned
void ACDCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* playerController = Cast<APlayerController>(Controller);
	if (playerController)
	{
		EnableInput(playerController); 

		if (IsLocallyControlled()) 
		{
			UEnhancedInputLocalPlayerSubsystem* subSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()); 
			if (subSystem)
			{
				subSystem->AddMappingContext(_inputMappingContext, 0);
			}
		}
	}
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
		SetControlCameraRotation(_controlRotation, _cameraRotation);
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
	if (_combat->IsAimng())
	{
		float InterpSpeed = 10.0f;
		
		FTransform _currentArmTransform =
			UKismetMathLibrary::TInterpTo(_armMesh->GetRelativeTransform(), _aimArmTransform, DeltaTime, InterpSpeed);
		_armMesh->SetRelativeTransform(_currentArmTransform);
		
		float NewFOV = FMath::FInterpTo(_camera->FieldOfView, _combat->GetCurWeapon()->GetZoomedFOV(), DeltaTime, InterpSpeed);
		_camera->SetFieldOfView(NewFOV);
	}
	else
	{
		float InterpSpeed = 10.0f;
		
		FTransform _currentArmTransform =
			UKismetMathLibrary::TInterpTo(_armMesh->GetRelativeTransform(), _defaultArmTransform, DeltaTime, InterpSpeed);
		_armMesh->SetRelativeTransform(_currentArmTransform);
		
		float NewFOV = FMath::FInterpTo(_camera->FieldOfView, _defaultFOV, DeltaTime, InterpSpeed);
		_camera->SetFieldOfView(NewFOV);
	}
}

void ACDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (enhancedInputComponent)
	{
		//Auto Replicate
		enhancedInputComponent->BindAction(_moveAction, ETriggerEvent::Triggered, this, &ACDCharacter::Move);
		enhancedInputComponent->BindAction(_lookAction, ETriggerEvent::Triggered, this, &ACDCharacter::Look);
		enhancedInputComponent->BindAction(_jumpAction, ETriggerEvent::Triggered, this, &ACDCharacter::Jump);
		enhancedInputComponent->BindAction(_jumpAction, ETriggerEvent::Completed, this, &ACDCharacter::StopJumping);
		enhancedInputComponent->BindAction(_crouchAction, ETriggerEvent::Triggered, this, &ACDCharacter::Crouch, false);
		enhancedInputComponent->BindAction(_crouchAction, ETriggerEvent::Completed, this, &ACDCharacter::UnCrouch, false);
		//
		enhancedInputComponent->BindAction(_walkAction, ETriggerEvent::Triggered, this, &ACDCharacter::Walk);
		enhancedInputComponent->BindAction(_walkAction, ETriggerEvent::Completed, this, &ACDCharacter::UnWalk);

		//Need Server Request
		enhancedInputComponent->BindAction(_fireAction, ETriggerEvent::Triggered, this, &ACDCharacter::RequestFire);
		enhancedInputComponent->BindAction(_aimAction, ETriggerEvent::Completed, this, &ACDCharacter::RequestAim);
		enhancedInputComponent->BindAction(_reloadAction, ETriggerEvent::Completed, this, &ACDCharacter::RequestReload);
		enhancedInputComponent->BindAction(_changeWeaponActions[0], ETriggerEvent::Started, this, &ACDCharacter::RequestChangeWeapon, 0);
		enhancedInputComponent->BindAction(_changeWeaponActions[1], ETriggerEvent::Started, this, &ACDCharacter::RequestChangeWeapon, 1);
		enhancedInputComponent->BindAction(_changeWeaponActions[2], ETriggerEvent::Started, this, &ACDCharacter::RequestChangeWeapon, 2);
		enhancedInputComponent->BindAction(_changeWeaponActions[3], ETriggerEvent::Started, this, &ACDCharacter::RequestChangeWeapon, 3);
		enhancedInputComponent->BindAction(_changeWeaponActions[4], ETriggerEvent::Started, this, &ACDCharacter::RequestChangeWeapon, 4);
		enhancedInputComponent->BindAction(_dropWeaponAction, ETriggerEvent::Completed, this, &ACDCharacter::RequestDropWeapon);
	}
}

float ACDCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float curHealth = _attributeSet->GetHealth();
	curHealth -= DamageAmount;
	_attributeSet->SetHealth(curHealth);
	if (curHealth <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("player Dead"));
	}
	
	return DamageAmount;
}

void ACDCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACDCharacter, _controlRotation);
	DOREPLIFETIME(ACDCharacter, _cameraRotation);
}

inline void ACDCharacter::PossessedBy(AController* NewController)
{ //Server Part
	Super::PossessedBy(NewController);
	if (_abilitySystemComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Possessed Called"));
		_abilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAttributes();
	}
}

void ACDCharacter::RespawnPlayer()
{
}

void ACDCharacter::UpdateVisibilityForSpectator(bool isWatching)
{
	if (isWatching)
	{
		_armMesh->SetOnlyOwnerSee(false);
		_armMesh->SetVisibility(true);
		
		GetMesh()->SetVisibility(false);
	}
	else
	{
		_armMesh->SetOnlyOwnerSee(true);
		_armMesh->SetVisibility(false);
		
		GetMesh()->SetVisibility(true);
	}
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

void ACDCharacter::RequestFire()
{
	if (!IsLocallyControlled() || !_combat) return;
	
	if (_combat->IsFireAvail())
	{
		if (_combat->IsAmmoEmpty())
		{
			RequestReload();
		}
		else
		{
			_combat->Fire();
		}
	}
}

void ACDCharacter::RequestAim()
{
	if (!_combat)
		return;
	
	// _isAim set -> server _isAim set => On_Rep
	if (IsLocallyControlled())
	{
		if (_combat->IsAimng())
		{
			_combat->Aim();
		}
		else
		{
			_combat->UnAim();
		}
	}
	ServerAim();
}

void ACDCharacter::RequestUnAim()
{
	if (!_combat)
		return;
	
	// _isAim set -> server _isAim set => On_Rep
	if (IsLocallyControlled())
	{
		_combat->UnAim();
	}
	ServerUnAim();
}

void ACDCharacter::RequestReload()
{
	if (!_combat)
		return;
	
	_combat->Reload();
}

void ACDCharacter::RequestChangeWeapon(int weaponIndex)
{
	if (!_combat)
		return;
	//if (_combat->ChangeWeapon(weaponIndex))
	//{
	//	RequestUnAim();
	//}
	_combat->ChangeWeapon(weaponIndex);
}

void ACDCharacter::RequestDropWeapon()
{
	if (!_combat)
		return;

	//RequestUnAim();

	_combat->DropWeapon();
}

void ACDCharacter::GetWeapon(AWeapon* weapon)
{
	if (!_combat)
		return;
	_combat->GetWeapon(weapon);
}

void ACDCharacter::SetControlCameraRotation_Implementation(FRotator control, FRotator camera)
{
	_controlRotation = control;
	_cameraRotation = camera;
}

void ACDCharacter::ServerAim_Implementation()
{
	if (!_combat)
		return;
	if(_combat->IsAimng())
	{
		ServerUnAim();
	}
	else if (_combat->IsAimAvail())
	{
		_combat->Aim();
	}
}

void ACDCharacter::ServerUnAim_Implementation()
{
	if (!_combat)
    	return;
	_combat->UnAim();
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

	FGameplayEffectSpecHandle NewHandle = _abilitySystemComponent->MakeOutgoingSpec(_defaultAttributeEffect, 0, EffectContext);
	if(NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveHandle = 
			_abilitySystemComponent->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
	}
}
