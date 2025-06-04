// Fill out your copyright notice in the Description page of Project Settings.


#include "PrivateSessionsWidget.h"

#include "PrivateSessionLine.h"
#include "CDServer/Data/Map/MapData.h"
#include "CDServer/UI/HTTP/HTTPRequestTypes.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"

void UPrivateSessionsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	TextBox_ForSearch->OnTextChanged.AddDynamic(this, &UPrivateSessionsWidget::FilterScrollBox);
	TextBox_RoomName->OnTextChanged.AddDynamic(this, &UPrivateSessionsWidget::EnableCreateButton);
	
	for (auto Mode : MapData->GetModes())
	{
		Dropdown_Map->AddOption(Mode);
	}
	Dropdown_Mode->SetSelectedIndex(0);
	OnDropdownSelectionChanged(FString(), ESelectInfo::Type());
}

void UPrivateSessionsWidget::UpdateSessions(const FCDDescribeGameSessionResult& DescribeGameSessionResult)
{
	Button_Refresh->SetIsEnabled(true);
	SelectedSessionLine = nullptr;
	ScrollBox_Sessions->ClearChildren();

	TArray<FCDGameSession> Sessions = DescribeGameSessionResult.GameSessions;
	for (auto Session : Sessions)
	{
		UPrivateSessionLine* SessionLine = CreateWidget<UPrivateSessionLine>(this, PrivateSessionLineClass, TEXT("Session Line"));
		ScrollBox_Sessions->AddChild(SessionLine);

		SessionLine->parent = this;
		SessionLine->SetGameSessionId(Session.GameSessionId);
		
		SessionLine->TextBlock_RoomName->SetText(FText::FromString(Session.Name));
		for (auto& property : Session.GameProperties)
		{
			if (property.Key == TEXT("Mode"))
			{
				SessionLine->TextBlock_RoomMode->SetText(FText::FromString(property.Value));
			}
		}
		SessionLine->TextBlock_RoomCount->SetText(FText::FromString(Session.CurrentPlayerSessionCount + " / " + Session.MaximumPlayerSessionCount));

		if (!Session.CurrentPlayerSessionCount.Equals(Session.MaximumPlayerSessionCount))
		{
			SessionLine->Button_Session->OnClicked.AddDynamic(SessionLine, &UPrivateSessionLine::OnSessionClicked);
			SessionLine->Button_Session->OnHovered.AddDynamic(SessionLine, &UPrivateSessionLine::OnHovered);
			SessionLine->Button_Session->OnUnhovered.AddDynamic(SessionLine, &UPrivateSessionLine::OnUnhovered);
		}
	}
}

FString UPrivateSessionsWidget::GetCurGameSessionId()
{
	if (IsValid(SelectedSessionLine))
	{
		return SelectedSessionLine->GetGameSessionId();
	}

	return FString();
}


void UPrivateSessionsWidget::SetStatusMessage(const FString& Message, bool bShouldResetWidgets)
{
	TextBlock_Status->SetText(FText::FromString(Message));
	if (bShouldResetWidgets)
	{
		Button_Join->SetIsEnabled(true);
	}
}

void UPrivateSessionsWidget::SetCreateStatusMessage(const FString& Message, bool bShouldResetWidgets)
{
	TextBlock_Create_Status->SetText(FText::FromString(Message));
	if (bShouldResetWidgets)
	{
		Button_Create->SetIsEnabled(true);
		Button_Quit->SetIsEnabled(true);
	}
}

void UPrivateSessionsWidget::FilterScrollBox(const FText& text)
{
	FString FilterString = text.ToString().ToLower();

	for (const auto& widget : ScrollBox_Sessions->GetAllChildren())
	{
		UPrivateSessionLine* SessionLine = Cast<UPrivateSessionLine>(widget);
		if (SessionLine)
		{
			FString SessionName = SessionLine->TextBlock_RoomName->GetText().ToString().ToLower();
			if (SessionName.Contains(FilterString))
			{
				SessionLine->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				SessionLine->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}
}

void UPrivateSessionsWidget::EnableCreateButton(const FText& Text)
{
	if (Text.IsEmpty())
	{
		Button_Create->SetIsEnabled(false);
	}
	else
	{
		Button_Create->SetIsEnabled(true);
	}
}

void UPrivateSessionsWidget::OnDropdownSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	Dropdown_Map->ClearOptions();
	for (auto map : MapData->GetMapsFromMode(Dropdown_Mode->GetSelectedOption()))
	{
		Dropdown_Map->AddOption(map);
	}
	Dropdown_Map->SetSelectedIndex(0);
}
