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
	
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	void StopBehavior();
	void RestartBehavior();

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBehaviorTree> BehaviorTree;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void OnPossess(APawn* InPawn) override;

	// virtual FGenericTeamId GetGenericTeamId() const override;
private:
	UPROPERTY(EditDefaultsOnly, Category="AI")
	uint8 TeamID=0;
	

	
	
};
