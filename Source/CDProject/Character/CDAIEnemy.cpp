// Fill out your copyright notice in the Description page of Project Settings.


#include "CDAIEnemy.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CDProject/AI/CDAIController.h"


// Sets default values
ACDAIEnemy::ACDAIEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ACDAIEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (!HasAuthority()) return;
	CDAIController = Cast<ACDAIController>(NewController);
	CDAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	CDAIController->RunBehaviorTree(BehaviorTree);
}


// Called when the game starts or when spawned
void ACDAIEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACDAIEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACDAIEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

