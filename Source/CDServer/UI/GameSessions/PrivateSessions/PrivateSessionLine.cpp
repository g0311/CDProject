// Fill out your copyright notice in the Description page of Project Settings.


#include "PrivateSessionLine.h"

#include "PrivateSessionsWidget.h"
#include "Components/Image.h"

void UPrivateSessionLine::OnSessionClicked()
{
	if (parent->SelectedSessionLine)
	{
		parent->SelectedSessionLine->SetSelected(false);
	}
	parent->SelectedSessionLine = this;
	parent->SelectedSessionLine->SetSelected(true);
}

void UPrivateSessionLine::OnHovered()
{
	if (!bIsSelected)
		Image_BackGround->SetBrushTintColor(FSlateColor(FLinearColor(0.15f,0.15f,0.15f,1.f)));
}

void UPrivateSessionLine::OnUnhovered()
{
	if (!bIsSelected)
		Image_BackGround->SetBrushTintColor(FSlateColor(FLinearColor(0.f,0.f,0.f,1.f)));
}

void UPrivateSessionLine::SetSelected(bool selected)
{
	bIsSelected = selected;
	if (selected)
	{
		Image_BackGround->SetBrushTintColor(FSlateColor(FLinearColor(0.2f,0.2f,0.2f,1.f)));
	}
	else
	{
		Image_BackGround->SetBrushTintColor(FSlateColor(FLinearColor(0.f,0.f,0.f,1.f)));
	}
}

void UPrivateSessionLine::SetGameSessionId(const FString& id)
{
	GameSessionId = id;
}

const FString& UPrivateSessionLine::GetGameSessionId()
{
	return GameSessionId;
}