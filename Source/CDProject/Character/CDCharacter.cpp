// Fill out your copyright notice in the Description page of Project Settings.


#include "CDCharacter.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"
#include "CDCharacterAttributeSet.h"
#include "CDProject/Anim/CDAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values
ACDCharacter::ACDCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	_camera->SetupAttachment(RootComponent);
	_camera->SetRelativeLocation(FVector(0, 0, _eyeHeight));
	
	_armMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arm Mesh"));
	_armMesh->SetupAttachment(_camera);
	_armMesh->bCastDynamicShadow = false;
	_armMesh->CastShadow = false;
	_armMesh->SetOnlyOwnerSee(true);
	GetMesh()->SetOwnerNoSee(true);
	
	_abilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	_abilitySystemComponent->SetIsReplicated(true);
	_abilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	_attributeSet = CreateDefaultSubobject<UCDCharacterAttributeSet>(TEXT("AttributeSet"));
}

// Called when the game starts or when spawned
void ACDCharacter::BeginPlay()
{
	Super::BeginPlay();

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
	{
		_abilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAttributes();
	}
}

// Called every frame
void ACDCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (Controller != nullptr)
	{
		FRotator ControlRot = Controller->GetControlRotation();
		FRotator ActorRot = GetActorRotation();
		float AimYaw = FMath::UnwindDegrees(ControlRot.Yaw - ActorRot.Yaw);
		
		if (AimYaw <= -75.f || AimYaw >= 75.f)
		{
			FRotator TargetRotation = FRotator(0.f, ControlRot.Yaw, 0.f);
			FRotator SmoothRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 2.5f); // 회전 속도 조절
			SetActorRotation(SmoothRotation);
		}
	}
}

// Called to bind functionality to input
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
		enhancedInputComponent->BindAction(_aimAction, ETriggerEvent::Completed, this, &ACDCharacter::Aim);
		enhancedInputComponent->BindAction(_changeWeaponAction, ETriggerEvent::Completed, this, &ACDCharacter::ChangeWeapon);
	}
}

void ACDCharacter::Move(const FInputActionValue& value)
{
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

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(-LookAxisVector.Y);
	}
}
void ACDCharacter::Fire(const FInputActionValue& value)
{
	
}
void ACDCharacter::Aim(const FInputActionValue& value)
{
	UCDAnimInstance* anim = Cast<UCDAnimInstance>(GetMesh()->GetAnimInstance());
	anim->_isAiming = !anim->_isAiming;
}
void ACDCharacter::ChangeWeapon(const FInputActionValue& value)
{
	UCDAnimInstance* anim = Cast<UCDAnimInstance>(GetMesh()->GetAnimInstance());
	anim->_isPistol = !anim->_isPistol;
}

UAbilitySystemComponent* ACDCharacter::GetAbilitySystemComponent() const
{
	return _abilitySystemComponent;
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

