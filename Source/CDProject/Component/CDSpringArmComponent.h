// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "CDSpringArmComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CDPROJECT_API UCDSpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()
public:
	FORCEINLINE void SetFakeRotation(FRotator Rotation) {FakeRotationForRemote = Rotation;}
	FORCEINLINE FRotator GetFakeRotation() {return FakeRotationForRemote;}

protected:
	virtual void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FRotator FakeRotationForRemote;
};
