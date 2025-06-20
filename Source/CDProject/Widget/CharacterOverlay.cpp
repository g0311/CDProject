// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterOverlay.h"

#include "KillLogLine.h"
#include "Components/TextBlock.h"

void UCharacterOverlay::CreateKillLog(const FString& Killer, const FString& Victim)
{
	if (KillLogLineClass)
	{
		UKillLogLine* KillLogLine = CreateWidget<UKillLogLine>(this, KillLogLineClass);
		KillLogLine->TextBlock_Killer->SetText(FText::FromString(Killer));
		KillLogLine->TextBlock_Victim->SetText(FText::FromString(Victim));
	}
}
