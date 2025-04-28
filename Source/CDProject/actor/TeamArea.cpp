// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamArea.h"

#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "CDProject/GameMode/CDGameMode.h"
#include "Components/BoxComponent.h"


class ACDPlayerController;
// Sets default values
ATeamArea::ATeamArea()
{
	PrimaryActorTick.bCanEverTick = true;
	AreaVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("AreaVolume"));
	SetRootComponent(AreaVolume);
	AreaVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AreaVolume->SetCollisionResponseToAllChannels(ECR_Overlap);
}

void ATeamArea::BeginPlay()
{
	Super::BeginPlay();
	AGameMode* CDGameMode=Cast<AGameMode>(GetWorld()->GetAuthGameMode());
	//LockDuration=CDGameMode->
	AreaVolume->OnComponentBeginOverlap.AddDynamic(this, &ATeamArea::OnOverlapBegin);
}

void ATeamArea::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACDCharacter* OverlappingCharacter = Cast<ACDCharacter>(OtherActor))
	{
		LockedCharacters.Add(OverlappingCharacter);
		OverlappingCharacter->DisableInput(nullptr);
		AController* PlayerController = OverlappingCharacter->GetController();
		ACDPlayerController* CDPC = Cast<ACDPlayerController>(PlayerController);
		if (CDPC)
		{
			CDPC->ShowStoreWidget(true);
			//CDPC->Client_ShowStoreWidget(true);
		}
		if (!GetWorld()->GetTimerManager().IsTimerActive(LockTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(LockTimerHandle, this, &ATeamArea::UnlockArea, LockDuration, false);
		}
	}	
}

void ATeamArea::UnlockArea()
{
	for (ACharacter* Character : LockedCharacters)
	{
		if (Character)
		{
			Character->EnableInput(nullptr);
			AController* PlayerController = Character->GetController();
			ACDPlayerController* CDPC = Cast<ACDPlayerController>(PlayerController);
			CDPC->ShowStoreWidget(false);
			CDPC->Client_ShowStoreWidget(false);
		}
	}
	LockedCharacters.Empty();
}

