// Fill out your copyright notice in the Description page of Project Settings.


#include "CDAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CDProject/Character/CDCharacter.h"
#include "CDProject/PlayerState/CDPlayerState.h"
#include "CDServer/Player/Team.h"
#include "GameFramework/GameModeBase.h"

// Sets default values
ACDAIController::ACDAIController()
{
	
}

ETeamAttitude::Type ACDAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const FGenericTeamId MyTeamId = GetGenericTeamId();
	const IGenericTeamAgentInterface* OtherAgent = Cast<IGenericTeamAgentInterface>(&Other);
	if (!OtherAgent)
	{
		return ETeamAttitude::Neutral;
	}
	const FGenericTeamId OtherTeamId = OtherAgent->GetGenericTeamId();
	if (MyTeamId == OtherTeamId)
	{
		return ETeamAttitude::Friendly;
	}
	else if (OtherTeamId == FGenericTeamId::NoTeam)
	{
		return ETeamAttitude::Neutral;
	}
	else
	{
		return ETeamAttitude::Hostile;
	}
	// if (const APawn* OtherPawn = Cast<APawn>(&Other)) {
	//
	// 	if (const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController()))
	// 	{
	// 		return Super::GetTeamAttitudeTowards(*OtherPawn->GetController());
	// 	}
	// }
}

void ACDAIController::StopBehavior()
{
	if (UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		BTComp->StopTree();
		StopMovement();
		SetFocus(nullptr);
	}
}

void ACDAIController::RestartBehavior()
{
	if (UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		if (IsValid(BehaviorTree))
			BTComp->StartTree(*BehaviorTree);
	}
}

// Called when the game starts or when spawned
void ACDAIController::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void ACDAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (ACDPlayerState* APS=Cast<ACDPlayerState>(PlayerState))
	{
		int32 TeamIdFromPS = APS->GetTeam()==ETeam::ET_RedTeam?1:2; 
		SetGenericTeamId(FGenericTeamId(TeamIdFromPS));
	}
}

