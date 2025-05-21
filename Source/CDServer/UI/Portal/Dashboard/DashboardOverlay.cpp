// Fill out your copyright notice in the Description page of Project Settings.


#include "DashboardOverlay.h"

#include "CareerPage.h"
#include "GamePage.h"
#include "LeaderBoardPage.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

void UDashboardOverlay::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button_Game->OnClicked.AddDynamic(this, &UDashboardOverlay::ShowGamePage);
	Button_Career->OnClicked.AddDynamic(this, &UDashboardOverlay::ShowCareerPage);
	Button_LeaderBoard->OnClicked.AddDynamic(this, &UDashboardOverlay::ShowLeaderBoardPage);

	ShowGamePage();
}

void UDashboardOverlay::ShowGamePage()
{
	WidgetSwitcher->SetActiveWidget(GamePage);
}

void UDashboardOverlay::ShowCareerPage()
{
	WidgetSwitcher->SetActiveWidget(CareerPage);
}

void UDashboardOverlay::ShowLeaderBoardPage()
{
	WidgetSwitcher->SetActiveWidget(LeaderBoardPage);
}
