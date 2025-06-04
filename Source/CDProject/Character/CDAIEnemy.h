// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CDCharacter.h"
#include "CDAIEnemy.generated.h"

UCLASS()
class CDPROJECT_API ACDAIEnemy : public ACDCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACDAIEnemy();
	virtual void PossessedBy(AController* NewController) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category="AI")
	class UBehaviorTree* BehaviorTree;
	
	UPROPERTY()
	class ACDAIController* CDAIController;
};
