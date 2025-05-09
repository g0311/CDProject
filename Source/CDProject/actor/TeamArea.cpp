// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamArea.h"

#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Controller/CDPlayerController.h"
#include "CDProject/GameMode/RoundGameMode.h"
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

void ATeamArea::Destroyed()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::Destroyed();
}

void ATeamArea::BeginPlay()
{
	Super::BeginPlay();
	AGameMode* CDGameMode=Cast<AGameMode>(GetWorld()->GetAuthGameMode());
	//LockDuration=CDGameMode->
	AreaVolume->OnComponentBeginOverlap.AddDynamic(this, &ATeamArea::OnOverlapBegin);
	GameStartFlag=true;
}

void ATeamArea::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//ShowStore(OtherActor);
}

void ATeamArea::ShowStore(AActor* actor)
{
	if (ACDCharacter* OverlappingCharacter = Cast<ACDCharacter>(actor))
	{
		if (AController* PlayerController = OverlappingCharacter->GetController())
		{
			if (bAreaLocked)
			{
				LockedCharacters.Add(OverlappingCharacter);
				OverlappingCharacter->DisableInput(nullptr);
				if (ACDPlayerController* CDPC = Cast<ACDPlayerController>(PlayerController))
				{
					CDPC->ShowStoreWidget(true);
				}
				GetWorld()->GetTimerManager().SetTimer(LockTimerHandle, this, &ATeamArea::UnlockArea, LockDuration, false);
			}
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick([this, actor]()
			{
				if (IsValid(actor) && IsValid(this))
					this->ShowStore(actor);
			});
		}
	}
}

void ATeamArea::UnlockArea()
{
	bAreaLocked = false; 
	for (ACharacter* Character : LockedCharacters)
	{
		if (Character)
		{
			if (AController* PlayerController = Character->GetController())
			{
				if (ACDPlayerController* CDPC = Cast<ACDPlayerController>(PlayerController))
				{
					CDPC->ShowStoreWidget(false); 
					Character->EnableInput(CDPC);
				}
			}
		}
	}
	LockedCharacters.Empty();
}

