// Fill out your copyright notice in the Description page of Project Settings.


#include "CDCharacter.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"
#include "CDCharacterAttributeSet.h"

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
	}
}

// Called every frame
void ACDCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

UAbilitySystemComponent* ACDCharacter::GetAbilitySystemComponent() const
{
	return _abilitySystemComponent;
}

