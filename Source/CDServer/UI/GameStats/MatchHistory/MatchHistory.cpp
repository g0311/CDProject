// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchHistory.h"

#include "MatchHistoryLine.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

void UMatchHistory::UpdateScrollBox(const TArray<FCDMatchData>& MatchDatas)
{
	ScrollBox_History->ClearChildren();

	for (auto MatchData : MatchDatas)
	{
		UMatchHistoryLine* HistoryLine = CreateWidget<UMatchHistoryLine>(this, MatchHistoryLineClass, TEXT(""));
		ScrollBox_History->AddChild(HistoryLine);

		switch (MatchData.Iswin)
		{
		case -1:
			HistoryLine->TextBlock_WinLose->SetText(FText::FromString(TEXT("LOSE")));
			break;
		case 0:
			HistoryLine->TextBlock_WinLose->SetText(FText::FromString(TEXT("DRAW")));
			break;
		case 1:
			HistoryLine->TextBlock_WinLose->SetText(FText::FromString(TEXT("WIN")));
			break;
		default:
			break;
		}
		
		FText KillFormatPattern = FText::FromString(TEXT("Kill: {0}"));
		FFormatNamedArguments KillArgs;
		KillArgs.Add(TEXT("0"), FText::AsNumber(MatchData.Kill)); 
		HistoryLine->TextBlock_Kill->SetText(FText::Format(KillFormatPattern, KillArgs));
		
		FText DeathFormatPattern = FText::FromString(TEXT("Death: {0}"));
		FFormatNamedArguments DeathArgs;
		DeathArgs.Add(TEXT("0"), FText::AsNumber(MatchData.Death));
		HistoryLine->TextBlock_Death->SetText(FText::Format(DeathFormatPattern, DeathArgs));
		
		HistoryLine->TextBlock_RoomMap->SetText(FText::FromString(MatchData.Map));
		HistoryLine->TextBlock_RoomMode->SetText(FText::FromString(MatchData.Mode));
	}
}
