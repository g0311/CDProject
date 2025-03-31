// Fill out your copyright notice in the Description page of Project Settings.


#include "FootIKComponent.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UFootIKComponent::UFootIKComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFootIKComponent::BeginPlay()
{
	Super::BeginPlay();
	_playerPawn = Cast<ACharacter>(GetOwner());
}

void UFootIKComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UFootIKComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	IK_Update(DeltaTime);
	//IK_ResetVars(DeltaTime);
	//IK_Debug();
}

void UFootIKComponent::IK_Update(float deltaTime)
{
	IK_TraceInfo traceLef = IK_FootTrace(55.f, "foot_l");
	IK_TraceInfo traceRig = IK_FootTrace(55.f, "foot_r");

	IK_Update_FootRotation(deltaTime, NormalToRotator(traceLef.impactLocation), &_outValue._footRotation_Left, 22.f);
	IK_Update_FootRotation(deltaTime, NormalToRotator(traceRig.impactLocation), &_outValue._footRotation_Right, 22.f);
	
	//UKismetSystemLibrary::PrintString(GetWorld(), "traceLef.offset : " + FString::SanitizeFloat(traceLef.offset), true, false, FLinearColor::Red, 0.0f);
	//UKismetSystemLibrary::PrintString(GetWorld(), "traceRig offset : " + FString::SanitizeFloat(traceRig.offset), true, false, FLinearColor::Red, 0.0f);
	
	float fHipsOffset = UKismetMathLibrary::Min(traceLef.offset, traceRig.offset);
	IK_Update_FootOffset(deltaTime, fHipsOffset, &_outValue._hipOffset, 17.f);
	
	IK_Update_FootOffset(deltaTime, traceLef.offset - fHipsOffset, &_outValue._effectorLocation_Left, 22.f);
	IK_Update_FootOffset(deltaTime, -1*(traceRig.offset - fHipsOffset), &_outValue._effectorLocation_Right, 22.f);
}

void UFootIKComponent::IK_Debug()
{
	UKismetSystemLibrary::PrintString(GetWorld(), "Foot_L Rotation : " + _outValue._footRotation_Left.ToString(), true, false, FLinearColor::Red, 0.0f);
	UKismetSystemLibrary::PrintString(GetWorld(), "Foot_R Rotation : " + _outValue._footRotation_Right.ToString(), true, false, FLinearColor::Red, 0.0f);
	UKismetSystemLibrary::PrintString(GetWorld(), "Foot_L EffectorLocation(Z) : " + FString::SanitizeFloat(_outValue._effectorLocation_Left), true, false, FLinearColor::Red, 0.0f);
	UKismetSystemLibrary::PrintString(GetWorld(), "Foot_R EffectorLocation(Z) : " + FString::SanitizeFloat(_outValue._effectorLocation_Right), true, false, FLinearColor::Red, 0.0f);
	UKismetSystemLibrary::PrintString(GetWorld(), "HipOffset(Z) : " + FString::SanitizeFloat(_outValue._hipOffset), true, false, FLinearColor::Red, 0.0f);
}

IK_TraceInfo UFootIKComponent::IK_FootTrace(float traceDistance, FName socket)
{
	IK_TraceInfo traceInfo;
 
	//! Set Linetraces startpoint and end point
	FVector socketLocation = _playerPawn->GetMesh()->GetSocketLocation(socket);
	FVector line_Start = FVector(socketLocation.X, socketLocation.Y, _playerPawn->GetActorLocation().Z);
	FVector line_End = FVector(socketLocation.X, socketLocation.Y
		, (_playerPawn->GetActorLocation().Z - _playerPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()) - traceDistance);
 
	//! Process Line Trace
	FHitResult hitResult;
	TArray<AActor*> ignore;
	ignore.Add(GetOwner());
 
	bool bDebug = false;
	EDrawDebugTrace::Type eDebug = EDrawDebugTrace::None;
	if (bDebug == true) eDebug = EDrawDebugTrace::ForOneFrame;
 
	bool bResult = UKismetSystemLibrary::LineTraceSingle(GetWorld(), line_Start, line_End,
		UEngineTypes::ConvertToTraceType(ECC_Visibility), true, ignore, eDebug, hitResult, true);
 
	//! Set ImpactNormal and Offset from HitResult
	traceInfo.impactLocation = hitResult.ImpactNormal;
	if (hitResult.IsValidBlockingHit() == true)
	{
		float impactLength = (hitResult.ImpactPoint - hitResult.TraceEnd).Size();
		traceInfo.offset = 5.0f + (impactLength - traceDistance);
	}
	else
	{
		traceInfo.offset = 0.0f;
	}
	return traceInfo;
}

void UFootIKComponent::IK_Update_FootOffset(float deltaTime, float targetValue, float* effectorValue, float interpSpeed)
{
	float fInterpValue = UKismetMathLibrary::FInterpTo(*effectorValue, targetValue, deltaTime, interpSpeed);
	*effectorValue = fInterpValue;
}

void UFootIKComponent::IK_Update_FootRotation(float deltaTime, FRotator targetValue, FRotator* footRotatorValue, float interpSpeed)
{
	FRotator pInterpRotator = UKismetMathLibrary::RInterpTo(*footRotatorValue, targetValue, deltaTime, interpSpeed);
	*footRotatorValue = pInterpRotator;
}

void UFootIKComponent::IK_Update_CapsuleHalfHeight(float deltaTime, float hipsShifts, bool resetDefault)
{
	UCapsuleComponent* pCapsule = _playerPawn->GetCapsuleComponent();
	if (pCapsule == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("IK : Capsule is NULL"));
		return;
	}

	//! Get Half Height of capsule component
	float capsuleHalf = 0.0f;
	if (resetDefault == true)
	{
		capsuleHalf = _playerPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}
	else
	{
		float halfAbsSize = UKismetMathLibrary::Abs(hipsShifts) * 0.5f;
		capsuleHalf = _playerPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - halfAbsSize;
	}

	//! Set capsule component height with FInterpTo 
	float scaledCapsuleHalfHeight = pCapsule->GetScaledCapsuleHalfHeight();
	float interpValue = UKismetMathLibrary::FInterpTo(scaledCapsuleHalfHeight, capsuleHalf, deltaTime, 13.f);

	pCapsule->SetCapsuleHalfHeight(interpValue, true);
}

FRotator UFootIKComponent::NormalToRotator(FVector vector)
{
	float fAtan2_1 = UKismetMathLibrary::DegAtan2(vector.Y, vector.Z);
	float fAtan2_2 = UKismetMathLibrary::DegAtan2(vector.X, vector.Z);
	fAtan2_2 *= -1.0f;
	FRotator pResult = FRotator(fAtan2_2, 0.0f, fAtan2_1);
 
	return pResult;
}
