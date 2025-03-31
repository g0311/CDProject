// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FootIKComponent.generated.h"


struct IK_TraceInfo
{
	float offset;
	FVector impactLocation;
};

USTRUCT(Atomic, BlueprintType)
struct FST_IK_AnimValue
{
	GENERATED_BODY()

	public :
	FST_IK_AnimValue() 
		: _effectorLocation_Left(0.0f)
		, _effectorLocation_Right(0.0f)
		, _footRotation_Left(FRotator::ZeroRotator)
		, _footRotation_Right(FRotator::ZeroRotator)
		, _hipOffset(0.0f)
	{
	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float		_effectorLocation_Left;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float		_effectorLocation_Right;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator	_footRotation_Left;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator	_footRotation_Right;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float		_hipOffset;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CDPROJECT_API UFootIKComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFootIKComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void IK_Update(float deltaTime);
	void IK_Debug();
	IK_TraceInfo IK_FootTrace(float traceDistance, FName socket);
	void IK_Update_FootOffset(float deltaTime, float targetValue, float* effectorValue, float interpSpeed);
	void IK_Update_FootRotation(float fDeltaTime, FRotator pTargetValue, FRotator * pFootRotatorValue, float fInterpSpeed);
	void IK_Update_CapsuleHalfHeight(float deltaTime, float hipsShifts, bool resetDefault);
	FRotator NormalToRotator(FVector pVector);

	FST_IK_AnimValue _outValue;
private:
	UPROPERTY()
	class ACharacter* _playerPawn;
};
