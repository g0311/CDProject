// Fill out your copyright notice in the Description page of Project Settings.


#include "C4Area.h"

#include "CDProject/Character/CDCharacter.h"
#include "CDProject/Component/CombatComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AC4Area::AC4Area()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	AreaVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("AreaVolume"));
	SetRootComponent(AreaVolume);
	AreaVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AreaVolume->SetCollisionResponseToAllChannels(ECR_Overlap);
}

// Called when the game starts or when spawned
void AC4Area::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		AreaVolume->OnComponentBeginOverlap.AddDynamic(this, &AC4Area::OnOverlapBegin);
		AreaVolume->OnComponentEndOverlap.AddDynamic(this, &AC4Area::OnOverlapEnd);
	}
}

void AC4Area::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACDCharacter* OverlappingCharacter = Cast<ACDCharacter>(OtherActor))
	{
		if (UCombatComponent* combat = OverlappingCharacter->GetCombatComponent())
		{
			combat->SetC4Area(true);
		}
	}
}

void AC4Area::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (ACDCharacter* OverlappingCharacter = Cast<ACDCharacter>(OtherActor))
	{
		if (UCombatComponent* combat = OverlappingCharacter->GetCombatComponent())
		{
			combat->SetC4Area(false);
		}
	}
}

