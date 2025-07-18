// Fill out your copyright notice in the Description page of Project Settings.


#include "MatchHistory.h"

#include "MatchHistoryLine.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

void UMatchHistory::UpdateScrollBox(const TArray<FCDMatchData>& MatchDatas)
{
	ScrollBox_History->ClearChildren();
	
	FLinearColor SemiTransparentRed = FLinearColor(1.f, 0.f, 0.f, 0.3f);
	FLinearColor SemiTransparentBlue = FLinearColor(0.f, 0.f, 1.f, 0.3f);
	FLinearColor SemiTransparentGreen = FLinearColor(0.f, 1.f, 0.f, 0.3f);
	for (int i = MatchDatas.Num() - 1; i >= 0; i--)
	{
		auto MatchData = MatchDatas[i];
		UMatchHistoryLine* HistoryLine = CreateWidget<UMatchHistoryLine>(this, MatchHistoryLineClass, TEXT(""));
		ScrollBox_History->AddChild(HistoryLine);
		
		switch (MatchData.Iswin)
		{
		case -1:
			HistoryLine->TextBlock_WinLose->SetText(FText::FromString(TEXT("LOSE")));
			HistoryLine->Image_BackGround->SetBrushTintColor(FSlateColor(SemiTransparentRed));
			break;
		case 0:
			HistoryLine->TextBlock_WinLose->SetText(FText::FromString(TEXT("DRAW")));
			HistoryLine->Image_BackGround->SetBrushTintColor(FSlateColor(SemiTransparentBlue));
			break;
		case 1:
			HistoryLine->TextBlock_WinLose->SetText(FText::FromString(TEXT("WIN")));
			HistoryLine->Image_BackGround->SetBrushTintColor(FSlateColor(SemiTransparentGreen));
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
