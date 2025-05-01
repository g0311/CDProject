// Fill out your copyright notice in the Description page of Project Settings.


#include "C4Area.h"


// Sets default values
AC4Area::AC4Area()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AC4Area::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AC4Area::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

