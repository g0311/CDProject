// Fill out your copyright notice in the Description page of Project Settings.


#include "FootIKComponent.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
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

	IK_TraceInfo traceLef = IK_FootTrace(55.f, "foot_l");
	IK_TraceInfo traceRig = IK_FootTrace(55.f, "foot_r");
}

IK_TraceInfo UFootIKComponent::IK_FootTrace(float traceDistance, FName socket)
{
	IK_TraceInfo traceInfo;
 
	//! Set Linetraces startpoint and end point
	FVector socketLocation = _playerPawn->GetMesh()->GetSocketLocation(socket);
	FVector line_Start = FVector(socketLocation.X, socketLocation.Y, _playerPawn->GetActorLocation().Z);
	FVector line_End = FVector(socketLocation.X, socketLocation.Y
		, (_playerPawn->GetActorLocation().Z - _playerPawn->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()*2) - traceDistance);
 
	//! Process Line Trace
	FHitResult hitResult;
	TArray<AActor*> ignore;
	ignore.Add(GetOwner());
 
	bool bDebug = true;
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

