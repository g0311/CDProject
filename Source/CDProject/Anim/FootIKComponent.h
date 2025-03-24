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
		UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float		m_fEffectorLocation_Left;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float		m_fEffectorLocation_Right;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator	m_pFootRotation_Left;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator	m_pFootRotation_Right;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float		m_fHipOffset;
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

	IK_TraceInfo IK_FootTrace(float traceDistance, FName socket);

private :
	UPROPERTY()
	class ACharacter* _playerPawn;
	
	//! Owner Characters capsule height
	float	playerCapsuleHeight;
	//! IK Anim Instance Value struct
	FST_IK_AnimValue m_stIKAnimValuse;
	//! IK Active state
	bool m_bActive = false;
	float m_fDeltaTime = 0.0f;
};
