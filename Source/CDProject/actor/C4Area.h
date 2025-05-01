// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "C4Area.generated.h"

UCLASS()
class CDPROJECT_API AC4Area : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AC4Area();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
