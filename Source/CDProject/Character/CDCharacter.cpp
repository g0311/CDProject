// Fill out your copyright notice in the Description page of Project Settings.


#include "CDCharacter.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystemComponent.h"
#include "CDCharacterAttributeSet.h"
#include "CDCharacterMovementComponent.h"
#include "CDProject/Anim/CDAnimInstance.h"
#include "CDProject/Component//FootIKComponent.h"
#include "CDProject/Component/CombatComponent.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CDProject/Weapon/Weapon.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

class UCDAnimInstance;
// Sets default values
ACDCharacter::ACDCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	
	_springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	_springArm->SetupAttachment(RootComponent);
	
	_camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	_camera->SetupAttachment(_springArm);
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
}

// Called when the game starts or when spawned
void ACDCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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
		ServerSetControlCameraRotation(_controlRotation, _cameraRotation);
	}
	if (!IsLocallyControlled())
	{
		//for Spector Update?
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
	if (_combat->IsAiming())
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
	if (_attributeSet && _attributeSet->GetHealth() == 0)
		return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	float finalDamage = DamageAmount;
	
	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* pointEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		USkeletalMeshComponent* MeshComp = GetMesh();
		FName Bone = pointEvent->HitInfo.BoneName;
		UE_LOG(LogTemp, Log, TEXT("Comp: %s, Bone: %s"), *pointEvent->HitInfo.Component->GetName(), *Bone.ToString());
		
		FName ParentBone = MeshComp->GetParentBone(Bone);
		while (ParentBone != NAME_None)
		{
			if (ParentBone.ToString().Contains("head") ||
				ParentBone.ToString().Contains("neck"))
			{
				finalDamage *= 2.f;
				break;
			}
			if (ParentBone.ToString().Contains("upperarm"))
			{
				finalDamage *= 0.5f;
				break;
			}
			if (ParentBone.ToString().Contains("thigh"))
			{
				finalDamage *= 0.75f;
				break;
			}
			
			ParentBone = MeshComp->GetParentBone(ParentBone);
		}
	}
	//Effect 기반으로 변경 후, PostGameplayEffectExecute()에서 On Dead 호출하면 댐
	HandleDamage(finalDamage);
	
	//for listen server
	ACDPlayerController* ACPC = Cast<ACDPlayerController>(Controller);
	if (ACPC)
	{
		ACPC->SetHUDHealth(_attributeSet->GetHealth());
		ACPC->SetHUDShield(_attributeSet->GetShield());
	}
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ACDCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACDCharacter, _controlRotation);
	DOREPLIFETIME(ACDCharacter, _cameraRotation);
}

void ACDCharacter::PossessedBy(AController* NewController)
{ //Server Part
	Super::PossessedBy(NewController);
	if (_abilitySystemComponent)
	{
		_abilitySystemComponent->InitAbilityActorInfo(this, this);
		InitializeAttributes();
	}
}

//Should Be MultiCast
void ACDCharacter::RespawnPlayer()
{
	if (_attributeSet->GetHealth() > 0)
	{
		if (HasAuthority())
		{
			_combat->Reset(false);
			_attributeSet->SetHealth(_attributeSet->GetMaxHealth());
			GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			//Move To Spawn Point	
			
		}
	}
	else
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		GetMesh()->GetAnimInstance()->Montage_Stop(0.f);
		if (HasAuthority())
		{
			_combat->Reset(true);
			_attributeSet->SetHealth(_attributeSet->GetMaxHealth());
			
			//Move to Spawn Point
			
		}
		else if (IsLocallyControlled())
		{
			_armMesh->SetVisibility(true);
			//Enable Input
			APlayerController* PC = Cast<APlayerController>(GetController());
			if (PC)
			{
				ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
				if (LocalPlayer)
				{
					UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
					if (Subsystem)
					{
						Subsystem->AddMappingContext(_inputMappingContext, 0);
					}
				}
			}
		}
	}
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
		// FOV에 비례한 감도 조절
		float CurrentFOV = _camera->FieldOfView;
		float DefaultFOV = 90.f; // 기본 FOV 값 (줌 아웃 상태)
		
		float FOVScale = CurrentFOV / DefaultFOV;
		float FinalSensitivity = FOVScale * _mouseSensitivity;

		AddControllerYawInput(LookAxisVector.X * FinalSensitivity);
		AddControllerPitchInput(-LookAxisVector.Y * FinalSensitivity);
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
	UCDCharacterMovementComponent* CDCMC = Cast<UCDCharacterMovementComponent>(GetCharacterMovement());
	if (CDCMC)
		CDCMC->bWantsToWalk = true;
}

