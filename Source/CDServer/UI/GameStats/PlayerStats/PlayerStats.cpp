// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStats.h"

#include "Components/TextBlock.h"

void UPlayerStats::UpdateData(const FCDRetrieveMatchStatsResult& Stats)
{
	TextBlock_Kill->SetText(FText::AsNumber(Stats.Kill));
	TextBlock_Death->SetText(FText::AsNumber(Stats.Death));
	TextBlock_HeadShot->SetText(FText::AsPercent(static_cast<double>(Stats.Headshot) / Stats.shot));
	TextBlock_Win->SetText(FText::AsNumber(Stats.Totalwin));
	TextBlock_Lose->SetText(FText::AsNumber(Stats.Totallose));
	TextBlock_Draw->SetText(FText::AsNumber(Stats.Totaldraw));
}
