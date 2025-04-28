// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TeamArea.generated.h"

UCLASS()
class CDPROJECT_API ATeamArea : public AActor
{
	GENERATED_BODY()

public:
	ATeamArea();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
						bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void UnlockArea();

private:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* AreaVolume;

	UPROPERTY(EditAnywhere)
	float LockDuration = 10.0f;

	FTimerHandle LockTimerHandle;

	UPROPERTY()
	TArray<ACharacter*> LockedCharacters;
};
