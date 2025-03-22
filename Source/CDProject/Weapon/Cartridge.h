// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cartridge.generated.h"

UCLASS()
class CDPROJECT_API ACartridge : public AActor
{
	GENERATED_BODY()

public:
	ACartridge();
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CartridgeMesh;

	UPROPERTY(EditAnywhere)
	class USoundCue* FallSound;
	
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit);


	
	
};
