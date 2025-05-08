// Fill out your copyright notice in the Description page of Project Settings.


#include "ModeSelect.h"

#include "CDProject/GameMode/RoundGameMode.h"
#include "Components/Button.h"
#include "GameFramework/GameModeBase.h"

void UModeSelect::NativeConstruct()
{
	Super::NativeConstruct();
	if (Demolition)
	{
		Demolition->OnClicked.AddDynamic(this, &UModeSelect::OnDemolitionClicked);
		
	}
	if (DeathMatch)
	{
		DeathMatch->OnClicked.AddDynamic(this, &UModeSelect::OnDeathMatchClicked);
	}
}

void UModeSelect::OnDemolitionClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (PC->HasAuthority())
		{
			FString TravelURL = FString::Printf(TEXT("/Game/Maps/DemolitionMap?game=%s?listen"), *DemolitionModeClass->GetPathName());
			PC->GetWorld()->ServerTravel(TravelURL);
		}
	}
	RemoveFromParent();
}

void UModeSelect::OnDeathMatchClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (PC->HasAuthority())
		{
			FString TravelURL = FString::Printf(TEXT("/Game/Maps/DeathMatchMap?game=%s?listen"), *DeathMatchModeClass->GetPathName());
			PC->GetWorld()->ServerTravel(TravelURL);
		}
	}
	RemoveFromParent();
}
