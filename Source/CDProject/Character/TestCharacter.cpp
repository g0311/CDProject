
#include "TestCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Animation/AnimInstance.h"

ATestCharacter::ATestCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    USpringArmComponent* SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 300.0f;
    SpringArm->bUsePawnControlRotation = true;

    UCameraComponent* Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm);
    Camera->bUsePawnControlRotation = false;
}

void ATestCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(PlayerMappingContext, 0);
        }
    }
}

void ATestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInput->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ATestCharacter::Move);
        EnhancedInput->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ATestCharacter::Look);
        EnhancedInput->BindAction(IA_Equip, ETriggerEvent::Triggered, this, &ATestCharacter::Equip);
        EnhancedInput->BindAction(IA_Fire, ETriggerEvent::Triggered, this, &ATestCharacter::Fire);
    }
}

void ATestCharacter::Move(const FInputActionValue& Value)
{
    FVector2D MovementVector = Value.Get<FVector2D>();
    if (Controller)
    {
        AddMovementInput(GetActorForwardVector(), MovementVector.Y);
        AddMovementInput(GetActorRightVector(), MovementVector.X);
    }
}

void ATestCharacter::Look(const FInputActionValue& Value)
{
    FVector2D LookAxisVector = Value.Get<FVector2D>();
    AddControllerYawInput(LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
}

void ATestCharacter::Equip(const FInputActionValue& Value)
{
    if (EquipMontage)
    {
        PlayAnimMontage(EquipMontage);
        UE_LOG(LogTemp, Log, TEXT("Equip montage played."));
    }
}

void ATestCharacter::Fire(const FInputActionValue& Value)
{
    if (FireMontage)
    {
        PlayAnimMontage(FireMontage);
        UE_LOG(LogTemp, Log, TEXT("Fire montage played."));
    }
}
