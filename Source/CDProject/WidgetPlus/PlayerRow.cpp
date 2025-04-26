// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerRow.h"

#include "CDProject/PlayerState/CDPlayerState.h"
#include "Components/TextBlock.h"

void UPlayerRow::UpdateRow(FText PlayerName, int32 Kills, int32 Deaths, int32 Gold)
{
	if (PlayerNameText) PlayerNameText->SetText(PlayerName);
	if (KillText) KillText->SetText(FText::AsNumber(Kills));
	if (DeathText) DeathText->SetText(FText::AsNumber(Deaths));
	if (GoldText) GoldText->SetText(FText::AsNumber(Gold));
}

void UPlayerRow::Setup(class ACDPlayerState* CDPlayerState)
{
	if (!CDPlayerState) return;

	if (PlayerNameText)
	{
		PlayerNameText->SetText(CDPlayerState->GetPlayerName());
	}
	if (KillText)
	{
		KillText->SetText(FText::AsNumber(CDPlayerState->GetKills()));
	}
	if (DeathText)
	{
		DeathText->SetText(FText::AsNumber(CDPlayerState->GetDeaths()));
	}
	if (GoldText)
	{
		GoldText->SetText(FText::AsNumber(CDPlayerState->GetGold()));
	}
}
