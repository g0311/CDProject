// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerRow.h"

#include "CDProject/PlayerState/CDPlayerState.h"
#include "Components/TextBlock.h"

void UPlayerRow::Setup(ACDPlayerState* InPlayerState)
{
	PSRef = InPlayerState;

	if (PSRef)
	{
		PSRef->OnScoreUpdated.AddDynamic(this, &UPlayerRow::UpdateRow);
		PSRef->OnGoldUpdated.AddDynamic(this, &UPlayerRow::UpdateGold);
		UpdateRow(); 
	}
}

void UPlayerRow::UpdateRow()
{
	if (!PSRef) return;
	
	if (PlayerNameText)
		PlayerNameText->SetText(FText::FromString(PSRef->GetPlayerName()));
	
	if (KillText)
		KillText->SetText(FText::AsNumber(PSRef->GetKills()));

	if (DeathText)
		DeathText->SetText(FText::AsNumber(PSRef->GetDeaths()));

	if (GoldText)
		GoldText->SetText(FText::AsNumber(PSRef->GetGold()));
}

void UPlayerRow::UpdateGold(int32 NewGold)
{
	if (GoldText)
		GoldText->SetText(FText::AsNumber(NewGold));
}