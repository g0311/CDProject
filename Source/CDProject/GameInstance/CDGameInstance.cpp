// Fill out your copyright notice in the Description page of Project Settings.


#include "CDGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UCDGameInstance::Init()
{
	Super::Init();
	
	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UCDGameInstance::HandleNetworkFailure);
	}
}

void UCDGameInstance::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
	const FString& ErrorString)
{
	UGameplayStatics::OpenLevel(this, TEXT("/Game/Maps/ClientDefaultLevel"));
}
