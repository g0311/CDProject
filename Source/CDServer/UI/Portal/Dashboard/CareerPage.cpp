// Fill out your copyright notice in the Description page of Project Settings.


#include "CareerPage.h"

#include "CDServer/UI/GameStats/GameStatsManager.h"
#include "CDServer/UI/GameStats/MatchHistory/MatchHistory.h"
#include "CDServer/UI/GameStats/PlayerStats/PlayerStats.h"
#include "Components/Button.h"

void UCareerPage::NativeConstruct()
{
	Super::NativeConstruct();
	
	check(IsValid(GameStatsManagerClass));
	GameStatsManager = NewObject<UGameStatsManager>(this, GameStatsManagerClass);
	
	Button_Refresh->OnClicked.AddDynamic(this, &UCareerPage::RefreshStatsButtonClicked);
	RefreshStatsButtonClicked();
	
	GameStatsManager->OnRetrieveMatchStats.AddDynamic(this, &UCareerPage::OnRetrieveMatchStats);
}

void UCareerPage::RefreshStatsButtonClicked()
{
	GameStatsManager->RetrieveMatchStats();
}

void UCareerPage::OnRetrieveMatchStats(const FCDRetrieveMatchStatsResult& result)
{
	MatchHistoryWidget->UpdateScrollBox(result.Matches);
	PlayerStatsWidget->UpdateData(result);
}