void ACDCharacter::UnWalk()
{
	UCDCharacterMovementComponent* CDCMC = Cast<UCDCharacterMovementComponent>(GetCharacterMovement());
	if (CDCMC)
		CDCMC->bWantsToWalk = false;
}

void ACDCharacter::RequestFire()
{
	if (!_combat) return;
	
	if (_combat->IsFireAvail())
	{
		if (_combat->IsAmmoEmpty())
		{
			RequestReload();
		}
		else
		{
			_combat->RequestFire();
		}
	}
}

void ACDCharacter::RequestAim()
{
	if (!_combat)
		return;
	
	// _isAim set -> server _isAim set => On_Rep
	// 반응 시간 최적화?
	bool nextAiming = !_combat->IsAiming();
	_combat->Aim(nextAiming);
	_combat->ServerAim(nextAiming);
}

void ACDCharacter::RequestReload()
{
	if (!_combat)
		return;
	_combat->Aim(false);
	_combat->ServerReload();
}

void ACDCharacter::RequestChangeWeapon(int weaponIndex)
{
	if (!_combat)
		return;
	_combat->Aim(false);
	_combat->RequestChange(weaponIndex);
}

void ACDCharacter::RequestDropWeapon()
{
	if (!_combat)
		return;
	_combat->Aim(false);
	_combat->ServerDropWeapon();
}

//Always Called By Server
void ACDCharacter::GetWeapon(AWeapon* weapon)
{
	if (!_combat)
		return;
	_combat->Aim(false);
	_combat->GetWeapon(weapon);
}

void ACDCharacter::ServerSetControlCameraRotation_Implementation(FRotator control, FRotator camera)
{
	_controlRotation = control;
	_cameraRotation = camera;
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

void ACDCharacter::Multicast_Dead_Implementation()
{
	UCDAnimInstance* bodyAnim = Cast<UCDAnimInstance>(GetMesh()->GetAnimInstance());
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(GetArmMesh()->GetAnimInstance());

	if (IsLocallyControlled())
	{
		//Disable Input
		APlayerController* PC = Cast<APlayerController>(GetController());
		if (PC)
		{
			ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
			if (LocalPlayer)
			{
				UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
				if (Subsystem)
				{
					Subsystem->RemoveMappingContext(_inputMappingContext);
				}
			}
		}
		//UnVisible Arm Mesh
		GetArmMesh()->SetVisibility(false);
	}
	if (HasAuthority())
	{
		//Drop All Weapon
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
		
	if (bodyAnim)
		bodyAnim->PlayDeadMontage();
	if (armAnim)
		armAnim->PlayDeadMontage();
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

void ACDCharacter::Multicast_Hit()
{
	UCDAnimInstance* bodyAnim = Cast<UCDAnimInstance>(GetMesh()->GetAnimInstance());
	UCDAnimInstance* armAnim = Cast<UCDAnimInstance>(GetArmMesh()->GetAnimInstance());
	if (bodyAnim)
	{
		bodyAnim->PlayHitMontage();
	}
	if (armAnim)
	{
		armAnim->PlayHitMontage();
	}
}

void ACDCharacter::HandleDamage(float FinalDamage)
{
	if (_attributeSet == nullptr) return;

	float CurShield = _attributeSet->GetShield();
	float CurHealth = _attributeSet->GetHealth();

	if (CurShield > 0.f)
	{
		CurShield = FMath::Clamp(CurShield - FinalDamage, 0.f, 100.f);
		_attributeSet->SetShield(CurShield);
	}
	else
	{
		CurHealth = FMath::Clamp(CurHealth - FinalDamage, 0.f, 100.f);
		_attributeSet->SetHealth(CurHealth);
	}
	
	if (CurHealth == 0.f)
	{
		Multicast_Dead();
	}
	else
	{
		Multicast_Hit();
	}
}
