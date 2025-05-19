// Fill out your copyright notice in the Description page of Project Settings.


#include "CDAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"


// Sets default values
ACDAIController::ACDAIController()
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComponent");
	check(Blackboard);
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");
	check(BehaviorTreeComponent);
}

// Called when the game starts or when spawned
void ACDAIController::BeginPlay()
{
	Super::BeginPlay();
	
}

