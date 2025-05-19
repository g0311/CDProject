// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CDAIController.generated.h"

UCLASS()
class CDPROJECT_API ACDAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACDAIController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	class UBehaviorTreeComponent* BehaviorTreeComponent;

	
};
