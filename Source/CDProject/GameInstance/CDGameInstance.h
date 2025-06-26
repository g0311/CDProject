// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CDGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class CDPROJECT_API UCDGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	virtual void Init() override;
	void HandleNetworkFailure
		(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
};
